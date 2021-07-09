/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/crt/Api.h>

#include <awstest/EchoTestRpcClient.h>

#include <aws/testing/aws_test_harness.h>

#include <iostream>
#include <queue>
#include <sstream>

using namespace Aws::Crt;
using namespace Aws::Eventstreamrpc;
using namespace Awstest;

struct EventStreamClientTestContext
{
    std::unique_ptr<ApiHandle> apiHandle;
    std::unique_ptr<Io::EventLoopGroup> elGroup;
    std::unique_ptr<Io::HostResolver> resolver;
    std::unique_ptr<Io::ClientBootstrap> clientBootstrap;
};

static EventStreamClientTestContext s_testContext;

static int s_testSetup(struct aws_allocator *allocator, void *ctx)
{
    auto *testContext = static_cast<EventStreamClientTestContext *>(ctx);

    testContext->apiHandle = std::unique_ptr<ApiHandle>(new ApiHandle(allocator));
    testContext->elGroup = std::unique_ptr<Io::EventLoopGroup>(new Io::EventLoopGroup(0, allocator));
    testContext->resolver =
        std::unique_ptr<Io::HostResolver>(new Io::DefaultHostResolver(*testContext->elGroup, 8, 30, allocator));
    testContext->clientBootstrap = std::unique_ptr<Io::ClientBootstrap>(
        new Io::ClientBootstrap(*testContext->elGroup, *testContext->resolver, allocator));

    return AWS_ERROR_SUCCESS;
}

static int s_testTeardown(struct aws_allocator *allocator, int setup_result, void *ctx)
{
    auto *testContext = static_cast<EventStreamClientTestContext *>(ctx);

    testContext->elGroup.reset();
    testContext->resolver.reset();
    testContext->clientBootstrap.reset();
    /* The ApiHandle must be deallocated last or a deadlock occurs. */
    testContext->apiHandle.reset();

    return AWS_ERROR_SUCCESS;
}

static int s_TestEventStreamConnect(struct aws_allocator *allocator, void *ctx);
static int s_TestEchoOperation(struct aws_allocator *allocator, void *ctx);
static int s_TestOperationWhileDisconnected(struct aws_allocator *allocator, void *ctx);

class TestLifecycleHandler : public ConnectionLifecycleHandler
{
  public:
    TestLifecycleHandler()
    {
        semaphoreULock = std::unique_lock<std::mutex>(semaphoreLock);
        isConnected = false;
        lastErrorCode = AWS_OP_ERR;
    }

    void OnConnectCallback() override
    {
        std::lock_guard<std::mutex> lockGuard(semaphoreLock);

        isConnected = true;

        semaphore.notify_one();
    }

    void OnDisconnectCallback(RpcError error) override
    {
        std::lock_guard<std::mutex> lockGuard(semaphoreLock);

        lastErrorCode = error.baseStatus;

        semaphore.notify_one();
    }

    void WaitOnCondition(std::function<bool(void)> condition) { semaphore.wait(semaphoreULock, condition); }

  private:
    friend int s_TestEventStreamConnect(struct aws_allocator *allocator, void *ctx);
    std::condition_variable semaphore;
    std::mutex semaphoreLock;
    std::unique_lock<std::mutex> semaphoreULock;
    int isConnected;
    int lastErrorCode;
};

static void s_onMessageFlush(int errorCode)
{
    (void)errorCode;
}

AWS_TEST_CASE_FIXTURE(EventStreamConnect, s_testSetup, s_TestEventStreamConnect, s_testTeardown, &s_testContext);
static int s_TestEventStreamConnect(struct aws_allocator *allocator, void *ctx)
{
    auto *testContext = static_cast<EventStreamClientTestContext *>(ctx);

    {
        MessageAmendment connectionAmendment;
        auto messageAmender = [&](void) -> const MessageAmendment & { return connectionAmendment; };

        ConnectionConfig accessDeniedConfig;
        accessDeniedConfig.SetHostName(Aws::Crt::String("127.0.0.1"));
        accessDeniedConfig.SetPort(8033U);

        /* Happy path case. */
        {
            ConnectionLifecycleHandler lifecycleHandler;
            Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);
            auto connectedStatus = client.Connect(lifecycleHandler);
            ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);
            client.Close();
        }

        /* Empty amendment headers. */
        {
            TestLifecycleHandler lifecycleHandler;
            ClientConnection connection(allocator);
            auto future = connection.Connect(accessDeniedConfig, &lifecycleHandler, *testContext->clientBootstrap);
            ASSERT_TRUE(future.get().baseStatus == EVENT_STREAM_RPC_CONNECTION_ACCESS_DENIED);
        }

        /* Rejected client-name header. */
        {
            TestLifecycleHandler lifecycleHandler;
            ClientConnection connection(allocator);
            connectionAmendment.AddHeader(EventStreamHeader(
                Aws::Crt::String("client-name"), Aws::Crt::String("rejected.testy_mc_testerson"), allocator));
            auto future = connection.Connect(accessDeniedConfig, &lifecycleHandler, *testContext->clientBootstrap);
            ASSERT_TRUE(future.get().baseStatus == EVENT_STREAM_RPC_CONNECTION_ACCESS_DENIED);
        }

        /* Connect without taking its future then immediately close. */
        {
            ConnectionLifecycleHandler lifecycleHandler;
            Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);
            auto connectedStatus = client.Connect(lifecycleHandler);
            client.Close();
            client.Close();
            ASSERT_FALSE(client.IsConnected());
        }
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE_FIXTURE(
    OperateWhileDisconnected,
    s_testSetup,
    s_TestOperationWhileDisconnected,
    s_testTeardown,
    &s_testContext);
