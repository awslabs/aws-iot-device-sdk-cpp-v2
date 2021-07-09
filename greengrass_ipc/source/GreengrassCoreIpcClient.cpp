#include <aws/crt/io/Bootstrap.h>
#include <aws/greengrass/GreengrassCoreIpcClient.h>

namespace Aws
{
    namespace Greengrass
    {
        GreengrassCoreIpcClient::GreengrassCoreIpcClient(
            Aws::Crt::Io::ClientBootstrap &clientBootstrap,
            Aws::Crt::Allocator *allocator) noexcept
            : m_connection(allocator), m_clientBootstrap(clientBootstrap), m_allocator(allocator)
        {
            m_greengrassCoreIpcServiceModel.AssignModelNameToErrorResponse(
                Aws::Crt::String("aws.greengrass#InvalidTokenError"), InvalidTokenError::s_allocateFromPayload);
            m_greengrassCoreIpcServiceModel.AssignModelNameToErrorResponse(
                Aws::Crt::String("aws.greengrass#FailedUpdateConditionCheckError"),
                FailedUpdateConditionCheckError::s_allocateFromPayload);
            m_greengrassCoreIpcServiceModel.AssignModelNameToErrorResponse(
                Aws::Crt::String("aws.greengrass#ConflictError"), ConflictError::s_allocateFromPayload);
            m_greengrassCoreIpcServiceModel.AssignModelNameToErrorResponse(
                Aws::Crt::String("aws.greengrass#ComponentNotFoundError"),
                ComponentNotFoundError::s_allocateFromPayload);
            m_greengrassCoreIpcServiceModel.AssignModelNameToErrorResponse(
                Aws::Crt::String("aws.greengrass#ResourceNotFoundError"), ResourceNotFoundError::s_allocateFromPayload);
            m_greengrassCoreIpcServiceModel.AssignModelNameToErrorResponse(
                Aws::Crt::String("aws.greengrass#InvalidArgumentsError"), InvalidArgumentsError::s_allocateFromPayload);
            m_greengrassCoreIpcServiceModel.AssignModelNameToErrorResponse(
                Aws::Crt::String("aws.greengrass#InvalidArtifactsDirectoryPathError"),
                InvalidArtifactsDirectoryPathError::s_allocateFromPayload);
            m_greengrassCoreIpcServiceModel.AssignModelNameToErrorResponse(
                Aws::Crt::String("aws.greengrass#InvalidRecipeDirectoryPathError"),
                InvalidRecipeDirectoryPathError::s_allocateFromPayload);
            m_greengrassCoreIpcServiceModel.AssignModelNameToErrorResponse(
                Aws::Crt::String("aws.greengrass#ServiceError"), ServiceError::s_allocateFromPayload);
            m_greengrassCoreIpcServiceModel.AssignModelNameToErrorResponse(
                Aws::Crt::String("aws.greengrass#UnauthorizedError"), UnauthorizedError::s_allocateFromPayload);
        }

        std::future<RpcError> GreengrassCoreIpcClient::Connect(
            ConnectionLifecycleHandler &lifecycleHandler,
            const ConnectionConfig &connectionConfig) noexcept
        {
            return m_connection.Connect(connectionConfig, &lifecycleHandler, m_clientBootstrap);
        }

        void GreengrassCoreIpcClient::Close() noexcept { m_connection.Close(); }

        GreengrassCoreIpcClient::~GreengrassCoreIpcClient() noexcept { Close(); }

