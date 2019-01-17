/*
 * Copyright 2010-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */
#include <aws/crt/Api.h>
#include <aws/crt/JsonObject.h>
#include <aws/crt/UUID.h>

#include <aws/iotshadow/ErrorResponse.h>
#include <aws/iotshadow/IotShadowClient.h>
#include <aws/iotshadow/ShadowDeltaUpdatedEvent.h>
#include <aws/iotshadow/ShadowDeltaUpdatedSubscriptionRequest.h>
#include <aws/iotshadow/UpdateShadowRequest.h>
#include <aws/iotshadow/UpdateShadowResponse.h>
#include <aws/iotshadow/UpdateShadowSubscriptionRequest.h>

#include <algorithm>
#include <condition_variable>
#include <iostream>
#include <mutex>

using namespace Aws::Crt;
using namespace Aws::Iotshadow;

static const char *SHADOW_VALUE_DEFAULT = "off";

static void s_printHelp()
{
    fprintf(stdout, "Usage:\n");
    fprintf(
        stdout,
        "shadow-sync --endpoint <endpoint> --cert <path to cert>"
        " --key <path to key> --ca_file <optional: path to custom ca>"
        "--thing_name <thing name> --shadow_property <Name of property in shadow to keep in sync.>\n\n");
    fprintf(stdout, "endpoint: the endpoint of the mqtt server not including a port\n");
    fprintf(stdout, "cert: path to your client certificate in PEM format\n");
    fprintf(stdout, "key: path to your key in PEM format\n");
    fprintf(
        stdout,
        "ca_file: Optional, if the mqtt server uses a certificate that's not already"
        " in your trust store, set this.\n");
    fprintf(stdout, "\tIt's the path to a CA file in PEM format\n");
    fprintf(stdout, "thing_name: the name of your IOT thing\n");
    fprintf(stdout, "shadow_property: The name of the shadow property you want to change.\n");
}

static bool s_cmdOptionExists(char **begin, char **end, const String &option)
{
    return std::find(begin, end, option) != end;
}

