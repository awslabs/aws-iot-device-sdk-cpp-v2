/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/crt/Api.h>
#include <aws/crt/UUID.h>
#include <aws/crt/io/HostResolver.h>

#include <aws/iot/MqttClient.h>

#include <aws/iotsecuretunneling/IotSecureTunnelingClient.h>
#include <aws/iotsecuretunneling/SecureTunnelingNotifyResponse.h>
#include <aws/iotsecuretunneling/SubscribeToTunnelsNotifyRequest.h>
#include <iostream>

#include <algorithm>
#include <condition_variable>
#include <iostream>
#include <mutex>

using namespace std;
using namespace Aws::Iot;
using namespace Aws::Crt;
using namespace Aws::Crt::Mqtt;
using namespace Aws::Iotsecuretunneling;

static void s_printHelp()
{
    fprintf(stdout, "Usage:\n");
    fprintf(
        stdout,
        "tunnel-notification --endpoint <endpoint> --cert <path to cert>"
        " --key <path to key> --ca_file <optional: path to custom ca>"
        " --thing_name <thing name> \n\n");
    fprintf(stdout, "endpoint: the endpoint of the mqtt server not including a port\n");
    fprintf(stdout, "cert: path to your client certificate in PEM format\n");
    fprintf(stdout, "key: path to your key in PEM format\n");
    fprintf(
        stdout,
        "ca_file: Optional, if the mqtt server uses a certificate that's not already"
        " in your trust store, set this.\n");
    fprintf(stdout, "\tIt's the path to a CA file in PEM format\n");
    fprintf(stdout, "thing_name: the name of your IOT thing\n");
}

bool s_cmdOptionExists(char **begin, char **end, const String &option)
{
    return std::find(begin, end, option) != end;
}