static int s_TestOperationWhileDisconnected(struct aws_allocator *allocator, void *ctx)
{
    auto *testContext = static_cast<EventStreamClientTestContext *>(ctx);

    /* Don't connect at all and try running operations as normal. */
    {
        ConnectionLifecycleHandler lifecycleHandler;
        Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);
        auto echoMessage = client.NewEchoMessage();
        EchoMessageRequest echoMessageRequest;
        MessageData messageData;
        Aws::Crt::String expectedMessage("l33t");
        messageData.SetStringMessage(expectedMessage);
        echoMessageRequest.SetMessage(messageData);
        auto requestFuture = echoMessage.Activate(echoMessageRequest, s_onMessageFlush);
        ASSERT_TRUE(requestFuture.get().baseStatus == EVENT_STREAM_RPC_CONNECTION_CLOSED);
        auto result = echoMessage.GetOperationResult().get();
        ASSERT_FALSE(result);
        auto error = result.GetRpcError();
        ASSERT_TRUE(error.baseStatus == EVENT_STREAM_RPC_CONNECTION_CLOSED);
    }

    /* Idempotent close and its safety. */
    {
        ConnectionLifecycleHandler lifecycleHandler;
        Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);
        client.Close();
        client.Close();
    }

    return AWS_OP_SUCCESS;
}