        SubscribeToIoTCoreOperation GreengrassCoreIpcClient::NewSubscribeToIoTCore(
            SubscribeToIoTCoreStreamHandler &streamHandler) noexcept
        {
            return SubscribeToIoTCoreOperation(
                m_connection,
                &streamHandler,
                m_greengrassCoreIpcServiceModel.m_subscribeToIoTCoreOperationContext,
                m_allocator);
        }
        PublishToIoTCoreOperation GreengrassCoreIpcClient::NewPublishToIoTCore() noexcept
        {
            return PublishToIoTCoreOperation(
                m_connection, m_greengrassCoreIpcServiceModel.m_publishToIoTCoreOperationContext, m_allocator);
        }
        SubscribeToConfigurationUpdateOperation GreengrassCoreIpcClient::NewSubscribeToConfigurationUpdate(
            SubscribeToConfigurationUpdateStreamHandler &streamHandler) noexcept
        {
            return SubscribeToConfigurationUpdateOperation(
                m_connection,
                &streamHandler,
                m_greengrassCoreIpcServiceModel.m_subscribeToConfigurationUpdateOperationContext,
                m_allocator);
        }
        DeleteThingShadowOperation GreengrassCoreIpcClient::NewDeleteThingShadow() noexcept
        {
            return DeleteThingShadowOperation(
                m_connection, m_greengrassCoreIpcServiceModel.m_deleteThingShadowOperationContext, m_allocator);
        }
        DeferComponentUpdateOperation GreengrassCoreIpcClient::NewDeferComponentUpdate() noexcept
        {
            return DeferComponentUpdateOperation(
                m_connection, m_greengrassCoreIpcServiceModel.m_deferComponentUpdateOperationContext, m_allocator);
        }
        SubscribeToValidateConfigurationUpdatesOperation GreengrassCoreIpcClient::
            NewSubscribeToValidateConfigurationUpdates(
                SubscribeToValidateConfigurationUpdatesStreamHandler &streamHandler) noexcept
        {
            return SubscribeToValidateConfigurationUpdatesOperation(
                m_connection,
                &streamHandler,
                m_greengrassCoreIpcServiceModel.m_subscribeToValidateConfigurationUpdatesOperationContext,
                m_allocator);
        }
        GetConfigurationOperation GreengrassCoreIpcClient::NewGetConfiguration() noexcept
        {
            return GetConfigurationOperation(
                m_connection, m_greengrassCoreIpcServiceModel.m_getConfigurationOperationContext, m_allocator);
        }
        SubscribeToTopicOperation GreengrassCoreIpcClient::NewSubscribeToTopic(
            SubscribeToTopicStreamHandler &streamHandler) noexcept
        {
            return SubscribeToTopicOperation(
                m_connection,
                &streamHandler,
                m_greengrassCoreIpcServiceModel.m_subscribeToTopicOperationContext,
                m_allocator);
        }
        GetComponentDetailsOperation GreengrassCoreIpcClient::NewGetComponentDetails() noexcept
        {
            return GetComponentDetailsOperation(
                m_connection, m_greengrassCoreIpcServiceModel.m_getComponentDetailsOperationContext, m_allocator);
        }
        PublishToTopicOperation GreengrassCoreIpcClient::NewPublishToTopic() noexcept
        {
            return PublishToTopicOperation(
                m_connection, m_greengrassCoreIpcServiceModel.m_publishToTopicOperationContext, m_allocator);
        }
        ListComponentsOperation GreengrassCoreIpcClient::NewListComponents() noexcept
        {
            return ListComponentsOperation(
                m_connection, m_greengrassCoreIpcServiceModel.m_listComponentsOperationContext, m_allocator);
        }
        CreateDebugPasswordOperation GreengrassCoreIpcClient::NewCreateDebugPassword() noexcept
        {
            return CreateDebugPasswordOperation(
                m_connection, m_greengrassCoreIpcServiceModel.m_createDebugPasswordOperationContext, m_allocator);
        }
        GetThingShadowOperation GreengrassCoreIpcClient::NewGetThingShadow() noexcept
        {
            return GetThingShadowOperation(
                m_connection, m_greengrassCoreIpcServiceModel.m_getThingShadowOperationContext, m_allocator);
        }
        SendConfigurationValidityReportOperation GreengrassCoreIpcClient::NewSendConfigurationValidityReport() noexcept
        {
            return SendConfigurationValidityReportOperation(
                m_connection,
                m_greengrassCoreIpcServiceModel.m_sendConfigurationValidityReportOperationContext,
                m_allocator);
        }
        UpdateThingShadowOperation GreengrassCoreIpcClient::NewUpdateThingShadow() noexcept
        {
            return UpdateThingShadowOperation(
                m_connection, m_greengrassCoreIpcServiceModel.m_updateThingShadowOperationContext, m_allocator);
        }
        UpdateConfigurationOperation GreengrassCoreIpcClient::NewUpdateConfiguration() noexcept
        {
            return UpdateConfigurationOperation(
                m_connection, m_greengrassCoreIpcServiceModel.m_updateConfigurationOperationContext, m_allocator);
        }
        ValidateAuthorizationTokenOperation GreengrassCoreIpcClient::NewValidateAuthorizationToken() noexcept
        {
            return ValidateAuthorizationTokenOperation(
                m_connection,
                m_greengrassCoreIpcServiceModel.m_validateAuthorizationTokenOperationContext,
                m_allocator);
        }
        RestartComponentOperation GreengrassCoreIpcClient::NewRestartComponent() noexcept
        {
            return RestartComponentOperation(
                m_connection, m_greengrassCoreIpcServiceModel.m_restartComponentOperationContext, m_allocator);
        }
        GetLocalDeploymentStatusOperation GreengrassCoreIpcClient::NewGetLocalDeploymentStatus() noexcept
        {
            return GetLocalDeploymentStatusOperation(
                m_connection, m_greengrassCoreIpcServiceModel.m_getLocalDeploymentStatusOperationContext, m_allocator);
        }
        GetSecretValueOperation GreengrassCoreIpcClient::NewGetSecretValue() noexcept
        {
            return GetSecretValueOperation(
                m_connection, m_greengrassCoreIpcServiceModel.m_getSecretValueOperationContext, m_allocator);
        }
        UpdateStateOperation GreengrassCoreIpcClient::NewUpdateState() noexcept
        {
            return UpdateStateOperation(
                m_connection, m_greengrassCoreIpcServiceModel.m_updateStateOperationContext, m_allocator);
        }
        ListNamedShadowsForThingOperation GreengrassCoreIpcClient::NewListNamedShadowsForThing() noexcept
        {
            return ListNamedShadowsForThingOperation(
                m_connection, m_greengrassCoreIpcServiceModel.m_listNamedShadowsForThingOperationContext, m_allocator);
        }
        SubscribeToComponentUpdatesOperation GreengrassCoreIpcClient::NewSubscribeToComponentUpdates(
            SubscribeToComponentUpdatesStreamHandler &streamHandler) noexcept
        {
            return SubscribeToComponentUpdatesOperation(
                m_connection,
                &streamHandler,
                m_greengrassCoreIpcServiceModel.m_subscribeToComponentUpdatesOperationContext,
                m_allocator);
        }
        ListLocalDeploymentsOperation GreengrassCoreIpcClient::NewListLocalDeployments() noexcept
        {
            return ListLocalDeploymentsOperation(
                m_connection, m_greengrassCoreIpcServiceModel.m_listLocalDeploymentsOperationContext, m_allocator);
        }
        StopComponentOperation GreengrassCoreIpcClient::NewStopComponent() noexcept
        {
            return StopComponentOperation(
                m_connection, m_greengrassCoreIpcServiceModel.m_stopComponentOperationContext, m_allocator);
        }
        CreateLocalDeploymentOperation GreengrassCoreIpcClient::NewCreateLocalDeployment() noexcept
        {
            return CreateLocalDeploymentOperation(
                m_connection, m_greengrassCoreIpcServiceModel.m_createLocalDeploymentOperationContext, m_allocator);
        }

    } // namespace Greengrass
} // namespace Aws
