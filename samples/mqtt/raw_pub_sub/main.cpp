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
        "raw-pub-sub --endpoint <endpoint> --cert <path to cert>"
        " --key <path to key> --topic --ca_file <optional: path to custom ca>"
        " --use_websocket --user_name <username> --password <password> --protocol_name <protocol> "
        " --auth_params=<comma delimited list> --proxy_host <host> --proxy_port <port>\n\n");
    fprintf(stdout, "endpoint: the endpoint of the mqtt server not including a port\n");
    fprintf(stdout, "cert: path to your client certificate in PEM format. Don't use with use_websocket\n");
    fprintf(stdout, "key: path to your key in PEM format. Dont use with use_websocket\n");
    fprintf(stdout, "topic: topic to publish, subscribe to.\n");
    fprintf(stdout, "client_id: client id to use (optional)\n");
    fprintf(
        stdout,
        "ca_file: Optional, if the mqtt server uses a certificate that's not already"
        " in your trust store, set this.\n");
    fprintf(stdout, "\tIt's the path to a CA file in PEM format\n");
    fprintf(stdout, "use_websocket: if specified, uses a websocket over https (optional)\n");
    fprintf(stdout, "proxy_host: if you want to use a proxy with websockets, specify the host here (optional).\n");
    fprintf(
        stdout, "proxy_port: defaults to 8080 is proxy_host is set. Set this to any value you'd like (optional).\n");
    fprintf(stdout, "user_name: User name to send with mqtt connect.\n");
    fprintf(stdout, "password: Password to send with mqtt connect.\n");
    fprintf(stdout, "protocol_name: (optional) defaults to mqtt.\n");
    fprintf(
        stdout,
        "auth_params: (optional) Comma delimited list of auth parameters. For websockets these will be set as headers. "
        "For raw mqtt these will be appended to user_name.\n\n");
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
    String topic;
    String clientId(Aws::Crt::UUID().ToString());
    String proxyHost;
    uint16_t proxyPort(8080);
    String userName;
    String password;
    String protocolName("mqtt");
    Vector<String> authParams;

    bool useWebSocket = false;

    /*********************** Parse Arguments ***************************/
    if (!(s_cmdOptionExists(argv, argv + argc, "--endpoint") && s_cmdOptionExists(argv, argv + argc, "--topic")))
    {
        s_printHelp();
        return 0;
    }

    endpoint = s_getCmdOption(argv, argv + argc, "--endpoint");

    if (s_cmdOptionExists(argv, argv + argc, "--key"))
    {
        keyPath = s_getCmdOption(argv, argv + argc, "--key");
    }

    if (s_cmdOptionExists(argv, argv + argc, "--cert"))
    {
        certificatePath = s_getCmdOption(argv, argv + argc, "--cert");
    }

    topic = s_getCmdOption(argv, argv + argc, "--topic");
    if (s_cmdOptionExists(argv, argv + argc, "--ca_file"))
    {
        caFile = s_getCmdOption(argv, argv + argc, "--ca_file");
    }
    if (s_cmdOptionExists(argv, argv + argc, "--client_id"))
    {
        clientId = s_getCmdOption(argv, argv + argc, "--client_id");
    }
    if (s_cmdOptionExists(argv, argv + argc, "--use_websocket"))
    {
        protocolName = "http/1.1";
        useWebSocket = true;
        if (s_cmdOptionExists(argv, argv + argc, "--proxy_host"))
        {
            proxyHost = s_getCmdOption(argv, argv + argc, "--proxy_host");
        }

        if (s_cmdOptionExists(argv, argv + argc, "--proxy_port"))
        {
            proxyPort = static_cast<uint16_t>(atoi(s_getCmdOption(argv, argv + argc, "--proxy_port")));
        }
    }

    if (s_cmdOptionExists(argv, argv + argc, "--user_name"))
    {
        userName = s_getCmdOption(argv, argv + argc, "--user_name");
    }

    if (s_cmdOptionExists(argv, argv + argc, "--password"))
    {
        password = s_getCmdOption(argv, argv + argc, "--password");
    }

    if (s_cmdOptionExists(argv, argv + argc, "--protocol_name"))
    {
        protocolName = s_getCmdOption(argv, argv + argc, "--protocol_name");
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

    if (!certificatePath.empty() && !keyPath.empty())
    {
        ctxOptions = Io::TlsContextOptions::InitClientWithMtls(certificatePath.c_str(), keyPath.c_str());
    }

    if (!caFile.empty())
    {
        ctxOptions.OverrideDefaultTrustStore(nullptr, caFile.c_str());
    }

    uint16_t port = useWebSocket ? 443 : 8883;
    if (Io::TlsContextOptions::IsAlpnSupported())
    {
        port = 443;
        ctxOptions.SetAlpnList(protocolName.c_str());
    }

    Io::SocketOptions socketOptions;
    socketOptions.SetConnectTimeoutMs(3000);

    Io::TlsContext tlsContext(ctxOptions, Io::TlsMode::CLIENT);

    auto connection = client.NewConnection(endpoint.c_str(), port, socketOptions, tlsContext, useWebSocket);

    if (!connection)
    {
        fprintf(stderr, "MQTT Connection Creation failed with error %s\n", ErrorDebugString(client.LastError()));
        exit(-1);
    }

    if (!authParams.empty())
    {
        if (useWebSocket)
        {
            /* set authorizer headers on the outgoing websocket upgrade request. */
            connection->WebsocketInterceptor = [&](std::shared_ptr<Http::HttpRequest> req,
                                                   const Mqtt::OnWebSocketHandshakeInterceptComplete &onComplete) {
                for (auto &param : authParams)
                {
                    Http::HttpHeader header;
                    AWS_ZERO_STRUCT(header);
                    ByteCursor paramPair = ByteCursorFromCString(param.c_str());
                    ByteCursor nameOrValue;
                    AWS_ZERO_STRUCT(nameOrValue);
                    if (!aws_byte_cursor_next_split(&paramPair, '=', &nameOrValue))
                    {
                        s_printHelp();
                        exit(-1);
                    }
                    header.name = nameOrValue;
                    if (!aws_byte_cursor_next_split(&paramPair, '=', &nameOrValue))
                    {
                        s_printHelp();
                        exit(-1);
                    }
                    header.value = nameOrValue;
                    req->AddHeader(header);
                }
                onComplete(req, AWS_OP_SUCCESS);
            };
        } /* set authorizer parameters on username if we're using raw mqtt */
        else if (!userName.empty())
        {
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
    }

    if (!userName.empty())
    {
        connection->SetLogin(userName.c_str(), password.c_str());
    }

    if (!proxyHost.empty())
    {
        Http::HttpClientConnectionProxyOptions proxyOptions;
        proxyOptions.HostName = proxyHost;
        proxyOptions.Port = proxyPort;
        connection->SetWebsocketProxyOptions(proxyOptions);
    }

    /*
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::mutex mutex;
    std::condition_variable conditionVariable;
    bool connectionSucceeded = false;
    bool connectionClosed = false;
    bool connectionCompleted = false;
    bool publishCompleted = false;
    bool subscribeAck = false;

    /*
     * This will execute when an mqtt connect has completed or failed.
     */
    auto onConnectionCompleted = [&](Mqtt::MqttConnection &, int errorCode, Mqtt::ReturnCode returnCode, bool) {
        if (errorCode)
        {
            fprintf(stdout, "Connection failed with error %s\n", ErrorDebugString(errorCode));
            std::lock_guard<std::mutex> lockGuard(mutex);
            connectionSucceeded = false;
        }
        else
        {
            fprintf(stdout, "Connection completed with return code %d\n", returnCode);
            connectionSucceeded = true;
        }
        {
            std::lock_guard<std::mutex> lockGuard(mutex);
            connectionCompleted = true;
        }
        conditionVariable.notify_one();
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
            std::lock_guard<std::mutex> lockGuard(mutex);
            connectionClosed = true;
        }
        conditionVariable.notify_one();
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

    std::unique_lock<std::mutex> uniqueLock(mutex);
    conditionVariable.wait(uniqueLock, [&]() { return connectionCompleted; });

    if (connectionSucceeded)
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
        auto onSubAck = [&](Mqtt::MqttConnection &, uint16_t packetId, const String &topic, Mqtt::QOS, int errorCode) {
            if (packetId)
            {
                fprintf(stdout, "Subscribe on topic %s on packetId %d Succeeded\n", topic.c_str(), packetId);
            }
            else
            {
                fprintf(stdout, "Subscribe failed with error %s\n", aws_error_debug_str(errorCode));
            }
            subscribeAck = true;
            std::lock_guard<std::mutex> lockGuard(mutex);
            conditionVariable.notify_one();
        };

        connection->Subscribe(topic.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, onPublish, onSubAck);
        conditionVariable.wait(uniqueLock, [&]() { return subscribeAck; });

        while (true)
        {
            publishCompleted = false;
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

            auto onPublishComplete = [&, payloadPtr](Mqtt::MqttConnection &, uint16_t packetId, int errorCode) {
                aws_byte_buf_clean_up(payloadPtr);

                if (packetId)
                {
                    fprintf(stdout, "Operation on packetId %d Succeeded\n", packetId);
                }
                else
                {
                    fprintf(stdout, "Operation failed with error %s\n", aws_error_debug_str(errorCode));
                }
                std::lock_guard<std::mutex> lockGuard(mutex);
                publishCompleted = true;
                conditionVariable.notify_one();
            };
            connection->Publish(topic.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, false, payload, onPublishComplete);
            conditionVariable.wait(uniqueLock, [&]() { return publishCompleted; });
        }

        /*
         * Unsubscribe from the topic.
         */
        connection->Unsubscribe(
            topic.c_str(), [&](Mqtt::MqttConnection &, uint16_t, int) { conditionVariable.notify_one(); });
        conditionVariable.wait(uniqueLock);
    }

    /* Disconnect */
    if (connection->Disconnect())
    {
        conditionVariable.wait(uniqueLock, [&]() { return connectionClosed; });
    }
    return 0;
}
