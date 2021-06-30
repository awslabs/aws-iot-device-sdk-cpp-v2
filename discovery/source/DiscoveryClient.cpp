/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/discovery/DiscoveryClient.h>

#include <aws/crt/Api.h>
#include <aws/crt/Types.h>
#include <aws/crt/http/HttpRequestResponse.h>
#include <aws/crt/io/TlsOptions.h>
#include <aws/crt/io/Uri.h>

namespace Aws
{
    namespace Discovery
    {
        DiscoveryClientConfig::DiscoveryClientConfig() noexcept
            : Bootstrap(nullptr), TlsContext(), SocketOptions(), Region(), MaxConnections(2), ProxyOptions()
        {
        }

        DiscoveryClient::DiscoveryClient(
            const Aws::Discovery::DiscoveryClientConfig &clientConfig,
            Crt::Allocator *allocator) noexcept
        {
            AWS_FATAL_ASSERT(clientConfig.TlsContext);
            AWS_FATAL_ASSERT(clientConfig.Bootstrap);

            m_allocator = allocator;

            Crt::StringStream ss;

            // Temporary fix for connection with china endpoint
            if (clientConfig.Region == "cn-north-1")
            {
                ss << "greengrass.ats.iot." << clientConfig.Region << ".amazonaws.com.cn";
            }
            else
            {
                ss << "greengrass-ats.iot." << clientConfig.Region << ".amazonaws.com";
            }

            Crt::Io::TlsConnectionOptions tlsConnectionOptions = clientConfig.TlsContext->NewConnectionOptions();
            uint16_t port = 443;

            if (Crt::Io::TlsContextOptions::IsAlpnSupported())
            {
                tlsConnectionOptions.SetAlpnList("x-amzn-http-ca");
            }
            else
            {
                port = 8883;
            }

            m_hostName = ss.str();
            Crt::ByteCursor serverName = Crt::ByteCursorFromCString(m_hostName.c_str());
            tlsConnectionOptions.SetServerName(serverName);

            Crt::Http::HttpClientConnectionOptions connectionOptions;
            connectionOptions.SocketOptions = clientConfig.SocketOptions;
            connectionOptions.Bootstrap = clientConfig.Bootstrap;
            connectionOptions.TlsOptions = tlsConnectionOptions;
            connectionOptions.HostName = Crt::String((const char *)serverName.ptr, serverName.len);
            connectionOptions.Port = port;
            if (clientConfig.ProxyOptions)
            {
                connectionOptions.ProxyOptions = *clientConfig.ProxyOptions;
            }

            Crt::Http::HttpClientConnectionManagerOptions connectionManagerOptions;
            connectionManagerOptions.ConnectionOptions = connectionOptions;
            connectionManagerOptions.MaxConnections = clientConfig.MaxConnections;

            m_connectionManager =
                Crt::Http::HttpClientConnectionManager::NewClientConnectionManager(connectionManagerOptions, allocator);
        }

        std::shared_ptr<DiscoveryClient> DiscoveryClient::CreateClient(
            const Aws::Discovery::DiscoveryClientConfig &clientConfig,
            Crt::Allocator *allocator)
        {
            auto *toSeat = static_cast<DiscoveryClient *>(aws_mem_acquire(allocator, sizeof(DiscoveryClient)));
            if (toSeat)
            {
                toSeat = new (toSeat) DiscoveryClient(clientConfig, allocator);
                return std::shared_ptr<DiscoveryClient>(
                    toSeat, [allocator](DiscoveryClient *client) { Crt::Delete(client, allocator); });
            }

            return nullptr;
        }

        struct ClientCallbackContext
        {
            Crt::StringStream ss;
            int responseCode;
        };

        bool DiscoveryClient::Discover(
            const Crt::String &thingName,
            const OnDiscoverResponse &onDiscoverResponse) noexcept
        {
            auto callbackContext = Crt::MakeShared<ClientCallbackContext>(m_allocator);
            if (!callbackContext)
            {
                return false;
            }

            callbackContext->responseCode = 0;

            bool res = m_connectionManager->AcquireConnection(
                [this, callbackContext, thingName, onDiscoverResponse](
                    std::shared_ptr<Crt::Http::HttpClientConnection> connection, int errorCode) {
                    if (errorCode)
                    {
                        onDiscoverResponse(nullptr, errorCode, 0);
                        return;
                    }

                    auto request = Aws::Crt::MakeShared<Crt::Http::HttpRequest>(m_allocator, m_allocator);
                    if (request == nullptr)
                    {
                        onDiscoverResponse(nullptr, Crt::LastErrorOrUnknown(), 0);
                        return;
                    }

                    Crt::StringStream ss;
                    ss << "/greengrass/discover/thing/" << thingName;
                    Crt::String uriStr = ss.str();
                    if (!request->SetMethod(Crt::ByteCursorFromCString("GET")))
                    {
                        onDiscoverResponse(nullptr, Crt::LastErrorOrUnknown(), 0);
                        return;
                    }

                    if (!request->SetPath(Crt::ByteCursorFromCString(uriStr.c_str())))
                    {
                        onDiscoverResponse(nullptr, Crt::LastErrorOrUnknown(), 0);
                        return;
                    }

                    Crt::Http::HttpHeader hostNameHeader;
                    hostNameHeader.name = Crt::ByteCursorFromCString("host");
                    hostNameHeader.value = Crt::ByteCursorFromCString(m_hostName.c_str());

                    if (!request->AddHeader(hostNameHeader))
                    {
                        onDiscoverResponse(nullptr, Crt::LastErrorOrUnknown(), 0);
                        return;
                    }

                    Crt::Http::HttpRequestOptions requestOptions;
                    requestOptions.request = request.get();
                    requestOptions.onIncomingHeaders =
                        [](Crt::Http::HttpStream &, aws_http_header_block, const Crt::Http::HttpHeader *, std::size_t) {
                        };
                    requestOptions.onIncomingHeadersBlockDone =
                        [callbackContext](Crt::Http::HttpStream &stream, aws_http_header_block) {
                            callbackContext->responseCode = stream.GetResponseStatusCode();
                        };
                    requestOptions.onIncomingBody =
                        [callbackContext](Crt::Http::HttpStream &, const Crt::ByteCursor &data) {
                            callbackContext->ss.write(reinterpret_cast<const char *>(data.ptr), data.len);
                        };
                    requestOptions.onStreamComplete = [request, connection, callbackContext, onDiscoverResponse](
                                                          Crt::Http::HttpStream &, int errorCode) {
                        if (!errorCode && callbackContext->responseCode == 200)
                        {
                            Crt::JsonObject jsonObject(callbackContext->ss.str());
                            DiscoverResponse response(jsonObject.View());
                            onDiscoverResponse(&response, AWS_ERROR_SUCCESS, callbackContext->responseCode);
                        }
                        else
                        {
                            if (!errorCode)
                            {
                                errorCode = AWS_ERROR_UNKNOWN;
                            }
                            onDiscoverResponse(nullptr, errorCode, callbackContext->responseCode);
                        }
                    };

                    auto stream = connection->NewClientStream(requestOptions);
                    if (!stream)
                    {
                        onDiscoverResponse(nullptr, Crt::LastErrorOrUnknown(), 0);
                    }

                    if (!stream->Activate())
                    {
                        onDiscoverResponse(nullptr, Crt::LastErrorOrUnknown(), 0);
                    }
                });

            if (!res)
            {
                return false;
            }

            return true;
        }
    } // namespace Discovery
} // namespace Aws
