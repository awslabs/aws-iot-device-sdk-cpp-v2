#include <aws/crt/io/Bootstrap.h>
#include <awstest/EchoTestRpcClient.h>

namespace Awstest
{
    EchoTestRpcClient::EchoTestRpcClient(
        Aws::Crt::Io::ClientBootstrap &clientBootstrap,
        Aws::Crt::Allocator *allocator) noexcept
        : m_connection(allocator), m_clientBootstrap(clientBootstrap), m_allocator(allocator)
    {
        m_echoTestRpcServiceModel.AssignModelNameToErrorResponse(
            Aws::Crt::String("awstest#ServiceError"), ServiceError::s_allocateFromPayload);
    }

    std::future<RpcError> EchoTestRpcClient::Connect(
        ConnectionLifecycleHandler &lifecycleHandler,
        const ConnectionConfig &connectionConfig) noexcept
    {
        return m_connection.Connect(connectionConfig, &lifecycleHandler, m_clientBootstrap);
    }

    void EchoTestRpcClient::Close() noexcept { m_connection.Close(); }

    EchoTestRpcClient::~EchoTestRpcClient() noexcept { Close(); }

    GetAllProductsOperation EchoTestRpcClient::NewGetAllProducts() noexcept
    {
        return GetAllProductsOperation(
            m_connection, m_echoTestRpcServiceModel.m_getAllProductsOperationContext, m_allocator);
    }
    CauseServiceErrorOperation EchoTestRpcClient::NewCauseServiceError() noexcept
    {
        return CauseServiceErrorOperation(
            m_connection, m_echoTestRpcServiceModel.m_causeServiceErrorOperationContext, m_allocator);
    }
    CauseStreamServiceToErrorOperation EchoTestRpcClient::NewCauseStreamServiceToError(
        CauseStreamServiceToErrorStreamHandler &streamHandler) noexcept
    {
        return CauseStreamServiceToErrorOperation(
            m_connection,
            &streamHandler,
            m_echoTestRpcServiceModel.m_causeStreamServiceToErrorOperationContext,
            m_allocator);
    }
    EchoStreamMessagesOperation EchoTestRpcClient::NewEchoStreamMessages(
        EchoStreamMessagesStreamHandler &streamHandler) noexcept
    {
        return EchoStreamMessagesOperation(
            m_connection, &streamHandler, m_echoTestRpcServiceModel.m_echoStreamMessagesOperationContext, m_allocator);
    }
    EchoMessageOperation EchoTestRpcClient::NewEchoMessage() noexcept
    {
        return EchoMessageOperation(m_connection, m_echoTestRpcServiceModel.m_echoMessageOperationContext, m_allocator);
    }
    GetAllCustomersOperation EchoTestRpcClient::NewGetAllCustomers() noexcept
    {
        return GetAllCustomersOperation(
            m_connection, m_echoTestRpcServiceModel.m_getAllCustomersOperationContext, m_allocator);
    }

} // namespace Awstest