static char *s_getCmdOption(char **begin, char **end, const String &option)
{
    char **itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

static void s_changeShadowValue(IotShadowClient& client, const String &thingName, const String &shadowProperty, const String &value)
{
    fprintf(stdout, "Changing local shadow value to %s.\n", value.c_str());

    UpdateShadowRequest updateShadowRequest;
    updateShadowRequest.ClientToken = Aws::Crt::UUID();

    JsonObject stateDocument;
    JsonObject reported;
    reported.WithString(shadowProperty, value);
    stateDocument.WithObject("reported", std::move(reported));
    JsonObject desired;
    desired.WithString(shadowProperty, value);
    stateDocument.WithObject("desired", std::move(desired));

    updateShadowRequest.State = std::move(stateDocument);
    updateShadowRequest.ThingName = thingName;

    auto publishCompleted = [thingName, value](int ioErr) {
        if (ioErr != AWS_OP_SUCCESS)
        {
            fprintf(stderr, "failed to update %s shadow state: error %s\n", thingName.c_str(), ErrorDebugString(ioErr));
            return;
        }

        fprintf(stdout, "Successfully updated shadow state for %s, to %s\n", thingName.c_str(), value.c_str());
    };

    client.PublishUpdateShadow(updateShadowRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, std::move(publishCompleted));
}

int main(int argc, char *argv[])
{
    /************************ Setup the Lib ****************************/
    /*
     * These make debug output via ErrorDebugString() work.
     */
    LoadErrorStrings();

    /*
     * Do the global initialization for the API.
     */
    ApiHandle apiHandle;

    String endpoint;
    String certificatePath;
    String keyPath;
    String caFile;
    String thingName;
    String shadowProperty;

    /*********************** Parse Arguments ***************************/
    if (!(s_cmdOptionExists(argv, argv + argc, "--endpoint") && s_cmdOptionExists(argv, argv + argc, "--cert") &&
          s_cmdOptionExists(argv, argv + argc, "--key") && s_cmdOptionExists(argv, argv + argc, "--thing_name") &&
          s_cmdOptionExists(argv, argv + argc, "--shadow_property")))
    {
        s_printHelp();
        return 0;
    }

    endpoint = s_getCmdOption(argv, argv + argc, "--endpoint");
    certificatePath = s_getCmdOption(argv, argv + argc, "--cert");
    keyPath = s_getCmdOption(argv, argv + argc, "--key");
    thingName = s_getCmdOption(argv, argv + argc, "--thing_name");
    shadowProperty = s_getCmdOption(argv, argv + argc, "--shadow_property");

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
    /*
     * We're using Mutual TLS for Mqtt, so we need to load our client certificates
     */
    Io::TlsContextOptions tlsCtxOptions =
        Io::TlsContextOptions::InitClientWithMtls(certificatePath.c_str(), keyPath.c_str());
    /*
     * If we have a custom CA, set that up here.
     */
    if (!caFile.empty())
    {
        tlsCtxOptions.OverrideDefaultTrustStore(nullptr, caFile.c_str());
    }

    uint16_t port = 8883;
    if (Io::TlsContextOptions::IsAlpnSupported())
    {
        /*
         * Use ALPN to negotiate the mqtt protocol on a normal
         * TLS port if possible.
         */
        tlsCtxOptions.SetAlpnList("x-amzn-mqtt-ca");
        port = 443;
    }

    Io::TlsContext tlsCtx(tlsCtxOptions, Io::TlsMode::CLIENT);

    if (!tlsCtx)
    {
        fprintf(stderr, "Tls Context creation failed with error %s\n", ErrorDebugString(tlsCtx.LastError()));
        exit(-1);
    }

    /*
     * Default Socket options to use. IPV4 will be ignored based on what DNS
     * tells us.
     */
    Io::SocketOptions socketOptions;
    socketOptions.connect_timeout_ms = 3000;
    socketOptions.domain = AWS_SOCKET_IPV4;
    socketOptions.type = AWS_SOCKET_STREAM;
    socketOptions.keep_alive_interval_sec = 0;
    socketOptions.keep_alive_timeout_sec = 0;
    socketOptions.keepalive = false;

    Io::ClientBootstrap bootstrap(eventLoopGroup);

    if (!bootstrap)
    {
        fprintf(stderr, "ClientBootstrap failed with error %s\n", ErrorDebugString(bootstrap.LastError()));
        exit(-1);
    }

    /*
     * Now Create a client. This can not throw.
     * An instance of a client must outlive its connections.
     * It is the users responsibility to make sure of this.
     */
    Mqtt::MqttClient mqttClient(bootstrap);

    /*
     * Since no exceptions are used, always check the bool operator
     * when an error could have occured.
     */
    if (!mqttClient)
    {
        fprintf(stderr, "MQTT Client Creation failed with error %s\n", ErrorDebugString(mqttClient.LastError()));
        exit(-1);
    }

    auto connectionOptions = tlsCtx.NewConnectionOptions();
    connectionOptions.server_name = endpoint.c_str();

    /*
     * Now create a connection object. Note: This type is move only
     * and its underlying memory is managed by the client.
     */
    auto connection = mqttClient.NewConnection(endpoint.c_str(), port, socketOptions, connectionOptions);

    if (!*connection)
    {
        fprintf(stderr, "MQTT Connection Creation failed with error %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    /*
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::condition_variable conditionVariable;
    std::atomic<bool> connectionSucceeded(false);
    std::atomic<bool> connectionClosed(false);
    std::atomic<bool> connectionCompleted(false);

    /*
     * This will execute when an mqtt connect has completed or failed.
     */
    auto onConnectionCompleted = [&](Mqtt::MqttConnection &, int errorCode, Mqtt::ReturnCode returnCode, bool) {
        if (errorCode)
        {
            fprintf(stdout, "Connection failed with error %s\n", ErrorDebugString(errorCode));
            connectionSucceeded = false;
        }
        else
        {
            fprintf(stdout, "Connection completed with return code %d\n", returnCode);
            fprintf(stdout, "Connection state %d\n", static_cast<int>(connection->GetConnectionState()));
            connectionSucceeded = true;
        }

        connectionCompleted = true;
        conditionVariable.notify_one();
    };

    /*
     * Invoked when a disconnect message has completed.
     */
    auto onDisconnect = [&](Mqtt::MqttConnection &conn) {
        {
            fprintf(stdout, "Connection state %d\n", static_cast<int>(conn.GetConnectionState()));
            connectionClosed = true;
        }
        conditionVariable.notify_one();
    };

    connection->OnConnectionCompleted = std::move(onConnectionCompleted);
    connection->OnDisconnect = std::move(onDisconnect);

    /*
     * Actually perform the connect dance.
     */
    if (!connection->Connect("client_id12335456", true, 0))
    {
        fprintf(stderr, "MQTT Connection failed with error %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    std::mutex mutex;
    std::unique_lock<std::mutex> uniqueLock(mutex);
    conditionVariable.wait(uniqueLock, [&]() { return connectionSucceeded || connectionClosed; });

    if (connectionSucceeded)
    {
        Aws::Iotshadow::IotShadowClient shadowClient(connection);

        std::atomic<bool> subscribeDeltaCompleted(false);
        std::atomic<bool> subscribeDeltaAccepedCompleted(false);
        std::atomic<bool> subscribeDeltaRejectedCompleted(false);

        auto onDeltaUpdatedSubAck = [&](int ioErr) {
            if (ioErr != AWS_OP_SUCCESS)
            {
                fprintf(stderr, "Error subscribing to shadow delta: %s\n", ErrorDebugString(ioErr));
                exit(-1);
            }

            subscribeDeltaCompleted = true;
            conditionVariable.notify_one();
        };

        auto onDeltaUpdatedAcceptedSubAck = [&](int ioErr) {
            if (ioErr != AWS_OP_SUCCESS)
            {
                fprintf(stderr, "Error subscribing to shadow delta accepted: %s\n", ErrorDebugString(ioErr));
                exit(-1);
            }

            subscribeDeltaAccepedCompleted = true;
            conditionVariable.notify_one();
        };

        auto onDeltaUpdatedRejectedSubAck = [&](int ioErr) {
            if (ioErr != AWS_OP_SUCCESS)
            {
                fprintf(stderr, "Error subscribing to shadow delta rejected: %s\n", ErrorDebugString(ioErr));
            }
            subscribeDeltaRejectedCompleted = true;
            conditionVariable.notify_one();
        };

        auto onDeltaUpdated = [&](ShadowDeltaUpdatedEvent *event, int ioErr) {
            if (event)
            {
                fprintf(stdout, "Received shadow delta event.\n");
                if (event->State && event->State->View().ValueExists(shadowProperty))
                {
                    JsonView objectView = event->State->View().GetJsonObject(shadowProperty);

                    if (objectView.IsNull())
                    {
                        fprintf(
                            stdout,
                            "Delta reports that %s was deleted. Resetting defaults...\n",
                            shadowProperty.c_str());
                        s_changeShadowValue(shadowClient, thingName, shadowProperty, SHADOW_VALUE_DEFAULT);
                    }
                    else
                    {
                        fprintf(
                            stdout,
                            "Delta reports that \"%s\" has a desired value of \"%s\", Changing local value...\n",
                            shadowProperty.c_str(),
                            event->State->View().GetString(shadowProperty).c_str());
                        s_changeShadowValue(
                            shadowClient, thingName, shadowProperty, event->State->View().GetString(shadowProperty));
                    }
                }
                else
                {
                    fprintf(stdout, "Delta did not report a change in \"%s\".\n", shadowProperty.c_str());
                }
            }

            if (ioErr)
            {
                fprintf(stdout, "Error processing shadow delta: %s\n", ErrorDebugString(ioErr));
                exit(-1);
            }
        };

        auto onUpdateShadowAccepted = [&](UpdateShadowResponse *response, int ioErr) {
            if (ioErr == AWS_OP_SUCCESS)
            {
                fprintf(
                    stdout,
                    "Finished updating reported shadow value to %s.\n",
                    response->State->Reported->View().GetString(shadowProperty).c_str());
                fprintf(stdout, "Enter desired value:\n");
            }
            else
            {
                fprintf(stderr, "Error on subscription: %s.\n", ErrorDebugString(ioErr));
                exit(-1);
            }
        };

        auto onUpdateShadowRejected = [&](ErrorResponse *error, int ioErr) {
            if (ioErr == AWS_OP_SUCCESS)
            {
                fprintf(stdout, "Update of shadow state failed with message %s and code %d.", error->Message->c_str(), *error->Code);                 
            }
            else
            {
                fprintf(stderr, "Error on subscription: %s.\n", ErrorDebugString(ioErr));
                exit(-1);
            }
        };

        ShadowDeltaUpdatedSubscriptionRequest shadowDeltaUpdatedRequest;
        shadowDeltaUpdatedRequest.ThingName = thingName;

        shadowClient.SubscribeToShadowDeltaUpdatedEvents(
            shadowDeltaUpdatedRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onDeltaUpdated, onDeltaUpdatedSubAck);

        UpdateShadowSubscriptionRequest updateShadowSubscriptionRequest;
        updateShadowSubscriptionRequest.ThingName = thingName;

        shadowClient.SubscribeToUpdateShadowAccepted(
            updateShadowSubscriptionRequest,
            AWS_MQTT_QOS_AT_LEAST_ONCE,
            onUpdateShadowAccepted,
            onDeltaUpdatedAcceptedSubAck);

        shadowClient.SubscribeToUpdateShadowRejected(
                updateShadowSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, onUpdateShadowRejected, onDeltaUpdatedRejectedSubAck);

        conditionVariable.wait(uniqueLock, [&]() {
            return subscribeDeltaCompleted.load() && subscribeDeltaAccepedCompleted.load() && subscribeDeltaRejectedCompleted.load(); 
        });

        while (true)
        {
            fprintf(stdout, "Enter Desired state of %s:\n", shadowProperty.c_str());
            String input;
            std::cin >> input;

            if (input == "exit" || input == "quit")
            {
                fprintf(stdout, "Exiting...");
                break;
            }

            s_changeShadowValue(shadowClient, thingName, shadowProperty, input);
        }
    }

    if (!connectionClosed)
    {
        /* Disconnect */
        connection->Disconnect();
        conditionVariable.wait(uniqueLock, [&]() { return connectionClosed.load(); });
    }
    return 0;
}
