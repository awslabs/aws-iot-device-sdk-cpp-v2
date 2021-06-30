/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>
#include <aws/crt/StlAllocator.h>
#include <aws/crt/auth/Credentials.h>
#include <aws/crt/io/TlsOptions.h>

#include <aws/iot/MqttClient.h>

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
        "basic-pub-sub --endpoint <endpoint> --cert <path to cert>"
        " --key <path to key> --topic <topic> --ca_file <optional: path to custom ca>"
        " --use_websocket --signing_region <region> --proxy_host <host> --proxy_port <port>"
        " --x509 --x509_role_alias <role_alias> --x509_endpoint <endpoint> --x509_thing <thing_name>"
        " --x509_cert <path to cert> --x509_key <path to key> --x509_rootca <path to root ca>\n\n");
    fprintf(stdout, "endpoint: the endpoint of the mqtt server not including a port\n");
    fprintf(
        stdout,
        "cert: path to your client certificate in PEM format. If this is not set you must specify use_websocket\n");
    fprintf(stdout, "key: path to your key in PEM format. If this is not set you must specify use_websocket\n");
    fprintf(stdout, "topic: topic to publish, subscribe to. (optional)\n");
    fprintf(stdout, "client_id: client id to use (optional)\n");
    fprintf(stdout, "port: the MQTT port to connect to (optional)\n");
    fprintf(stdout, "qos: the MQTT QoS level for published messages, valid values are 0,1,2 (optional)\n");
    fprintf(
        stdout,
        "ca_file: Optional, if the mqtt server uses a certificate that's not already"
        " in your trust store, set this.\n");
    fprintf(stdout, "\tIt's the path to a CA file in PEM format\n");
    fprintf(stdout, "use_websocket: if specified, uses a websocket over https (optional)\n");
    fprintf(
        stdout,
        "signing_region: used for websocket signer it should only be specific if websockets are used. (required for "
        "websockets)\n");
    fprintf(stdout, "proxy_host: host name of the http proxy to use (optional).\n");
    fprintf(stdout, "proxy_port: port of the http proxy to use (optional).\n");

    fprintf(stdout, "  x509: Use the x509 credentials provider while using websockets (optional)\n");
    fprintf(stdout, "  x509_role_alias: Role alias to use with the x509 credentials provider (required for x509)\n");
    fprintf(stdout, "  x509_endpoint: Endpoint to fetch x509 credentials from (required for x509)\n");
    fprintf(stdout, "  x509_thing: Thing name to fetch x509 credentials on behalf of (required for x509)\n");
    fprintf(
        stdout,
        "  x509_cert: Path to the IoT thing certificate used in fetching x509 credentials (required for x509)\n");
    fprintf(
        stdout,
        "  x509_key: Path to the IoT thing private key used in fetching x509 credentials (required for x509)\n");
    fprintf(
        stdout,
        "  x509_rootca: Path to the root certificate used in fetching x509 credentials (required for x509)\n\n");
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
    String topic("test/topic");
    String clientId(String("test-") + Aws::Crt::UUID().ToString());
    String signingRegion;
    String proxyHost;
    uint16_t port(0);
    uint16_t proxyPort(8080);
    aws_mqtt_qos qos = AWS_MQTT_QOS_AT_LEAST_ONCE;

    String x509Endpoint;
    String x509ThingName;
    String x509RoleAlias;
    String x509CertificatePath;
    String x509KeyPath;
    String x509RootCAFile;

    bool useWebSocket = false;
    bool useX509 = false;

    /*********************** Parse Arguments ***************************/
    if (!s_cmdOptionExists(argv, argv + argc, "--endpoint"))
    {
        s_printHelp();
        return 1;
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

    if (keyPath.empty() != certificatePath.empty())
    {
        fprintf(stdout, "Using mtls (cert and key) requires both the certificate and the private key\n");
        s_printHelp();
        return 1;
    }
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
	if (s_cmdOptionExists(argv, argv + argc, "--port"))
    {
        port = static_cast<uint16_t>(atoi(s_getCmdOption(argv, argv + argc, "--port")));
    }
    if (s_cmdOptionExists(argv, argv + argc, "--qos"))
    {
        qos = static_cast<aws_mqtt_qos>(atoi(s_getCmdOption(argv, argv + argc, "--qos")));
    }
    
    if (s_cmdOptionExists(argv, argv + argc, "--use_websocket"))
    {
        if (!s_cmdOptionExists(argv, argv + argc, "--signing_region"))
        {
            fprintf(stdout, "Websockets require a signing region to be specified.\n");
            s_printHelp();
            return 1;
        }
        useWebSocket = true;
        signingRegion = s_getCmdOption(argv, argv + argc, "--signing_region");
    }

    if (s_cmdOptionExists(argv, argv + argc, "--proxy_host"))
    {
        proxyHost = s_getCmdOption(argv, argv + argc, "--proxy_host");
    }

    if (s_cmdOptionExists(argv, argv + argc, "--proxy_port"))
    {
        int port = atoi(s_getCmdOption(argv, argv + argc, "--proxy_port"));
        if (port > 0 && port <= UINT16_MAX)
        {
            proxyPort = static_cast<uint16_t>(port);
        }
    }

    bool usingMtls = !certificatePath.empty() && !keyPath.empty();

    /* one or the other, but not both nor neither */
    if (useWebSocket == usingMtls)
    {
        if (useWebSocket && usingMtls)
        {
            fprintf(stdout, "You must use either websockets or mtls for authentication, but not both.\n");
        }
        else
        {
            fprintf(stdout, "You must use either websockets or mtls for authentication.\n");
        }

        s_printHelp();
        return 1;
    }

    // x509 credentials provider configuration
    if (s_cmdOptionExists(argv, argv + argc, "--x509"))
    {
        if (!useWebSocket)
        {
            fprintf(stdout, "X509 credentials sourcing requires websockets to be enabled and configured.\n");
            s_printHelp();
            return 1;
        }

        if (!s_cmdOptionExists(argv, argv + argc, "--x509_role_alias"))
        {
            fprintf(stdout, "X509 credentials sourcing requires an x509 role alias to be specified.\n");
            s_printHelp();
            return 1;
        }
        x509RoleAlias = s_getCmdOption(argv, argv + argc, "--x509_role_alias");

        if (!s_cmdOptionExists(argv, argv + argc, "--x509_endpoint"))
        {
            fprintf(stdout, "X509 credentials sourcing requires an x509 endpoint to be specified.\n");
            s_printHelp();
            return 1;
        }
        x509Endpoint = s_getCmdOption(argv, argv + argc, "--x509_endpoint");

        if (!s_cmdOptionExists(argv, argv + argc, "--x509_thing"))
        {
            fprintf(stdout, "X509 credentials sourcing requires an x509 thing name to be specified.\n");
            s_printHelp();
            return 1;
        }
        x509ThingName = s_getCmdOption(argv, argv + argc, "--x509_thing");

        if (!s_cmdOptionExists(argv, argv + argc, "--x509_cert"))
        {
            fprintf(stdout, "X509 credentials sourcing requires an Iot thing certificate to be specified.\n");
            s_printHelp();
            return 1;
        }
        x509CertificatePath = s_getCmdOption(argv, argv + argc, "--x509_cert");

        if (!s_cmdOptionExists(argv, argv + argc, "--x509_key"))
        {
            fprintf(stdout, "X509 credentials sourcing requires an Iot thing private key to be specified.\n");
            s_printHelp();
            return 1;
        }
        x509KeyPath = s_getCmdOption(argv, argv + argc, "--x509_key");

        if (s_cmdOptionExists(argv, argv + argc, "--x509_rootca"))
        {
            x509RootCAFile = s_getCmdOption(argv, argv + argc, "--x509_rootca");
        }

        useX509 = true;
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

    Aws::Crt::Http::HttpClientConnectionProxyOptions proxyOptions;
    if (!proxyHost.empty())
    {
        proxyOptions.HostName = proxyHost;
        proxyOptions.Port = proxyPort;
        proxyOptions.AuthType = Aws::Crt::Http::AwsHttpProxyAuthenticationType::None;
    }

    Aws::Crt::Io::TlsContext x509TlsCtx;
    Aws::Iot::MqttClientConnectionConfigBuilder builder;

    if (!certificatePath.empty() && !keyPath.empty())
    {
        builder = Aws::Iot::MqttClientConnectionConfigBuilder(certificatePath.c_str(), keyPath.c_str());
    }
    else if (useWebSocket)
    {
        std::shared_ptr<Aws::Crt::Auth::ICredentialsProvider> provider = nullptr;
        if (useX509)
        {
            Aws::Crt::Io::TlsContextOptions tlsCtxOptions =
                Aws::Crt::Io::TlsContextOptions::InitClientWithMtls(x509CertificatePath.c_str(), x509KeyPath.c_str());
            if (!tlsCtxOptions)
            {
                fprintf(
                    stderr,
                    "Unable to initialize tls context options, error: %s!\n",
                    ErrorDebugString(tlsCtxOptions.LastError()));
                return -1;
            }

            if (!x509RootCAFile.empty())
            {
                tlsCtxOptions.OverrideDefaultTrustStore(nullptr, x509RootCAFile.c_str());
            }

            x509TlsCtx = Aws::Crt::Io::TlsContext(tlsCtxOptions, Aws::Crt::Io::TlsMode::CLIENT);
            if (!x509TlsCtx)
            {
                fprintf(
                    stderr,
                    "Unable to create tls context, error: %s!\n",
                    ErrorDebugString(x509TlsCtx.GetInitializationError()));
                return -1;
            }

            Aws::Crt::Auth::CredentialsProviderX509Config x509Config;

            x509Config.TlsOptions = x509TlsCtx.NewConnectionOptions();
            if (!x509Config.TlsOptions)
            {
                fprintf(
                    stderr,
                    "Unable to create tls options from tls context, error: %s!\n",
                    ErrorDebugString(x509Config.TlsOptions.LastError()));
                return -1;
            }

            x509Config.Bootstrap = &bootstrap;
            x509Config.Endpoint = x509Endpoint;
            x509Config.RoleAlias = x509RoleAlias;
            x509Config.ThingName = x509ThingName;

            if (!proxyHost.empty())
            {
                x509Config.ProxyOptions = proxyOptions;
            }

            provider = Aws::Crt::Auth::CredentialsProvider::CreateCredentialsProviderX509(x509Config);
        }
        else
        {
            Aws::Crt::Auth::CredentialsProviderChainDefaultConfig defaultConfig;
            defaultConfig.Bootstrap = &bootstrap;

            provider = Aws::Crt::Auth::CredentialsProvider::CreateCredentialsProviderChainDefault(defaultConfig);
        }

        if (!provider)
        {
            fprintf(stderr, "Failure to create credentials provider!\n");
            return -1;
        }

        Aws::Iot::WebsocketConfig config(signingRegion, provider);
        builder = Aws::Iot::MqttClientConnectionConfigBuilder(config);
    }
    else
    {
        s_printHelp();
    }

    if (!proxyHost.empty())
    {
        builder.WithHttpProxyOptions(proxyOptions);
    }

    if (!caFile.empty())
    {
        builder.WithCertificateAuthority(caFile.c_str());
    }

	if (port != 0)
	{
		builder.WithPortOverride(port);
	}
	
    builder.WithEndpoint(endpoint);

    auto clientConfig = builder.Build();

    if (!clientConfig)
    {
        fprintf(
            stderr,
            "Client Configuration initialization failed with error %s\n",
            ErrorDebugString(clientConfig.LastError()));
        exit(-1);
    }

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

    if (!connection)
    {
        fprintf(stderr, "MQTT Connection Creation failed with error %s\n", ErrorDebugString(mqttClient.LastError()));
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

    connection->SetOnMessageHandler([](Mqtt::MqttConnection &,
                                       const String &topic,
                                       const ByteBuf &payload,
                                       bool /*dup*/,
                                       Mqtt::QOS /*qos*/,
                                       bool /*retain*/) {
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
        auto onMessage = [&](Mqtt::MqttConnection &,
                             const String &topic,
                             const ByteBuf &byteBuf,
                             bool /*dup*/,
                             Mqtt::QOS /*qos*/,
                             bool /*retain*/) {
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

        connection->Subscribe(topic.c_str(), AWS_MQTT_QOS_AT_LEAST_ONCE, onMessage, onSubAck);
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

            ByteBuf payload = ByteBufFromArray((const uint8_t *)input.data(), input.length());

            auto onPublishComplete = [](Mqtt::MqttConnection &, uint16_t packetId, int errorCode) {
                if (packetId)
                {
                    fprintf(stdout, "Operation on packetId %d Succeeded\n", packetId);
                }
                else
                {
                    fprintf(stdout, "Operation failed with error %s\n", aws_error_debug_str(errorCode));
                }
            };
            connection->Publish(topic.c_str(), qos, false, payload, onPublishComplete);
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