AWS_TEST_CASE_FIXTURE(EchoOperation, s_testSetup, s_TestEchoOperation, s_testTeardown, &s_testContext);
static int s_TestEchoOperation(struct aws_allocator *allocator, void *ctx)
{
    auto *testContext = static_cast<EventStreamClientTestContext *>(ctx);
    ConnectionLifecycleHandler lifecycleHandler;
    Aws::Crt::String expectedMessage("Async I0 FTW");
    EchoMessageRequest echoMessageRequest;
    MessageData messageData;
    messageData.SetStringMessage(expectedMessage);

    /* Perform a regular echo operation. */
    {
        Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);
        auto connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);
        auto echoMessage = client.NewEchoMessage();
        messageData.SetStringMessage(expectedMessage);
        echoMessageRequest.SetMessage(messageData);
        auto requestFuture = echoMessage.Activate(echoMessageRequest, s_onMessageFlush);
        requestFuture.wait();
        auto result = echoMessage.GetResult().get();
        ASSERT_TRUE(result);
        auto response = result.GetOperationResponse();
        ASSERT_NOT_NULL(response);
        ASSERT_TRUE(response->GetMessage().value().GetStringMessage().value() == expectedMessage);
    }

    /* Attempt a connection, close it, then try running operations as normal. */
    {
        ConnectionLifecycleHandler lifecycleHandler;
        Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);
        auto connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);
        client.Close();
        auto echoMessage = client.NewEchoMessage();
        EchoMessageRequest echoMessageRequest;
        MessageData messageData;
        Aws::Crt::String expectedMessage("l33t");
        messageData.SetStringMessage(expectedMessage);
        echoMessageRequest.SetMessage(messageData);
        auto requestFuture = echoMessage.Activate(echoMessageRequest, s_onMessageFlush);
        ASSERT_TRUE(requestFuture.get().baseStatus == EVENT_STREAM_RPC_CONNECTION_CLOSED);
        auto result = echoMessage.GetOperationResult().get();
        ASSERT_FALSE(result);
        auto error = result.GetRpcError();
        ASSERT_TRUE(error.baseStatus == EVENT_STREAM_RPC_CONNECTION_CLOSED);
    }

    /* Perform a regular echo operation but one after another without waiting.
     * Only the response from the first operation will be received. */
    {
        Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);
        auto connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);
        auto echoMessage = client.NewEchoMessage();
        messageData.SetStringMessage(expectedMessage);
        echoMessageRequest.SetMessage(messageData);
        auto requestFuture = echoMessage.Activate(echoMessageRequest, s_onMessageFlush);
        requestFuture = echoMessage.Activate(echoMessageRequest, s_onMessageFlush);
        MessageData differentMessage;
        differentMessage.SetBooleanMessage(true);
        echoMessageRequest.SetMessage(differentMessage);
        requestFuture = echoMessage.Activate(echoMessageRequest, s_onMessageFlush);
        requestFuture.wait();
        auto result = echoMessage.GetResult().get();
        ASSERT_TRUE(result);
        auto response = result.GetOperationResponse();
        ASSERT_NOT_NULL(response);
        ASSERT_TRUE(response->GetMessage().value().GetStringMessage().value() == expectedMessage);
    }

    /* Closing the stream should be idempotent. */
    {
        Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);

        auto connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);
        auto echoMessage = client.NewEchoMessage();
        messageData.SetStringMessage(expectedMessage);
        echoMessageRequest.SetMessage(messageData);
        auto requestFuture = echoMessage.Activate(echoMessageRequest, s_onMessageFlush);
        requestFuture = echoMessage.Activate(echoMessageRequest, s_onMessageFlush);
        MessageData differentMessage;
        differentMessage.SetBooleanMessage(true);
        echoMessageRequest.SetMessage(differentMessage);
        requestFuture = echoMessage.Activate(echoMessageRequest, s_onMessageFlush);
        requestFuture.wait();
        echoMessage.Close().wait();
        echoMessage.Close().wait();
        echoMessage.Close().wait();
        echoMessage.Close().wait();
    }

    /* Close without waiting on activation or close futures. */
    {
        Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);

        auto connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);
        auto echoMessage = client.NewEchoMessage();
        messageData.SetStringMessage(expectedMessage);
        echoMessageRequest.SetMessage(messageData);
        auto requestFuture = echoMessage.Activate(echoMessageRequest, s_onMessageFlush);
        requestFuture = echoMessage.Activate(echoMessageRequest, s_onMessageFlush);
        MessageData differentMessage;
        differentMessage.SetBooleanMessage(true);
        echoMessageRequest.SetMessage(differentMessage);
        echoMessage.Activate(echoMessageRequest, s_onMessageFlush);
        echoMessage.Close();
        echoMessage.Close();
        echoMessage.Close();
        echoMessage.Close();
    }

    /* Close without waiting for TERMINATE_STREAM to flush then immediately trying to activate. */
    {
        Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);

        auto connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);
        auto echoMessage = client.NewEchoMessage();
        messageData.SetStringMessage(expectedMessage);
        echoMessageRequest.SetMessage(messageData);
        auto requestFuture = echoMessage.Activate(echoMessageRequest, s_onMessageFlush);
        requestFuture = echoMessage.Activate(echoMessageRequest, s_onMessageFlush);
        MessageData differentMessage;
        differentMessage.SetBooleanMessage(true);
        echoMessageRequest.SetMessage(differentMessage);
        requestFuture = echoMessage.Activate(echoMessageRequest, s_onMessageFlush);
        requestFuture.wait();
        auto closeFuture = echoMessage.Close();
        requestFuture = echoMessage.Activate(echoMessageRequest, s_onMessageFlush);
        closeFuture.wait();
        requestFuture.wait();
    }

    /* Connect thrice and verify that the future of the first attempt succeeds.
     * The rest of the attempts must fail with an error.
     * Use the client to perform an operation and verify that the operation still succeeds. */
    {
        Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);
        auto connectedStatus = client.Connect(lifecycleHandler);
        auto failedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(failedStatus.get().baseStatus == EVENT_STREAM_RPC_CONNECTION_ALREADY_ESTABLISHED);
        failedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);
        ASSERT_TRUE(failedStatus.get().baseStatus == EVENT_STREAM_RPC_CONNECTION_ALREADY_ESTABLISHED);
        auto echoMessage = client.NewEchoMessage();
        echoMessageRequest.SetMessage(messageData);
        auto requestFuture = echoMessage.Activate(echoMessageRequest, s_onMessageFlush);
        requestFuture.wait();
        auto result = echoMessage.GetResult().get();
        ASSERT_TRUE(result);
        auto response = result.GetOperationResponse();
        ASSERT_NOT_NULL(response);
        ASSERT_TRUE(response->GetMessage().value().GetStringMessage().value() == expectedMessage);
    }

    /* Connect twice sequentially.
     * Use the client to perform an operation and verify that the operation still succeeds. */
    {
        Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);
        auto connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);
        connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_CONNECTION_ALREADY_ESTABLISHED);
        auto echoMessage = client.NewEchoMessage();
        echoMessageRequest.SetMessage(messageData);
        auto requestFuture = echoMessage.Activate(echoMessageRequest, s_onMessageFlush);
        requestFuture.wait();
        auto result = echoMessage.GetResult().get();
        ASSERT_TRUE(result);
        auto response = result.GetOperationResponse();
        ASSERT_NOT_NULL(response);
        ASSERT_TRUE(response->GetMessage().value().GetStringMessage().value() == expectedMessage);
    }

    return AWS_OP_SUCCESS;
}

