/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/StlAllocator.h>

#include <aws/crt/http/HttpRequestResponse.h>
#include <aws/crt/mqtt/MqttClient.h>

#include <algorithm>
#include <aws/crt/UUID.h>
#include <condition_variable>
#include <iostream>
#include <mutex>

using namespace Aws::Crt;

static void s_printHelp()
{
    fprintf(stdout, "Usage:\n");
    fprintf(
        stdout,
        "mqtt_custom_auth --endpoint <endpoint>"
        " --topic <topic> --ca_file <optional: path to custom ca>"
        " --user_name <username> --password <password>\n\n");
    fprintf(stdout, "endpoint: the endpoint of the mqtt server not including a port\n");
    fprintf(stdout, "topic: topic to publish, subscribe to. (optional)\n");
    fprintf(stdout, "client_id: client id to use (optional)\n");
    fprintf(
        stdout,
        "ca_file: Optional, if the mqtt server uses a certificate that's not already"
        " in your trust store, set this.\n");
    fprintf(stdout, "\tIt's the path to a CA file in PEM format\n");
    fprintf(stdout, "user_name: User name to send with mqtt connect.\n");
    fprintf(stdout, "password: Password to send with mqtt connect.\n");
    fprintf(
        stdout,
        "auth_params: (optional) Comma delimited list of auth parameters that will be appended to user_name.\n\n");
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
    String caFile;
    String topic("test/topic");
    String clientId(String("test-") + Aws::Crt::UUID().ToString());
    String userName;
    String password;
    // Valid protocol names are documented on page:
    // https://docs.aws.amazon.com/iot/latest/developerguide/protocols.html
    // Use "mqtt" for Custom Authentication
    String protocolName("mqtt"); // X.509 client certificate auth
    Vector<String> authParams;

    bool useWebSocket = false;

    /*********************** Parse Arguments ***************************/
    if (!s_cmdOptionExists(argv, argv + argc, "--endpoint"))
    {
        s_printHelp();
        return 0;
    }

    endpoint = s_getCmdOption(argv, argv + argc, "--endpoint");

    if (s_getCmdOption(argv, argv + argc, "--topic"))
    {
        topic = s_getCmdOption(argv, argv + argc, "--topic");
    }
    if (s_cmdOptionExists(argv, argv + argc, "--ca_file"))
    {
        caFile = s_getCmdOption(argv, argv + argc, "--ca_file");
    }
    if (s_cmdOptionExists(argv, argv + argc, "--client_id"))
    {
        clientId = s_getCmdOption(argv, argv + argc, "--client_id");
    }

    if (s_cmdOptionExists(argv, argv + argc, "--user_name"))
    {
        userName = s_getCmdOption(argv, argv + argc, "--user_name");
    }

    if (s_cmdOptionExists(argv, argv + argc, "--password"))
    {
        password = s_getCmdOption(argv, argv + argc, "--password");
    }

    if (s_cmdOptionExists(argv, argv + argc, "--auth_params"))
    {
        String params = s_getCmdOption(argv, argv + argc, "--auth_params");
        ByteCursor commaDelimitedParams = ByteCursorFromCString(params.c_str());
        ByteCursor split;
        AWS_ZERO_STRUCT(split);
        while (aws_byte_cursor_next_split(&commaDelimitedParams, ',', &split))
        {
            authParams.push_back(String((char *)split.ptr, split.len));
        }
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

    Aws::Crt::Io::DefaultHostResolver defaultHostResolver(eventLoopGroup, 1, 5);
    Io::ClientBootstrap bootstrap(eventLoopGroup, defaultHostResolver);

    if (!bootstrap)
    {
        fprintf(stderr, "ClientBootstrap failed with error %s\n", ErrorDebugString(bootstrap.LastError()));
        exit(-1);
    }

    Mqtt::MqttClient client(bootstrap);
    Io::TlsContextOptions ctxOptions = Io::TlsContextOptions::InitDefaultClient();


    if (!caFile.empty())
    {
        ctxOptions.OverrideDefaultTrustStore(nullptr, caFile.c_str());
    }

    uint16_t port = 443;
    if (!Io::TlsContextOptions::IsAlpnSupported())
    {
        fprintf(stderr, "ALPN support is needed to use MQTT username and password auth");
        exit(-1);
    }
    ctxOptions.SetAlpnList("mqtt");
    Io::SocketOptions socketOptions;
    socketOptions.SetConnectTimeoutMs(3000);

    Io::TlsContext tlsContext(ctxOptions, Io::TlsMode::CLIENT);

    auto connection = client.NewConnection(endpoint.c_str(), port, socketOptions, tlsContext, useWebSocket);

    if (!connection)
    {
        fprintf(stderr, "MQTT Connection Creation failed with error %s\n", ErrorDebugString(client.LastError()));
        exit(-1);
    }

    

    if (!userName.empty())
    {
        if (!authParams.empty())
        {
            /* set authorizer parameters on username if we're using raw mqtt */
            userName += "?";
            size_t count = 0;
            for (auto &param : authParams)
            {
                userName += param;
                count++;
                if (count < authParams.size())
                {
                    userName += "&";
                }
            }
        }
        connection->SetLogin(userName.c_str(), password.c_str());
    } 
    else 
    {
        fprintf(stderr, "Username must be specified");
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
            if (returnCode != AWS_MQTT_CONNECT_ACCEPTED)
            {
                fprintf(stdout, "Connection failed with mqtt return code %d\n", (int)returnCode);
                connectionCompletedPromise.set_value(false);
            }
            else
            {
                fprintf(stdout, "Connection completed successfully.");
                connectionCompletedPromise.set_value(true);
            }
        }
    };

    auto onInterrupted = [&](Mqtt::MqttConnection &, int error) {
        fprintf(stdout, "Connection interrupted with error %s\n", ErrorDebugString(error));
    };

    auto onResumed = [&](Mqtt::MqttConnection &, Mqtt::ReturnCode, bool) { fprintf(stdout, "Connection resumed\n"); };

    /*
     * Invoked when a disconnect message has completed.
     */
    auto onDisconnect = [&](Mqtt::MqttConnection &) {
        {
            fprintf(stdout, "Disconnect completed\n");
            connectionClosedPromise.set_value();
        }
    };

    connection->OnConnectionCompleted = std::move(onConnectionCompleted);
    connection->OnDisconnect = std::move(onDisconnect);
    connection->OnConnectionInterrupted = std::move(onInterrupted);
    connection->OnConnectionResumed = std::move(onResumed);

    connection->SetOnMessageHandler([](Mqtt::MqttConnection &, const String &topic, const ByteBuf &payload) {
        fprintf(stdout, "Generic Publish received on topic %s, payload:\n", topic.c_str());
        fwrite(payload.buffer, 1, payload.len, stdout);
        fprintf(stdout, "\n");
    });

    /*
     * Actually perform the connect dance.
     * This will use default ping behavior of 1 hour and 3 second timeouts.
     * If you want different behavior, those arguments go into slots 3 & 4.
     */
    fprintf(stdout, "Connecting...\n");
    if (!connection->Connect(clientId.c_str(), false, 1000))
    {
        fprintf(stderr, "MQTT Connection failed with error %s\n", ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    if (connectionCompletedPromise.get_future().get())
    {
        /*
         * This is invoked upon the receipt of a Publish on a subscribed topic.
         */
        auto onPublish = [&](Mqtt::MqttConnection &, const String &topic, const ByteBuf &byteBuf) {
            fprintf(stdout, "Publish received on topic %s\n", topic.c_str());
            fprintf(stdout, "\n Message:\n");
            fwrite(byteBuf.buffer, 1, byteBuf.len, stdout);
            fprintf(stdout, "\n");
        };

        /*
         * Subscribe for incoming publish messages on topic.
         */
        std::promise<void> subscribeFinishedPromise;
        auto onSubAck =
            [&](Mqtt::MqttConnection &, uint16_t packetId, const String &topic, Mqtt::QOS QoS, int errorCode) {
                if (errorCode)
                {
                    fprintf(stderr, "Subscribe failed with error %s\n", aws_error_debug_str(errorCode));
                    exit(-1);
                }
                else
                {
                    if (!packetId || QoS == AWS_MQTT_QOS_FAILURE)
                    {
                        fprintf(stderr, "Subscribe rejected by the broker.");
                        exit(-1);
                    }
                    else
                    {
                        fprintf(stdout, "Subscribe on topic %s on packetId %d Succeeded\n", topic.c_str(), packetId);
                    }
                }
                subscribeFinishedPromise.set_value();
            };

        connection->Subscribe(topic.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, onPublish, onSubAck);
        subscribeFinishedPromise.get_future().wait();

        while (true)
        {
            String input;
            fprintf(
                stdout,
                "Enter the message you want to publish to topic %s and press enter. Enter 'exit' to exit this "
                "program.\n",
                topic.c_str());
            std::getline(std::cin, input);

            if (input == "exit")
            {
                break;
            }

            ByteBuf payload = ByteBufNewCopy(DefaultAllocator(), (const uint8_t *)input.data(), input.length());
            ByteBuf *payloadPtr = &payload;

            auto onPublishComplete = [payloadPtr](Mqtt::MqttConnection &, uint16_t packetId, int errorCode) {
                aws_byte_buf_clean_up(payloadPtr);

                if (packetId)
                {
                    fprintf(stdout, "Operation on packetId %d Succeeded\n", packetId);
                }
                else
                {
                    fprintf(stdout, "Operation failed with error %s\n", aws_error_debug_str(errorCode));
                }
            };
            connection->Publish(topic.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, false, payload, onPublishComplete);
        }

        /*
         * Unsubscribe from the topic.
         */
        std::promise<void> unsubscribeFinishedPromise;
        connection->Unsubscribe(
            topic.c_str(), [&](Mqtt::MqttConnection &, uint16_t, int) { unsubscribeFinishedPromise.set_value(); });
        unsubscribeFinishedPromise.get_future().wait();
    }

    /* Disconnect */
    if (connection->Disconnect())
    {
        connectionClosedPromise.get_future().wait();
    }
    return 0;
}