char *s_getCmdOption(char **begin, char **end, const String &option)
{
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    /************************ Setup the Lib ****************************/
    /*
     * Do the global initialization for the API.
     */
    ApiHandle apiHandle;

    String endpoint;
    String certificatePath;
    String keyPath;
    String caFile;
    String clientId(String("test-") + Aws::Crt::UUID().ToString());
    String thingName;

    /*********************** Parse Arguments ***************************/
    if (!(s_cmdOptionExists(argv, argv + argc, "--endpoint") && s_cmdOptionExists(argv, argv + argc, "--cert") &&
          s_cmdOptionExists(argv, argv + argc, "--key") && s_cmdOptionExists(argv, argv + argc, "--thing_name")))
    {
        s_printHelp();
        return 0;
    }

    endpoint = s_getCmdOption(argv, argv + argc, "--endpoint");
    certificatePath = s_getCmdOption(argv, argv + argc, "--cert");
    keyPath = s_getCmdOption(argv, argv + argc, "--key");
    thingName = s_getCmdOption(argv, argv + argc, "--thing_name");

    if (s_cmdOptionExists(argv, argv + argc, "--ca_file"))
    {
        caFile = s_getCmdOption(argv, argv + argc, "--ca_file");
    }

    /********************** Now Setup an Mqtt Client ******************/
    /*
     * You need an event loop group to process IO events.
     * If you only have a few connections, 1 thread is ideal
     */
    Io::EventLoopGroup eventLoopGroup(1);
    if (!eventLoopGroup)
    {
        fprintf(
            stderr, "Event Loop Group Creation failed with error %s\n", ErrorDebugString(eventLoopGroup.LastError()));
        exit(-1);
    }

    Io::DefaultHostResolver defaultHostResolver(eventLoopGroup, 2, 30);
    Io::ClientBootstrap bootstrap(eventLoopGroup, defaultHostResolver);

    if (!bootstrap)
    {
        fprintf(stderr, "ClientBootstrap failed with error %s\n", ErrorDebugString(bootstrap.LastError()));
        exit(-1);
    }

    auto clientConfigBuilder = Aws::Iot::MqttClientConnectionConfigBuilder(certificatePath.c_str(), keyPath.c_str());
    clientConfigBuilder.WithEndpoint(endpoint);
    if (!caFile.empty())
    {
        clientConfigBuilder.WithCertificateAuthority(caFile.c_str());
    }
    auto clientConfig = clientConfigBuilder.Build();

    if (!clientConfig)
    {
        fprintf(
            stderr,
            "Client Configuration initialization failed with error %s\n",
            ErrorDebugString(clientConfig.LastError()));
        exit(-1);
    }

    /*
     * Now Create a client. This can not throw.
     * An instance of a client must outlive its connections.
     * It is the users responsibility to make sure of this.
     */
    Aws::Iot::MqttClient mqttClient(bootstrap);

    /*
     * Since no exceptions are used, always check the bool operator
     * when an error could have occurred.
     */
    if (!mqttClient)
    {
        fprintf(stderr, "MQTT Client Creation failed with error %s\n", ErrorDebugString(mqttClient.LastError()));
        exit(-1);
    }

    /*
     * Now create a connection object. Note: This type is move only
     * and its underlying memory is managed by the client.
     */
    auto connection = mqttClient.NewConnection(clientConfig);

    if (!*connection)
    {
        fprintf(stderr, "MQTT Connection Creation failed with error %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    /*
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::promise<bool> connectionCompletedPromise;
    std::promise<void> connectionClosedPromise;

    /*
     * This will execute when an mqtt connect has completed or failed.
     */
    auto onConnectionCompleted = [&](Mqtt::MqttConnection &, int errorCode, Mqtt::ReturnCode returnCode, bool) {
        if (errorCode)
        {
            fprintf(stdout, "Connection failed with error %s\n", ErrorDebugString(errorCode));
            connectionCompletedPromise.set_value(false);
        }
        else
        {
            fprintf(stdout, "Connection completed with return code %d\n", returnCode);
            connectionCompletedPromise.set_value(true);
        }
    };

    /*
     * Invoked when a disconnect message has completed.
     */
    auto onDisconnect = [&](Mqtt::MqttConnection & /*conn*/) {
        {
            fprintf(stdout, "Disconnect completed\n");
            connectionClosedPromise.set_value();
        }
    };

    connection->OnConnectionCompleted = std::move(onConnectionCompleted);
    connection->OnDisconnect = std::move(onDisconnect);

    /*
     * Actually perform the connect dance.
     */
    fprintf(stdout, "Connecting...\n");
    if (!connection->Connect(clientId.c_str(), true, 0))
    {
        fprintf(stderr, "MQTT Connection failed with error %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    auto onSubscribeToTunnelsNotifyResponse = [&](Aws::Iotsecuretunneling::SecureTunnelingNotifyResponse *response,
                                                  int ioErr) -> void {
        if (ioErr == 0)
        {
            fprintf(stdout, "Received MQTT Tunnel Notification\n");

            std::string clientAccessToken = response->ClientAccessToken->c_str();
            std::string clientMode = response->ClientMode->c_str();
            std::string region = response->Region->c_str();

            fprintf(
                stdout,
                "Recv: Token:%s, Mode:%s, Region:%s\n",
                clientAccessToken.c_str(),
                clientMode.c_str(),
                region.c_str());

            size_t nServices = response->Services->size();
            if (nServices <= 0)
            {
                fprintf(stdout, "No service requested\n");
            }
            else
            {
                std::string service = response->Services->at(0).c_str();
                fprintf(stdout, "Requested service=%s\n", service.c_str());

                if (nServices > 1)
                {
                    fprintf(stdout, "Multiple services not supported yet\n");
                }
            }
        }
        else
        {
            fprintf(stderr, "MQTT Connection failed with error %d\n", ioErr);
        }
        /* Disconnect */
        if (connection->Disconnect())
        {
            connectionClosedPromise.get_future().wait();
        }
    };

    auto OnSubscribeComplete = [&](int ioErr) -> void {
        if (ioErr)
        {
            fprintf(stderr, "MQTT Connection failed with error %d\n", ioErr);
        }
        else
        {
            fprintf(stdout, "Subscribe successfully\n");
        }
    };

    if (connectionCompletedPromise.get_future().get())
    {
        SubscribeToTunnelsNotifyRequest request;
        request.ThingName = thingName;

        IotSecureTunnelingClient client(connection);
        client.SubscribeToTunnelsNotify(
            request, AWS_MQTT_QOS_AT_LEAST_ONCE, onSubscribeToTunnelsNotifyResponse, OnSubscribeComplete);
    }
    while (1)
    {
        continue;
    }
}