class ThreadPool
{
  public:
    ThreadPool(int numThreads = std::thread::hardware_concurrency()) noexcept
        : m_numThreads(numThreads), m_stopped(false)
    {
        for (int i = 0; i < numThreads; i++)
        {
            m_threadPool.push_back(std::thread(&ThreadPool::TaskWorker, this));
        }
        m_taskErrorCode = AWS_OP_SUCCESS;
    }

    void AddTask(std::function<int()> task) noexcept
    {
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            m_queue.push(task);
        }
        m_taskReady.notify_one();
    }

    void Shutdown() noexcept
    {
        /* Wake up all threads so that they can complete. */
        m_taskReady.notify_all();

        /* Wait for all threads to complete. */
        for (std::thread &thread : m_threadPool)
        {
            thread.join();
        }

        m_threadPool.clear();
        m_stopped = true;
    }

    void BlockUntilTasksFinish() noexcept
    {
        while (true)
        {
            if (m_queueMutex.try_lock())
            {
                if (m_queue.empty())
                {
                    m_stopped = true;
                    m_queueMutex.unlock();
                    /* Wait for all threads to complete. */
                    m_taskReady.notify_all();
                    for (std::thread &thread : m_threadPool)
                    {
                        thread.join();
                    }
                    break;
                }
                else
                {
                    m_queueMutex.unlock();
                    std::this_thread::yield();
                }
            }
        }
    }

    int GetErrorCode() noexcept { return m_taskErrorCode; }

    ~ThreadPool() noexcept
    {
        if (!m_stopped)
            Shutdown();
    }

  private:
    int m_numThreads;
    std::mutex m_poolMutex;
    std::vector<std::thread> m_threadPool;
    std::mutex m_queueMutex;
    std::queue<std::function<int()>> m_queue;
    std::condition_variable m_taskReady;
    int m_taskErrorCode;
    bool m_stopped;

    void TaskWorker()
    {
        while (true)
        {
            {
                std::unique_lock<std::mutex> lock(m_queueMutex);

                m_taskReady.wait(lock, [this] { return !m_queue.empty() || m_stopped; });
                if (!m_queue.empty())
                {
                    std::function<int()> currentJob = m_queue.front();
                    m_queue.pop();
                    lock.unlock();
                    if (currentJob)
                    {
                        int errorCode = currentJob();
                        if (errorCode)
                            m_taskErrorCode = errorCode;
                    }
                }
                else if (m_stopped)
                {
                    break;
                }
            }
        }
    }
};

AWS_TEST_CASE_FIXTURE(StressTestClient, s_testSetup, s_TestStressClient, s_testTeardown, &s_testContext);
static int s_TestStressClient(struct aws_allocator *allocator, void *ctx)
{
    auto *testContext = static_cast<EventStreamClientTestContext *>(ctx);
    ThreadPool threadPool;
    ConnectionLifecycleHandler lifecycleHandler;
    Aws::Crt::String expectedMessage("Async I0 FTW");
    EchoMessageRequest echoMessageRequest;
    MessageData messageData;
    messageData.SetStringMessage(expectedMessage);

    {
        Awstest::EchoTestRpcClient client(*testContext->clientBootstrap, allocator);
        auto connectedStatus = client.Connect(lifecycleHandler);
        ASSERT_TRUE(connectedStatus.get().baseStatus == EVENT_STREAM_RPC_SUCCESS);
        auto invokeOperation = [&](void) -> int {
            auto echoMessage = client.NewEchoMessage();
            messageData.SetStringMessage(expectedMessage);
            echoMessageRequest.SetMessage(messageData);
            auto requestStatus = echoMessage.Activate(echoMessageRequest, s_onMessageFlush).get();
            auto resultFuture = echoMessage.GetResult();
            /* The response may never arrive depending on how many ongoing requests are made
             * so in case of timeout, assume success. */
            std::future_status status = resultFuture.wait_for(std::chrono::seconds(5));
            if (status != std::future_status::ready)
            {
                return AWS_OP_SUCCESS;
            }
            auto result = echoMessage.GetResult().get();
            ASSERT_TRUE(result);
            auto response = result.GetOperationResponse();
            ASSERT_NOT_NULL(response);
            ASSERT_TRUE(response->GetMessage().value().GetStringMessage().value() == expectedMessage);

            return AWS_OP_SUCCESS;
        };

        for (int i = 0; i < 1000; i++)
            threadPool.AddTask(invokeOperation);

        threadPool.BlockUntilTasksFinish();

        if (threadPool.GetErrorCode() != AWS_OP_SUCCESS)
            return threadPool.GetErrorCode();
    }

    return AWS_OP_SUCCESS;
}
