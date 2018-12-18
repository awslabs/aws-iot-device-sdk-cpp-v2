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

#include <aws/iotsdk/jobs/IotJobsClient.h>

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <algorithm>

using namespace Aws::Crt;
using namespace Aws::IotSdk::Jobs;

static void s_printHelp()
{
    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "describe-job-execution --endpoint <endpoint> --cert <path to cert>"
                    " --key <path to key> --ca_file <optional: path to custom ca>"
                    "--thing_name <thing name> --job_id <job id>\n\n");
    fprintf(stdout, "endpoint: the endpoint of the mqtt server not including a port\n");
    fprintf(stdout, "cert: path to your client certificate in PEM format\n");
    fprintf(stdout, "key: path to your key in PEM format\n");
    fprintf(stdout, "ca_file: Optional, if the mqtt server uses a certificate that's not already"
                    " in your trust store, set this.\n");
    fprintf(stdout, "\tIt's the path to a CA file in PEM format\n");
    fprintf(stdout, "thing_name: the name of your IOT thing\n");
    fprintf(stdout, "job_id: the job id you want to describe.\n");
}

bool s_cmdOptionExists(char** begin, char** end, const String& option)
{
    return std::find(begin, end, option) != end;
}

char* s_getCmdOption(char ** begin, char ** end, const String & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

int main(int argc, char* argv[])
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
    String jobId;

    /*********************** Parse Arguments ***************************/
    if (!(s_cmdOptionExists(argv, argv + argc, "--endpoint") &&
          s_cmdOptionExists(argv, argv + argc, "--cert") &&
          s_cmdOptionExists(argv, argv + argc, "--key") &&
          s_cmdOptionExists(argv, argv + argc, "--thing_name") &&
          s_cmdOptionExists(argv, argv + argc, "--job_id")))
    {
        s_printHelp();
        return 0;
    }

    endpoint = s_getCmdOption(argv, argv + argc, "--endpoint");
    certificatePath = s_getCmdOption(argv, argv + argc, "--cert");
    keyPath = s_getCmdOption(argv, argv + argc, "--key");
    thingName = s_getCmdOption(argv, argv + argc, "--thing_name");
    jobId = s_getCmdOption(argv, argv + argc, "--job_id");

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
        fprintf(stderr, "Event Loop Group Creation failed with error %s\n",
                ErrorDebugString(eventLoopGroup.LastError()));
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
        fprintf(stderr, "Tls Context creation failed with error %s\n",
                ErrorDebugString(tlsCtx.LastError()));
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
        fprintf(stderr, "ClientBootstrap failed with error %s\n",
                ErrorDebugString(bootstrap.LastError()));
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
        fprintf(stderr, "MQTT Client Creation failed with error %s\n",
                ErrorDebugString(mqttClient.LastError()));
        exit(-1);
    }

    auto connectionOptions = tlsCtx.NewConnectionOptions();
    /*
     * Now create a connection object. Note: This type is move only
     * and its underlying memory is managed by the client.
     */
    auto connection =
            mqttClient.NewConnection(endpoint.c_str(), port, socketOptions, connectionOptions);

    if (!*connection)
    {
        fprintf(stderr, "MQTT Connection Creation failed with error %s\n",
                ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    /*
     * In a real world application you probably don't want to enforce synchronous behavior
     * but this is a sample console application, so we'll just do that with a condition variable.
     */
    std::mutex mutex;
    std::condition_variable conditionVariable;
    bool connectionSucceeded = false;
    bool connectionClosed = false;

    /*
     * This will execute when an mqtt connect has completed or failed.
     */
    auto onConAck = [&](Mqtt::MqttConnection&,
                        Mqtt::ReturnCode returnCode, bool)
    {
        {
            fprintf(stdout, "Connection completed with return code %d\n", returnCode);
            fprintf(stdout, "Connection state %d\n", static_cast<int>(connection->GetConnectionState()));
            std::lock_guard<std::mutex> lockGuard(mutex);
            connectionSucceeded = true;
        }
        conditionVariable.notify_one();
    };

    /*
     * This will be invoked when the TCP connection fails.
     */
    auto onConFailure = [&](Mqtt::MqttConnection&, int error)
    {
        {
            fprintf(stdout, "Connection failed with %s\n", ErrorDebugString(error));
            std::lock_guard<std::mutex> lockGuard(mutex);
            connectionClosed = true;
        }
        conditionVariable.notify_one();
    };

    /*
     * Invoked when a disconnect message has completed.
     */
    auto onDisconnect = [&](Mqtt::MqttConnection& conn, int error) -> bool
    {
        {
            fprintf(stdout, "Connection closed with error %s\n", ErrorDebugString(error));
            fprintf(stdout, "Connection state %d\n", static_cast<int>(conn.GetConnectionState()));
            std::lock_guard<std::mutex> lockGuard(mutex);
            connectionClosed = true;
        }
        conditionVariable.notify_one();
        return false;
    };

    connection->OnConnAck = std::move(onConAck);
    connection->OnConnectionFailed = std::move(onConFailure);
    connection->OnDisconnect = std::move(onDisconnect);

    /*
     * Actually perform the connect dance.
     */
    if (!connection->Connect("client_id12335456", true, 0))
    {
        fprintf(stderr, "MQTT Connection failed with error %s\n",
                ErrorDebugString(connection->LastError()));
        exit(-1);
    }

    std::unique_lock<std::mutex> uniqueLock(mutex);
    conditionVariable.wait(uniqueLock, [&]() {return connectionSucceeded || connectionClosed; });

    if (connectionSucceeded)
    {
        IotJobsClient client(connection);

        DescribeJobExecutionSubscriptionRequest describeJobExecutionSubscriptionRequest(thingName, jobId);

        // This isn't absolutely neccessary but since we're doing a publish almost immediately afterwards,
        // to be cautious make sure the subscribe has finished before doing the publish.
        auto subAckHandler = [&](int ioErr)
        {
            if (!ioErr)
            {
                conditionVariable.notify_one();
            }
        };

        auto subscriptionHandler = [&](DescribeJobExecutionResponse* response, int ioErr)
        {
            if (ioErr)
            {
                fprintf(stderr, "Error %d occurred\n", ioErr);
                conditionVariable.notify_one();
                return;
            }

            fprintf(stdout, "Received Job:\n");
            fprintf(stdout, "Job Id: %s\n", response->Execution->JobId->c_str());
            fprintf(stdout, "ClientToken: %s\n", response->ClientToken->ToString().c_str());
            fprintf(stdout, "Execution Status: %s\n", JobStatusMarshaller::ToString(*response->Execution->Status));
            conditionVariable.notify_one();
        };

        client.SubscribeToDescribeJobExecutionAccepted(describeJobExecutionSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, subscriptionHandler, subAckHandler);
        conditionVariable.wait(uniqueLock);

        auto failureHandler = [&](JobsError* error, int ioErr)
        {
            if (ioErr)
            {
                fprintf(stderr, "Error %d occurred\n", ioErr);
                conditionVariable.notify_one();
                return;
            }

            if (error)
            {
                fprintf(stderr, "Service Error %d occured\n", (int)error->ErrorCode);
                conditionVariable.notify_one();
                return;
            }
        };

        client.SubscribeToDescribeJobExecutionRejected(describeJobExecutionSubscriptionRequest, AWS_MQTT_QOS_AT_LEAST_ONCE, failureHandler, subAckHandler);
        conditionVariable.wait(uniqueLock);

        DescribeJobExecutionRequest describeJobExecutionRequest(thingName, jobId);
        describeJobExecutionRequest.IncludeJobDocument = true;

        auto publishHandler = [&](int ioErr)
        {
            if (ioErr)
            {
                fprintf(stderr, "Error %d occurred\n", ioErr);
                conditionVariable.notify_one();
                return;
            }
        };

        client.PublishDescribeJobExecution(std::move(describeJobExecutionRequest), AWS_MQTT_QOS_AT_LEAST_ONCE, publishHandler);
        conditionVariable.wait(uniqueLock);
    }

    if (!connectionClosed) {
        /* Disconnect */
        connection->Disconnect();
        conditionVariable.wait(uniqueLock, [&]() { return connectionClosed; });
    }
    return 0;
}
