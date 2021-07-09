#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

/* This file is generated. */

#include <aws/greengrass/GreengrassCoreIpcModel.h>

using namespace Aws::Eventstreamrpc;

namespace Aws
{
    namespace Greengrass
    {
        class DefaultConnectionConfig : public ConnectionConfig
        {
          public:
            DefaultConnectionConfig() noexcept;
        };

        class GreengrassCoreIpcClient
        {
          public:
            GreengrassCoreIpcClient(
                Aws::Crt::Io::ClientBootstrap &clientBootstrap,
                Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
            /**
             * Connect the client to the server
             * @param lifecycleHandler An interface that is called upon when lifecycle events relating to the connection
             * occur.
             * @param connectionConfig The configuration parameters used for establishing the connection.
             * @return An `RpcError` that can be used to check whether the connection was established.
             */
            std::future<RpcError> Connect(
                ConnectionLifecycleHandler &lifecycleHandler,
                const ConnectionConfig &connectionConfig = DefaultConnectionConfig()) noexcept;
            bool IsConnected() const noexcept { return m_connection.IsOpen(); }
            void Close() noexcept;
            SubscribeToIoTCoreOperation NewSubscribeToIoTCore(SubscribeToIoTCoreStreamHandler &) noexcept;
            PublishToIoTCoreOperation NewPublishToIoTCore() noexcept;
            SubscribeToConfigurationUpdateOperation NewSubscribeToConfigurationUpdate(
                SubscribeToConfigurationUpdateStreamHandler &) noexcept;
            DeleteThingShadowOperation NewDeleteThingShadow() noexcept;
            DeferComponentUpdateOperation NewDeferComponentUpdate() noexcept;
            SubscribeToValidateConfigurationUpdatesOperation NewSubscribeToValidateConfigurationUpdates(
                SubscribeToValidateConfigurationUpdatesStreamHandler &) noexcept;
            GetConfigurationOperation NewGetConfiguration() noexcept;
            SubscribeToTopicOperation NewSubscribeToTopic(SubscribeToTopicStreamHandler &) noexcept;
            GetComponentDetailsOperation NewGetComponentDetails() noexcept;
            PublishToTopicOperation NewPublishToTopic() noexcept;
            ListComponentsOperation NewListComponents() noexcept;
            CreateDebugPasswordOperation NewCreateDebugPassword() noexcept;
            GetThingShadowOperation NewGetThingShadow() noexcept;
            SendConfigurationValidityReportOperation NewSendConfigurationValidityReport() noexcept;
            UpdateThingShadowOperation NewUpdateThingShadow() noexcept;
            UpdateConfigurationOperation NewUpdateConfiguration() noexcept;
            ValidateAuthorizationTokenOperation NewValidateAuthorizationToken() noexcept;
            RestartComponentOperation NewRestartComponent() noexcept;
            GetLocalDeploymentStatusOperation NewGetLocalDeploymentStatus() noexcept;
            GetSecretValueOperation NewGetSecretValue() noexcept;
            UpdateStateOperation NewUpdateState() noexcept;
            ListNamedShadowsForThingOperation NewListNamedShadowsForThing() noexcept;
            SubscribeToComponentUpdatesOperation NewSubscribeToComponentUpdates(
                SubscribeToComponentUpdatesStreamHandler &) noexcept;
            ListLocalDeploymentsOperation NewListLocalDeployments() noexcept;
            StopComponentOperation NewStopComponent() noexcept;
            CreateLocalDeploymentOperation NewCreateLocalDeployment() noexcept;
            ~GreengrassCoreIpcClient() noexcept;

          private:
            GreengrassCoreIpcServiceModel m_greengrassCoreIpcServiceModel;
            ClientConnection m_connection;
            Aws::Crt::Io::ClientBootstrap &m_clientBootstrap;
            Aws::Crt::Allocator *m_allocator;
            MessageAmendment m_connectAmendment;
        };
    } // namespace Greengrass
} // namespace Aws
