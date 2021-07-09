/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */
#include <aws/eventstreamrpc/EventStreamClient.h>

#include <aws/crt/Api.h>
#include <aws/crt/Config.h>
#include <aws/crt/auth/Credentials.h>

#include <stdint.h>
#include <string.h>

#include <algorithm>

constexpr auto EVENTSTREAM_VERSION_HEADER = ":version";
constexpr auto EVENTSTREAM_VERSION_STRING = "0.1.0";
constexpr auto CONTENT_TYPE_HEADER = ":content-type";
constexpr auto CONTENT_TYPE_APPLICATION_JSON = "application/json";
constexpr auto SERVICE_MODEL_TYPE_HEADER = "service-model-type";

namespace Aws
{
    namespace Eventstreamrpc
    {
        /* Because `std::function` cannot be typecasted to void *, we must contain it in a struct. */
        struct OnMessageFlushCallbackContainer
        {
            explicit OnMessageFlushCallbackContainer(Crt::Allocator *allocator) : allocator(allocator) {}
            Crt::Allocator *allocator;
            OnMessageFlushCallback onMessageFlushCallback;
            std::promise<RpcError> onFlushPromise;
        };

        MessageAmendment::MessageAmendment(Crt::Allocator *allocator) noexcept
            : m_headers(), m_payload(), m_allocator(allocator)
        {
        }

        MessageAmendment::MessageAmendment(const Crt::ByteBuf &payload, Crt::Allocator *allocator) noexcept
            : m_headers(), m_payload(Crt::ByteBufNewCopy(allocator, payload.buffer, payload.len)),
              m_allocator(allocator)
        {
        }

        MessageAmendment::MessageAmendment(
            const Crt::List<EventStreamHeader> &headers,
            Crt::Allocator *allocator) noexcept
            : m_headers(headers), m_payload(), m_allocator(allocator)
        {
        }

        MessageAmendment::MessageAmendment(Crt::List<EventStreamHeader> &&headers, Crt::Allocator *allocator) noexcept
            : m_headers(headers), m_payload(), m_allocator(allocator)
        {
        }

        MessageAmendment::MessageAmendment(
            const Crt::List<EventStreamHeader> &headers,
            Crt::Optional<Crt::ByteBuf> &payload,
            Crt::Allocator *allocator) noexcept
            : m_headers(headers), m_payload(), m_allocator(allocator)
        {
            if (payload.has_value())
            {
                m_payload = Crt::ByteBufNewCopy(allocator, payload.value().buffer, payload.value().len);
            }
        }

        MessageAmendment::MessageAmendment(const MessageAmendment &lhs)
            : m_headers(lhs.m_headers), m_payload(), m_allocator(lhs.m_allocator)
        {
            if (lhs.m_payload.has_value())
            {
                m_payload =
                    Crt::ByteBufNewCopy(lhs.m_allocator, lhs.m_payload.value().buffer, lhs.m_payload.value().len);
            }
        }

        MessageAmendment &MessageAmendment::operator=(const MessageAmendment &lhs)
        {
            m_headers = lhs.m_headers;
            if (lhs.m_payload.has_value())
            {
                m_payload =
                    Crt::ByteBufNewCopy(lhs.m_allocator, lhs.m_payload.value().buffer, lhs.m_payload.value().len);
            }
            m_allocator = lhs.m_allocator;

            return *this;
        }

        MessageAmendment::MessageAmendment(MessageAmendment &&rhs)
            : m_headers(std::move(rhs.m_headers)), m_payload(rhs.m_payload), m_allocator(rhs.m_allocator)
        {
            rhs.m_allocator = nullptr;
            rhs.m_payload = Crt::Optional<Crt::ByteBuf>();
        }

        const Crt::List<EventStreamHeader> &MessageAmendment::GetHeaders() const noexcept { return m_headers; }

        const Crt::Optional<Crt::ByteBuf> &MessageAmendment::GetPayload() const noexcept { return m_payload; }

        void MessageAmendment::SetPayload(const Crt::Optional<Crt::ByteBuf> &payload) noexcept
        {
            if (payload.has_value())
            {
                m_payload = Crt::ByteBufNewCopy(m_allocator, payload.value().buffer, payload.value().len);
            }
        }

        MessageAmendment::~MessageAmendment() noexcept
        {
            if (m_payload.has_value())
            {
                Crt::ByteBufDelete(m_payload.value());
            }
        }

        class EventStreamCppToNativeCrtBuilder
        {
          private:
            friend class ClientConnection;
            friend class ClientContinuation;
            static int s_fillNativeHeadersArray(
                const Crt::List<EventStreamHeader> &headers,
                struct aws_array_list *headersArray,
                Crt::Allocator *m_allocator = Crt::g_allocator)
            {
                AWS_ZERO_STRUCT(*headersArray);
                /* Check if the connection has expired before attempting to send. */
                int errorCode = aws_event_stream_headers_list_init(headersArray, m_allocator);

                if (!errorCode)
                {
                    /* Populate the array with the underlying handle of each EventStreamHeader. */
                    for (auto &i : headers)
                    {
                        errorCode = aws_array_list_push_back(headersArray, i.GetUnderlyingHandle());

                        if (errorCode)
                        {
                            break;
                        }
                    }
                }

                return errorCode;
            }
        };

        ClientConnection &ClientConnection::operator=(ClientConnection &&rhs) noexcept
        {
            m_allocator = std::move(rhs.m_allocator);
            m_underlyingConnection = rhs.m_underlyingConnection;
            rhs.m_stateMutex.lock();
            m_clientState = rhs.m_clientState;
            rhs.m_stateMutex.unlock();
            m_lifecycleHandler = rhs.m_lifecycleHandler;
            m_connectMessageAmender = rhs.m_connectMessageAmender;
            m_connectAckedPromise = std::move(rhs.m_connectAckedPromise);
            m_closedPromise = std::move(rhs.m_closedPromise);
            m_onConnectRequestCallback = rhs.m_onConnectRequestCallback;

            /* Reset rhs. */
            rhs.m_allocator = nullptr;
            rhs.m_underlyingConnection = nullptr;
            rhs.m_clientState = DISCONNECTED;
            rhs.m_lifecycleHandler = nullptr;
            rhs.m_connectMessageAmender = nullptr;
            rhs.m_closedPromise = {};
            rhs.m_onConnectRequestCallback = nullptr;

            return *this;
        }

        ClientConnection::ClientConnection(ClientConnection &&rhs) noexcept : m_lifecycleHandler(rhs.m_lifecycleHandler)
        {
            *this = std::move(rhs);
        }

        ClientConnection::ClientConnection(Crt::Allocator *allocator) noexcept
            : m_allocator(allocator), m_underlyingConnection(nullptr), m_clientState(DISCONNECTED),
              m_lifecycleHandler(nullptr), m_connectMessageAmender(nullptr), m_connectionWillSetup(false),
              m_onConnectRequestCallback(nullptr)
        {
        }

        ClientConnection::~ClientConnection() noexcept
        {
            m_stateMutex.lock();
            if (m_connectionWillSetup)
            {
                m_stateMutex.unlock();
                m_connectionSetupPromise.get_future().wait();
            }
            m_stateMutex.lock();
            if (m_clientState != DISCONNECTED)
            {
                Close();
                m_stateMutex.unlock();
                m_closedPromise.get_future().wait();
            }
            /* Cover the case in which the if statements are not hit. */
            m_stateMutex.unlock();
            m_stateMutex.unlock();

            m_underlyingConnection = nullptr;

            for (auto *i : m_continuationCallbackVector)
            {
                Crt::Delete<ContinuationCallbackData>(i, m_allocator);
            }
        }

        bool ConnectionLifecycleHandler::OnErrorCallback(RpcError error)
        {
            (void)error;
            /* Returning true implies that the connection should close as a result of encountering this error. */
            return true;
        }

        void ConnectionLifecycleHandler::OnPingCallback(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload)
        {
            (void)headers;
            (void)payload;
        }

        void ConnectionLifecycleHandler::OnConnectCallback() {}

        void ConnectionLifecycleHandler::OnDisconnectCallback(RpcError error) { (void)error; }

        Crt::String RpcError::StatusToString()
        {
            switch (baseStatus)
            {
                case EVENT_STREAM_RPC_SUCCESS:
                    return "EVENT_STREAM_RPC_SUCCESS";
                case EVENT_STREAM_RPC_NULL_PARAMETER:
                    return "EVENT_STREAM_RPC_NULL_PARAMETER";
                case EVENT_STREAM_RPC_UNINITIALIZED:
                    return "EVENT_STREAM_RPC_UNINITIALIZED";
                case EVENT_STREAM_RPC_ALLOCATION_ERROR:
                    return "EVENT_STREAM_RPC_ALLOCATION_ERROR";
                case EVENT_STREAM_RPC_CONNECTION_SETUP_FAILED:
                    return "EVENT_STREAM_RPC_CONNECTION_SETUP_FAILED";
                case EVENT_STREAM_RPC_CONNECTION_ACCESS_DENIED:
                    return "EVENT_STREAM_RPC_CONNECTION_ACCESS_DENIED";
                case EVENT_STREAM_RPC_CONNECTION_ALREADY_ESTABLISHED:
                    return "EVENT_STREAM_RPC_CONNECTION_ALREADY_ESTABLISHED";
                case EVENT_STREAM_RPC_CONNECTION_CLOSED:
                    return "EVENT_STREAM_RPC_CONNECTION_CLOSED";
                case EVENT_STREAM_RPC_CONTINUATION_CLOSED:
                    return "EVENT_STREAM_RPC_CONTINUATION_CLOSED";
                case EVENT_STREAM_RPC_UNKNOWN_PROTOCOL_MESSAGE:
                    return "EVENT_STREAM_RPC_UNKNOWN_PROTOCOL_MESSAGE";
                case EVENT_STREAM_RPC_UNMAPPED_DATA:
                    return "EVENT_STREAM_RPC_UNMAPPED_DATA";
                case EVENT_STREAM_RPC_UNSUPPORTED_CONTENT_TYPE:
                    return "EVENT_STREAM_RPC_UNSUPPORTED_CONTENT_TYPE";
                case EVENT_STREAM_RPC_CRT_ERROR:
                    Crt::String ret = "Failed with EVENT_STREAM_RPC_CRT_ERROR, the CRT error was ";
                    ret += Crt::ErrorDebugString(crtError);
                    return ret;
            }
            return "Unknown status code";
        }

        std::future<RpcError> ClientConnection::Connect(
            const ConnectionConfig &connectionConfig,
            ConnectionLifecycleHandler *connectionLifecycleHandler,
            Crt::Io::ClientBootstrap &clientBootstrap) noexcept
        {
            EventStreamRpcStatusCode baseError = EVENT_STREAM_RPC_SUCCESS;
            struct aws_event_stream_rpc_client_connection_options connOptions;

            {
                const std::lock_guard<std::recursive_mutex> lock(m_stateMutex);
                if (m_clientState == DISCONNECTED)
                {
                    m_clientState = CONNECTING_SOCKET;
                    m_onConnectCalled = false;
                    m_connectionSetupPromise = {};
                    m_connectAckedPromise = {};
                    m_closedPromise = {};
                    m_closeReason = {EVENT_STREAM_RPC_UNINITIALIZED, 0};
                    m_connectionConfig = connectionConfig;
                    m_lifecycleHandler = connectionLifecycleHandler;
                }
                else
                {
                    baseError = EVENT_STREAM_RPC_CONNECTION_ALREADY_ESTABLISHED;
                }
            }

            m_onConnectRequestCallback = m_connectionConfig.GetConnectRequestCallback();
            Crt::String hostName;

            if (baseError == EVENT_STREAM_RPC_SUCCESS)
            {
                AWS_ZERO_STRUCT(connOptions);
                if (m_connectionConfig.GetHostName().has_value())
                {
                    hostName = m_connectionConfig.GetHostName().value();
                    connOptions.host_name = hostName.c_str();
                }
                else
                {
                    baseError = EVENT_STREAM_RPC_NULL_PARAMETER;
                }
                if (m_connectionConfig.GetPort().has_value())
                {
                    connOptions.port = m_connectionConfig.GetPort().value();
                }
                else
                {
                    baseError = EVENT_STREAM_RPC_NULL_PARAMETER;
                }

                connOptions.bootstrap = clientBootstrap.GetUnderlyingHandle();
            }

            if (baseError)
            {
                std::promise<RpcError> errorPromise;
                errorPromise.set_value({baseError, 0});
                return errorPromise.get_future();
            }

            if (m_connectionConfig.GetSocketOptions().has_value())
            {
                m_socketOptions = m_connectionConfig.GetSocketOptions().value();
            }
            connOptions.socket_options = &m_socketOptions.GetImpl();

            connOptions.on_connection_setup = ClientConnection::s_onConnectionSetup;
            connOptions.on_connection_protocol_message = ClientConnection::s_onProtocolMessage;
            connOptions.on_connection_shutdown = ClientConnection::s_onConnectionShutdown;
            connOptions.user_data = reinterpret_cast<void *>(this);

            m_lifecycleHandler = connectionLifecycleHandler;
            m_connectMessageAmender = m_connectionConfig.GetConnectMessageAmender();

            if (m_connectionConfig.GetTlsConnectionOptions().has_value())
            {
                connOptions.tls_options = m_connectionConfig.GetTlsConnectionOptions()->GetUnderlyingHandle();
            }

            int crtError = aws_event_stream_rpc_client_connection_connect(m_allocator, &connOptions);

            if (crtError)
            {
                std::promise<RpcError> errorPromise;
                AWS_LOGF_ERROR(
                    AWS_LS_EVENT_STREAM_RPC_CLIENT,
                    "A CRT error occurred while attempting to establish the connection: %s",
                    Crt::ErrorDebugString(crtError));
                errorPromise.set_value({EVENT_STREAM_RPC_CRT_ERROR, crtError});
                return errorPromise.get_future();
            }
            else
            {
                const std::lock_guard<std::recursive_mutex> lock(m_stateMutex);
                m_connectionWillSetup = true;
            }

            return m_connectAckedPromise.get_future();
        }

        std::future<RpcError> ClientConnection::SendPing(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return s_sendPing(this, headers, payload, onMessageFlushCallback);
        }

        std::future<RpcError> ClientConnection::SendPingResponse(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return s_sendPingResponse(this, headers, payload, onMessageFlushCallback);
        }

        std::future<RpcError> ClientConnection::s_sendPing(
            ClientConnection *connection,
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return s_sendProtocolMessage(
                connection, headers, payload, AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_PING, 0, onMessageFlushCallback);
        }

        std::future<RpcError> ClientConnection::s_sendPingResponse(
            ClientConnection *connection,
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return s_sendProtocolMessage(
                connection,
                headers,
                payload,
                AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_PING_RESPONSE,
                0,
                onMessageFlushCallback);
        }

        std::future<RpcError> ClientConnection::SendProtocolMessage(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t messageFlags,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            return s_sendProtocolMessage(this, headers, payload, messageType, messageFlags, onMessageFlushCallback);
        }

        void ClientConnection::s_protocolMessageCallback(int errorCode, void *userData) noexcept
        {
            auto *callbackData = static_cast<OnMessageFlushCallbackContainer *>(userData);

            if (errorCode)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_EVENT_STREAM_RPC_CLIENT,
                    "A CRT error occurred while attempting to send a message: %s",
                    Crt::ErrorDebugString(errorCode));
                callbackData->onFlushPromise.set_value({EVENT_STREAM_RPC_CRT_ERROR, errorCode});
            }
            else
            {
                callbackData->onFlushPromise.set_value({EVENT_STREAM_RPC_SUCCESS, 0});
            }

            /* Call the user-provided callback. */
            if (callbackData->onMessageFlushCallback)
            {
                callbackData->onMessageFlushCallback(errorCode);
            }

            Crt::Delete(callbackData, callbackData->allocator);
        }

        std::future<RpcError> ClientConnection::s_sendProtocolMessage(
            ClientConnection *connection,
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t messageFlags,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            std::promise<RpcError> onFlushPromise;
            OnMessageFlushCallbackContainer *callbackContainer = nullptr;
            struct aws_array_list headersArray;

            /* The caller should never pass a NULL connection. */
            AWS_PRECONDITION(connection != nullptr);

            int errorCode = EventStreamCppToNativeCrtBuilder::s_fillNativeHeadersArray(
                headers, &headersArray, connection->m_allocator);

            if (!errorCode)
            {
                struct aws_event_stream_rpc_message_args msg_args;
                msg_args.headers = (struct aws_event_stream_header_value_pair *)headersArray.data;
                msg_args.headers_count = headers.size();
                msg_args.payload = payload.has_value() ? (aws_byte_buf *)(&(payload.value())) : nullptr;
                msg_args.message_type = messageType;
                msg_args.message_flags = messageFlags;

                /* This heap allocation is necessary so that the flush callback can still be invoked when this function
                 * returns. */
                callbackContainer =
                    Crt::New<OnMessageFlushCallbackContainer>(connection->m_allocator, connection->m_allocator);
                callbackContainer->onMessageFlushCallback = onMessageFlushCallback;
                callbackContainer->onFlushPromise = std::move(onFlushPromise);

                errorCode = aws_event_stream_rpc_client_connection_send_protocol_message(
                    connection->m_underlyingConnection,
                    &msg_args,
                    ClientConnection::s_protocolMessageCallback,
                    reinterpret_cast<void *>(callbackContainer));
            }

            /* Cleanup. */
            if (aws_array_list_is_valid(&headersArray))
            {
                aws_array_list_clean_up(&headersArray);
            }

            if (errorCode)
            {
                onFlushPromise = std::move(callbackContainer->onFlushPromise);
                AWS_LOGF_ERROR(
                    AWS_LS_EVENT_STREAM_RPC_CLIENT,
                    "A CRT error occurred while queueing a message to be sent on the connection: %s",
                    Crt::ErrorDebugString(errorCode));
                onFlushPromise.set_value({EVENT_STREAM_RPC_CRT_ERROR, errorCode});
                Crt::Delete(callbackContainer, connection->m_allocator);
            }
            else
            {
                return callbackContainer->onFlushPromise.get_future();
            }

            return onFlushPromise.get_future();
        }

        void ClientConnection::Close() noexcept
        {
            const std::lock_guard<std::recursive_mutex> lock(m_stateMutex);

            if (IsOpen())
            {
                aws_event_stream_rpc_client_connection_close(this->m_underlyingConnection, AWS_OP_SUCCESS);
            }
            else if (m_clientState == CONNECTING_SOCKET && !m_connectionWillSetup)
            {
                m_connectAckedPromise.set_value({EVENT_STREAM_RPC_CONNECTION_CLOSED, 0});
            }

            if (m_clientState != DISCONNECTING && m_clientState != DISCONNECTED)
            {
                m_clientState = DISCONNECTING;
            }

            if (m_closeReason.baseStatus == EVENT_STREAM_RPC_UNINITIALIZED)
            {
                m_closeReason = {EVENT_STREAM_RPC_CONNECTION_CLOSED, 0};
            }
        }

        EventStreamHeader::EventStreamHeader(
            const struct aws_event_stream_header_value_pair &header,
            Crt::Allocator *allocator)
            : m_allocator(allocator), m_valueByteBuf({}), m_underlyingHandle(header)
        {
        }

        EventStreamHeader::EventStreamHeader(
            const Crt::String &name,
            const Crt::String &value,
            Crt::Allocator *allocator) noexcept
            : m_allocator(allocator),
              m_valueByteBuf(Crt::ByteBufNewCopy(allocator, (uint8_t *)value.c_str(), value.length()))
        {
            m_underlyingHandle.header_name_len = static_cast<uint8_t>(name.length());
            size_t length;
            if (name.length() > INT8_MAX)
            {
                length = INT8_MAX;
            }
            else
            {
                length = static_cast<size_t>(name.length());
            }
            (void)memcpy(m_underlyingHandle.header_name, name.c_str(), length);
            m_underlyingHandle.header_value_type = AWS_EVENT_STREAM_HEADER_STRING;
            m_underlyingHandle.header_value.variable_len_val = m_valueByteBuf.buffer;
            m_underlyingHandle.header_value_len = (uint16_t)m_valueByteBuf.len;
        }

        EventStreamHeader::~EventStreamHeader() noexcept
        {
            if (aws_byte_buf_is_valid(&m_valueByteBuf))
                Crt::ByteBufDelete(m_valueByteBuf);
        }

        EventStreamHeader::EventStreamHeader(const EventStreamHeader &lhs) noexcept
            : m_allocator(lhs.m_allocator),
              m_valueByteBuf(Crt::ByteBufNewCopy(lhs.m_allocator, lhs.m_valueByteBuf.buffer, lhs.m_valueByteBuf.len)),
              m_underlyingHandle(lhs.m_underlyingHandle)
        {
            m_underlyingHandle.header_value.variable_len_val = m_valueByteBuf.buffer;
            m_underlyingHandle.header_value_len = static_cast<uint16_t>(m_valueByteBuf.len);
        }

        EventStreamHeader &EventStreamHeader::operator=(const EventStreamHeader &lhs) noexcept
        {
            m_allocator = lhs.m_allocator;
            m_valueByteBuf = Crt::ByteBufNewCopy(lhs.m_allocator, lhs.m_valueByteBuf.buffer, lhs.m_valueByteBuf.len);
            m_underlyingHandle = lhs.m_underlyingHandle;
            m_underlyingHandle.header_value.variable_len_val = m_valueByteBuf.buffer;
            m_underlyingHandle.header_value_len = static_cast<uint16_t>(m_valueByteBuf.len);
            return *this;
        }

        EventStreamHeader::EventStreamHeader(EventStreamHeader &&rhs) noexcept
            : m_allocator(rhs.m_allocator), m_valueByteBuf(rhs.m_valueByteBuf),
              m_underlyingHandle(rhs.m_underlyingHandle)
        {
            rhs.m_valueByteBuf.allocator = nullptr;
            rhs.m_valueByteBuf.buffer = nullptr;
        }

        const struct aws_event_stream_header_value_pair *EventStreamHeader::GetUnderlyingHandle() const
        {
            return &m_underlyingHandle;
        }

        Crt::String EventStreamHeader::GetHeaderName() const noexcept
        {
            return Crt::String(m_underlyingHandle.header_name, m_underlyingHandle.header_name_len, m_allocator);
        }

        bool EventStreamHeader::GetValueAsString(Crt::String &value) const noexcept
        {
            if (m_underlyingHandle.header_value_type != AWS_EVENT_STREAM_HEADER_STRING)
            {
                return false;
            }
            value = Crt::String(
                reinterpret_cast<char *>(m_underlyingHandle.header_value.variable_len_val),
                m_underlyingHandle.header_value_len,
                m_allocator);

            return true;
        }

        ClientContinuation ClientConnection::NewStream(ClientContinuationHandler &clientContinuationHandler) noexcept
        {
            return ClientContinuation(this, clientContinuationHandler, m_allocator);
        }

        void ClientConnection::s_onConnectionSetup(
            struct aws_event_stream_rpc_client_connection *connection,
            int errorCode,
            void *userData) noexcept
        {
            /* The `userData` pointer is used to pass `this` of a `ClientConnection` object. */
            auto *thisConnection = static_cast<ClientConnection *>(userData);

            const std::lock_guard<std::recursive_mutex> lock(thisConnection->m_stateMutex);

            if (errorCode)
            {
                thisConnection->m_clientState = DISCONNECTED;
                AWS_LOGF_ERROR(
                    AWS_LS_EVENT_STREAM_RPC_CLIENT,
                    "A CRT error occurred while setting up the connection: %s",
                    Crt::ErrorDebugString(errorCode));
                thisConnection->m_connectAckedPromise.set_value({EVENT_STREAM_RPC_CRT_ERROR, errorCode});
                aws_event_stream_rpc_client_connection_release(connection);
                thisConnection->m_underlyingConnection = nullptr;
                /* No connection to close on error, so no need to check return value of the callback. */
                (void)thisConnection->m_lifecycleHandler->OnErrorCallback({EVENT_STREAM_RPC_CRT_ERROR, errorCode});
            }
            else if (thisConnection->m_clientState == DISCONNECTING || thisConnection->m_clientState == DISCONNECTED)
            {
                thisConnection->m_underlyingConnection = connection;
                thisConnection->m_closeReason = {EVENT_STREAM_RPC_CONNECTION_CLOSED, 0};
                thisConnection->Close();
            }
            else
            {
                thisConnection->m_clientState = WAITING_FOR_CONNECT_ACK;
                thisConnection->m_underlyingConnection = connection;
                MessageAmendment messageAmendment;
                Crt::List<EventStreamHeader> messageAmendmentHeaders = messageAmendment.GetHeaders();

                if (thisConnection->m_connectMessageAmender)
                {
                    MessageAmendment connectAmendment(thisConnection->m_connectMessageAmender());
                    Crt::List<EventStreamHeader> amenderHeaderList = connectAmendment.GetHeaders();
                    /* The version header is necessary for establishing the connection. */
                    messageAmendment.AddHeader(EventStreamHeader(
                        Crt::String(EVENTSTREAM_VERSION_HEADER),
                        Crt::String(EVENTSTREAM_VERSION_STRING),
                        thisConnection->m_allocator));
                    /* Note that we are prepending headers from the user-provided amender. */
                    if (amenderHeaderList.size() > 0)
                    {
                        messageAmendmentHeaders.splice(messageAmendmentHeaders.end(), amenderHeaderList);
                    }
                    messageAmendment.SetPayload(connectAmendment.GetPayload());
                }

                /* Send a CONNECT packet to the server. */
                s_sendProtocolMessage(
                    thisConnection,
                    messageAmendment.GetHeaders(),
                    messageAmendment.GetPayload(),
                    AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_CONNECT,
                    0U,
                    thisConnection->m_onConnectRequestCallback);
            }

            thisConnection->m_connectionSetupPromise.set_value();
        }

        void MessageAmendment::AddHeader(EventStreamHeader &&header) noexcept { m_headers.emplace_back(header); }

        void ClientConnection::s_onConnectionShutdown(
            struct aws_event_stream_rpc_client_connection *connection,
            int errorCode,
            void *userData) noexcept
        {
            (void)connection;
            /* The `userData` pointer is used to pass `this` of a `ClientConnection` object. */
            auto *thisConnection = static_cast<ClientConnection *>(userData);

            const std::lock_guard<std::recursive_mutex> lock(thisConnection->m_stateMutex);

            if (thisConnection->m_closeReason.baseStatus == EVENT_STREAM_RPC_UNINITIALIZED && errorCode)
            {
                thisConnection->m_closeReason = {EVENT_STREAM_RPC_CRT_ERROR, errorCode};
            }

            thisConnection->m_underlyingConnection = nullptr;

            if (thisConnection->m_closeReason.baseStatus != EVENT_STREAM_RPC_UNINITIALIZED &&
                !thisConnection->m_onConnectCalled)
            {
                thisConnection->m_connectAckedPromise.set_value(thisConnection->m_closeReason);
            }

            thisConnection->m_clientState = DISCONNECTED;

            if (thisConnection->m_onConnectCalled)
            {
                if (errorCode)
                {
                    thisConnection->m_lifecycleHandler->OnDisconnectCallback({EVENT_STREAM_RPC_CRT_ERROR, errorCode});
                }
                else
                {
                    thisConnection->m_lifecycleHandler->OnDisconnectCallback({EVENT_STREAM_RPC_SUCCESS, 0});
                }
                thisConnection->m_onConnectCalled = false;
            }

            if (errorCode)
            {
                AWS_LOGF_ERROR(
                    AWS_LS_EVENT_STREAM_RPC_CLIENT,
                    "A CRT error occurred while shutting down the connection: %s",
                    Crt::ErrorDebugString(errorCode));
                thisConnection->m_closedPromise.set_value({EVENT_STREAM_RPC_CRT_ERROR, errorCode});
            }
            else
            {
                thisConnection->m_closedPromise.set_value({EVENT_STREAM_RPC_SUCCESS, errorCode});
            }
        }

        void ClientConnection::s_onProtocolMessage(
            struct aws_event_stream_rpc_client_connection *connection,
            const struct aws_event_stream_rpc_message_args *messageArgs,
            void *userData) noexcept
        {
            AWS_PRECONDITION(messageArgs != nullptr);
            (void)connection;

            /* The `userData` pointer is used to pass `this` of a `ClientConnection` object. */
            auto *thisConnection = static_cast<ClientConnection *>(userData);
            Crt::List<EventStreamHeader> pingHeaders;

            switch (messageArgs->message_type)
            {
                case AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_CONNECT_ACK:
                    thisConnection->m_stateMutex.lock();
                    if (thisConnection->m_clientState == WAITING_FOR_CONNECT_ACK)
                    {
                        if (messageArgs->message_flags & AWS_EVENT_STREAM_RPC_MESSAGE_FLAG_CONNECTION_ACCEPTED)
                        {
                            thisConnection->m_clientState = CONNECTED;
                            thisConnection->m_onConnectCalled = true;
                            thisConnection->m_connectAckedPromise.set_value({EVENT_STREAM_RPC_SUCCESS, 0});
                            thisConnection->m_lifecycleHandler->OnConnectCallback();
                        }
                        else
                        {
                            thisConnection->m_closeReason = {EVENT_STREAM_RPC_CONNECTION_ACCESS_DENIED, 0};
                            thisConnection->Close();
                        }
                    }
                    else
                    {
                        /* Unexpected CONNECT_ACK received. */
                    }
                    thisConnection->m_stateMutex.unlock();

                    break;

                case AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_PING:

                    for (size_t i = 0; i < messageArgs->headers_count; ++i)
                    {
                        pingHeaders.emplace_back(
                            EventStreamHeader(messageArgs->headers[i], thisConnection->m_allocator));
                    }

                    if (messageArgs->payload)
                    {
                        thisConnection->m_lifecycleHandler->OnPingCallback(pingHeaders, *messageArgs->payload);
                    }
                    else
                    {
                        thisConnection->m_lifecycleHandler->OnPingCallback(pingHeaders, Crt::Optional<Crt::ByteBuf>());
                    }

                    break;

                case AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_PING_RESPONSE:
                    return;
                    break;

                case AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_PROTOCOL_ERROR:
                case AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_INTERNAL_ERROR:

                    if (thisConnection->m_lifecycleHandler->OnErrorCallback(
                            {EVENT_STREAM_RPC_CRT_ERROR, AWS_ERROR_EVENT_STREAM_RPC_PROTOCOL_ERROR}))
                    {
                        thisConnection->Close();
                    }

                    break;

                default:
                    return;

                    if (thisConnection->m_lifecycleHandler->OnErrorCallback(
                            {EVENT_STREAM_RPC_UNKNOWN_PROTOCOL_MESSAGE, 0}))
                    {
                        thisConnection->Close();
                    }

                    break;
            }
        }

        void AbstractShapeBase::s_customDeleter(AbstractShapeBase *shape) noexcept
        {
            if (shape->m_allocator != nullptr)
                Crt::Delete<AbstractShapeBase>(shape, shape->m_allocator);
        }

        ClientContinuation::ClientContinuation(
            ClientConnection *connection,
            ClientContinuationHandler &continuationHandler,
            Crt::Allocator *allocator) noexcept
            : m_allocator(allocator), m_continuationHandler(continuationHandler), m_continuationToken(nullptr)
        {
            struct aws_event_stream_rpc_client_stream_continuation_options options;
            options.on_continuation = ClientContinuation::s_onContinuationMessage;
            options.on_continuation_closed = ClientContinuation::s_onContinuationClosed;

            const std::lock_guard<std::mutex> lock(connection->m_continuationVectorMutex);
            ContinuationCallbackData *callbackData = Crt::New<ContinuationCallbackData>(m_allocator, this, m_allocator);
            connection->m_continuationCallbackVector.emplace_back(callbackData);

            m_continuationHandler.m_callbackData = callbackData;
            options.user_data = reinterpret_cast<void *>(callbackData);

            if (connection->IsOpen())
            {
                m_continuationToken =
                    aws_event_stream_rpc_client_connection_new_stream(connection->m_underlyingConnection, &options);
                if (m_continuationToken == nullptr)
                {
                    Crt::Delete<ContinuationCallbackData>(callbackData, m_allocator);
                    connection->m_continuationCallbackVector.pop_back();
                    m_continuationHandler.m_callbackData = nullptr;
                }
            }
        }

        ClientContinuation::~ClientContinuation() noexcept
        {
            if (m_continuationToken)
            {
                aws_event_stream_rpc_client_continuation_release(m_continuationToken);
                m_continuationToken = nullptr;
            }
        }

        ClientContinuationHandler::~ClientContinuationHandler() noexcept
        {
            if (m_callbackData)
            {
                const std::lock_guard<std::mutex> lock(m_callbackData->callbackMutex);
                m_callbackData->continuationDestroyed = true;
            }
        }

        void ClientContinuation::s_onContinuationMessage(
            struct aws_event_stream_rpc_client_continuation_token *continuationToken,
            const struct aws_event_stream_rpc_message_args *messageArgs,
            void *userData) noexcept
        {
            (void)continuationToken;
            /* The `userData` pointer is used to pass a `ContinuationCallbackData` object. */
            auto *callbackData = static_cast<ContinuationCallbackData *>(userData);
            auto *thisContinuation = callbackData->clientContinuation;

            Crt::List<EventStreamHeader> continuationMessageHeaders;
            for (size_t i = 0; i < messageArgs->headers_count; ++i)
            {
                continuationMessageHeaders.emplace_back(
                    EventStreamHeader(messageArgs->headers[i], thisContinuation->m_allocator));
            }

            Crt::Optional<Crt::ByteBuf> payload;

            if (messageArgs->payload)
            {
                payload = Crt::Optional<Crt::ByteBuf>(*messageArgs->payload);
            }
            else
            {
                payload = Crt::Optional<Crt::ByteBuf>();
            }

            const std::lock_guard<std::mutex> lock(callbackData->callbackMutex);
            if (callbackData->continuationDestroyed)
                return;
            thisContinuation->m_continuationHandler.OnContinuationMessage(
                continuationMessageHeaders, payload, messageArgs->message_type, messageArgs->message_flags);
        }

        void ClientContinuation::s_onContinuationClosed(
            struct aws_event_stream_rpc_client_continuation_token *continuationToken,
            void *userData) noexcept
        {
            (void)continuationToken;

            /* The `userData` pointer is used to pass a `ContinuationCallbackData` object. */
            auto *callbackData = static_cast<ContinuationCallbackData *>(userData);

            const std::lock_guard<std::mutex> lock(callbackData->callbackMutex);
            if (callbackData->continuationDestroyed)
                return;

            auto *thisContinuation = callbackData->clientContinuation;
            thisContinuation->m_continuationHandler.OnContinuationClosed();
        }

        std::future<RpcError> ClientContinuation::Activate(
            const Crt::String &operationName,
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t messageFlags,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            struct aws_array_list headersArray;
            OnMessageFlushCallbackContainer *callbackContainer = nullptr;
            std::promise<RpcError> onFlushPromise;

            if (m_continuationToken == nullptr)
            {
                onFlushPromise.set_value({EVENT_STREAM_RPC_CONNECTION_CLOSED, 0});
                return onFlushPromise.get_future();
            }

            if (IsClosed())
            {
                onFlushPromise.set_value({EVENT_STREAM_RPC_CONTINUATION_CLOSED, 0});
                return onFlushPromise.get_future();
            }

            int errorCode =
                EventStreamCppToNativeCrtBuilder::s_fillNativeHeadersArray(headers, &headersArray, m_allocator);

            if (!errorCode)
            {
                struct aws_event_stream_rpc_message_args msg_args;
                msg_args.headers = (struct aws_event_stream_header_value_pair *)headersArray.data;
                msg_args.headers_count = headers.size();
                msg_args.payload = payload.has_value() ? (aws_byte_buf *)(&(payload.value())) : nullptr;
                msg_args.message_type = messageType;
                msg_args.message_flags = messageFlags;

                /* This heap allocation is necessary so that the flush callback can still be invoked when this function
                 * returns. */
                callbackContainer = Crt::New<OnMessageFlushCallbackContainer>(m_allocator, m_allocator);
                callbackContainer->onMessageFlushCallback = onMessageFlushCallback;
                callbackContainer->onFlushPromise = std::move(onFlushPromise);

                errorCode = aws_event_stream_rpc_client_continuation_activate(
                    m_continuationToken,
                    Crt::ByteCursorFromCString(operationName.c_str()),
                    &msg_args,
                    ClientConnection::s_protocolMessageCallback,
                    reinterpret_cast<void *>(callbackContainer));
            }

            /* Cleanup. */
            if (aws_array_list_is_valid(&headersArray))
            {
                aws_array_list_clean_up(&headersArray);
            }

            if (errorCode)
            {
                onFlushPromise = std::move(callbackContainer->onFlushPromise);
                onFlushPromise.set_value({EVENT_STREAM_RPC_CRT_ERROR, errorCode});
                Crt::Delete(callbackContainer, m_allocator);
            }
            else
            {
                return callbackContainer->onFlushPromise.get_future();
            }

            return onFlushPromise.get_future();
        }

        std::future<RpcError> ClientContinuation::SendMessage(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t messageFlags,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            struct aws_array_list headersArray;
            OnMessageFlushCallbackContainer *callbackContainer = nullptr;
            std::promise<RpcError> onFlushPromise;

            if (IsClosed())
            {
                onFlushPromise.set_value({EVENT_STREAM_RPC_CONTINUATION_CLOSED, 0});
                return onFlushPromise.get_future();
            }

            int errorCode =
                EventStreamCppToNativeCrtBuilder::s_fillNativeHeadersArray(headers, &headersArray, m_allocator);

            if (!errorCode)
            {
                struct aws_event_stream_rpc_message_args msg_args;
                msg_args.headers = (struct aws_event_stream_header_value_pair *)headersArray.data;
                msg_args.headers_count = headers.size();
                msg_args.payload = payload.has_value() ? (aws_byte_buf *)(&(payload.value())) : nullptr;
                msg_args.message_type = messageType;
                msg_args.message_flags = messageFlags;

                /* This heap allocation is necessary so that the flush callback can still be invoked when this function
                 * returns. */
                callbackContainer = Crt::New<OnMessageFlushCallbackContainer>(m_allocator, m_allocator);
                callbackContainer->onMessageFlushCallback = onMessageFlushCallback;
                callbackContainer->onFlushPromise = std::move(onFlushPromise);

                if (m_continuationToken)
                {
                    errorCode = aws_event_stream_rpc_client_continuation_send_message(
                        m_continuationToken,
                        &msg_args,
                        ClientConnection::s_protocolMessageCallback,
                        reinterpret_cast<void *>(callbackContainer));
                }
            }

            /* Cleanup. */
            if (aws_array_list_is_valid(&headersArray))
            {
                aws_array_list_clean_up(&headersArray);
            }

            if (errorCode)
            {
                onFlushPromise = std::move(callbackContainer->onFlushPromise);
                AWS_LOGF_ERROR(
                    AWS_LS_EVENT_STREAM_RPC_CLIENT,
                    "A CRT error occurred while queueing a message to be sent on a stream: %s",
                    Crt::ErrorDebugString(errorCode));
                onFlushPromise.set_value({EVENT_STREAM_RPC_CRT_ERROR, errorCode});
                Crt::Delete(callbackContainer, m_allocator);
            }
            else
            {
                return callbackContainer->onFlushPromise.get_future();
            }

            return onFlushPromise.get_future();
        }

        bool ClientContinuation::IsClosed() noexcept
        {
            if (!m_continuationToken)
            {
                return true;
            }
            else
            {
                return aws_event_stream_rpc_client_continuation_is_closed(m_continuationToken);
            }
        }

        OperationModelContext::OperationModelContext(const ServiceModel &serviceModel) noexcept
            : m_serviceModel(serviceModel)
        {
        }

        OperationError::OperationError() noexcept {}

        void OperationError::SerializeToJsonObject(Crt::JsonObject &payloadObject) const { (void)payloadObject; }

        AbstractShapeBase::AbstractShapeBase() noexcept : m_allocator(nullptr) {}

        AbstractShapeBase::~AbstractShapeBase() noexcept {}

        ClientOperation::ClientOperation(
            ClientConnection &connection,
            StreamResponseHandler *streamHandler,
            const OperationModelContext &operationModelContext,
            Crt::Allocator *allocator) noexcept
            : m_operationModelContext(operationModelContext), m_messageCount(0), m_allocator(allocator),
              m_streamHandler(streamHandler), m_clientContinuation(connection.NewStream(*this)), m_expectedCloses(0),
              m_streamClosedCalled(false)
        {
        }

        ClientOperation::ClientOperation(ClientOperation &&rhs) noexcept
            : m_operationModelContext(rhs.m_operationModelContext), m_messageCount(std::move(rhs.m_messageCount)),
              m_allocator(std::move(rhs.m_allocator)), m_streamHandler(rhs.m_streamHandler),
              m_clientContinuation(std::move(rhs.m_clientContinuation)),
              m_initialResponsePromise(std::move(rhs.m_initialResponsePromise))
        {
        }

        ClientOperation::~ClientOperation() noexcept
        {
            Close().wait();
            std::unique_lock<std::mutex> lock(m_continuationMutex);
            m_closeReady.wait(lock, [this] { return m_expectedCloses.load() == 0; });
        }

        TaggedResult::TaggedResult(Crt::ScopedResource<AbstractShapeBase> operationResponse) noexcept
            : m_responseType(OPERATION_RESPONSE)
        {
            m_operationResult.m_response = std::move(operationResponse);
        }

        TaggedResult::~TaggedResult() noexcept
        {
            if (m_responseType == OPERATION_RESPONSE)
            {
                m_operationResult.m_response.~unique_ptr();
            }
            else if (m_responseType == OPERATION_ERROR)
            {
                m_operationResult.m_error.~unique_ptr();
            }
        }

        TaggedResult::TaggedResult(Crt::ScopedResource<OperationError> operationError) noexcept
            : m_responseType(OPERATION_ERROR), m_rpcError({EVENT_STREAM_RPC_UNINITIALIZED, 0})
        {
            m_operationResult.m_error = std::move(operationError);
        }

        TaggedResult &TaggedResult::operator=(TaggedResult &&rhs) noexcept
        {
            m_responseType = rhs.m_responseType;
            if (m_responseType == OPERATION_RESPONSE)
            {
                m_operationResult.m_response = std::move(rhs.m_operationResult.m_response);
            }
            else if (m_responseType == OPERATION_ERROR)
            {
                m_operationResult.m_error = std::move(rhs.m_operationResult.m_error);
            }
            m_rpcError = rhs.m_rpcError;
            rhs.m_rpcError = {EVENT_STREAM_RPC_UNINITIALIZED, 0};

            return *this;
        }

        TaggedResult::TaggedResult(RpcError rpcError) noexcept
            : m_responseType(RPC_ERROR), m_operationResult(), m_rpcError(rpcError)
        {
        }

        TaggedResult::TaggedResult() noexcept
            : m_responseType(RPC_ERROR), m_operationResult(), m_rpcError({EVENT_STREAM_RPC_UNINITIALIZED, 0})
        {
        }

        TaggedResult::TaggedResult(TaggedResult &&rhs) noexcept
        {
            m_responseType = rhs.m_responseType;
            if (m_responseType == OPERATION_RESPONSE)
            {
                m_operationResult.m_response = std::move(rhs.m_operationResult.m_response);
            }
            else if (m_responseType == OPERATION_ERROR)
            {
                m_operationResult.m_error = std::move(rhs.m_operationResult.m_error);
            }
            m_rpcError = rhs.m_rpcError;
            rhs.m_rpcError = {EVENT_STREAM_RPC_UNINITIALIZED, 0};
        }

        TaggedResult::operator bool() const noexcept { return m_responseType == OPERATION_RESPONSE; }

        AbstractShapeBase *TaggedResult::GetOperationResponse() const noexcept
        {
            return (m_responseType == OPERATION_RESPONSE) ? m_operationResult.m_response.get() : nullptr;
        }

        OperationError *TaggedResult::GetOperationError() const noexcept
        {
            return (m_responseType == OPERATION_ERROR) ? m_operationResult.m_error.get() : nullptr;
        }

        RpcError TaggedResult::GetRpcError() const noexcept
        {
            if (m_responseType == RPC_ERROR)
            {
                return m_rpcError;
            }
            else
            {
                /* Assume success since an application response or error was received. */
                return {EVENT_STREAM_RPC_SUCCESS, 0};
            }
        }

        std::future<TaggedResult> ClientOperation::GetOperationResult() noexcept
        {
            {
                const std::lock_guard<std::mutex> lock(m_continuationMutex);

                if (m_clientContinuation.IsClosed() && !m_resultReceived)
                {
                    AWS_LOGF_ERROR(AWS_LS_EVENT_STREAM_RPC_CLIENT, "The underlying stream is already closed.");
                    m_initialResponsePromise.set_value(TaggedResult({EVENT_STREAM_RPC_CONNECTION_CLOSED, 0}));
                    m_resultReceived = true;
                }
            }

            return m_initialResponsePromise.get_future();
        }

        const EventStreamHeader *ClientOperation::GetHeaderByName(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::String &name) noexcept
        {
            for (auto it = headers.begin(); it != headers.end(); ++it)
            {
                if (name == it->GetHeaderName())
                {
                    return &(*it);
                }
            }
            return nullptr;
        }

        EventStreamRpcStatusCode ClientOperation::HandleData(const Crt::Optional<Crt::ByteBuf> &payload)
        {
            Crt::StringView payloadStringView;
            if (payload.has_value())
            {
                payloadStringView = Crt::ByteCursorToStringView(Crt::ByteCursorFromByteBuf(payload.value()));
            }

            /* The value of this hashmap contains the function that allocates the response object from the
             * payload. */
            /* Responses after the first message don't necessarily have the same shape as the first. */
            Crt::ScopedResource<AbstractShapeBase> response;
            if (m_messageCount == 1)
            {
                response = m_operationModelContext.AllocateInitialResponseFromPayload(payloadStringView, m_allocator);
            }
            else
            {
                response = m_operationModelContext.AllocateStreamingResponseFromPayload(payloadStringView, m_allocator);
            }

            if (response.get() == nullptr)
            {
                AWS_LOGF_ERROR(AWS_LS_EVENT_STREAM_RPC_CLIENT, "Failed to allocate a response from the payload.");
                return EVENT_STREAM_RPC_ALLOCATION_ERROR;
            }

            if (m_messageCount == 1)
            {
                const std::lock_guard<std::mutex> lock(m_continuationMutex);
                m_resultReceived = true;
                m_initialResponsePromise.set_value(TaggedResult(std::move(response)));
            }
            else
            {
                if (m_streamHandler)
                    m_streamHandler->OnStreamEvent(std::move(response));
            }

            return EVENT_STREAM_RPC_SUCCESS;
        }

        EventStreamRpcStatusCode ClientOperation::HandleError(
            const Crt::String &modelName,
            const Crt::Optional<Crt::ByteBuf> &payload,
            uint32_t messageFlags)
        {
            bool streamAlreadyTerminated = messageFlags & AWS_EVENT_STREAM_RPC_MESSAGE_FLAG_TERMINATE_STREAM;

            Crt::StringView payloadStringView;
            if (payload.has_value())
            {
                payloadStringView = Crt::ByteCursorToStringView(Crt::ByteCursorFromByteBuf(payload.value()));
            }

            /* The value of this hashmap contains the function that allocates the error from the
             * payload. */
            Crt::ScopedResource<OperationError> error =
                m_operationModelContext.AllocateOperationErrorFromPayload(modelName, payloadStringView, m_allocator);
            if (error.get() == nullptr)
                return EVENT_STREAM_RPC_UNMAPPED_DATA;
            if (error->GetMessage().has_value())
            {
                AWS_LOGF_ERROR(
                    AWS_LS_EVENT_STREAM_RPC_CLIENT,
                    "An error was received from the server: %s",
                    error->GetMessage().value().c_str());
            }
            TaggedResult taggedResult(std::move(error));
            if (m_messageCount == 1)
            {
                {
                    const std::lock_guard<std::mutex> lock(m_continuationMutex);
                    m_resultReceived = true;
                    m_initialResponsePromise.set_value(std::move(taggedResult));
                }
                /* Close the stream unless the server already closed it for us. This condition is checked
                 * so that TERMINATE_STREAM messages aren't resent by the client. */
                if (!streamAlreadyTerminated && !m_clientContinuation.IsClosed())
                {
                    Close().wait();
                }
            }
            else
            {
                bool shouldCloseNow = true;
                if (m_streamHandler)
                    shouldCloseNow = m_streamHandler->OnStreamError(std::move(error), {EVENT_STREAM_RPC_SUCCESS, 0});
                if (!streamAlreadyTerminated && shouldCloseNow && !m_clientContinuation.IsClosed())
                {
                    Close().wait();
                }
            }

            return EVENT_STREAM_RPC_SUCCESS;
        }

        bool StreamResponseHandler::OnStreamError(Crt::ScopedResource<OperationError> operationError, RpcError rpcError)
        {
            (void)operationError;
            (void)rpcError;
            /* Note: Always returning true implies that the stream should close
             * as a result of encountering this error. */
            return true;
        }

        void StreamResponseHandler::OnStreamEvent(Crt::ScopedResource<AbstractShapeBase> response) {}

        void StreamResponseHandler::OnStreamClosed() {}

        void ClientOperation::OnContinuationMessage(
            const Crt::List<EventStreamHeader> &headers,
            const Crt::Optional<Crt::ByteBuf> &payload,
            MessageType messageType,
            uint32_t messageFlags)
        {
            EventStreamRpcStatusCode errorCode = EVENT_STREAM_RPC_SUCCESS;
            const EventStreamHeader *modelHeader = nullptr;
            const EventStreamHeader *contentHeader = nullptr;
            Crt::String modelName;

            if (messageFlags & AWS_EVENT_STREAM_RPC_MESSAGE_FLAG_TERMINATE_STREAM)
            {
                const std::lock_guard<std::mutex> lock(m_continuationMutex);
                m_expectedCloses.fetch_add(1);
            }

            m_messageCount += 1;

            modelHeader = GetHeaderByName(headers, Crt::String(SERVICE_MODEL_TYPE_HEADER));
            if (modelHeader == nullptr)
            {
                /* Missing required service model type header. */
                AWS_LOGF_ERROR(
                    AWS_LS_EVENT_STREAM_RPC_CLIENT,
                    "A required header (%s) could not be found in the message.",
                    SERVICE_MODEL_TYPE_HEADER);
                errorCode = EVENT_STREAM_RPC_UNMAPPED_DATA;
            }

            /* Verify that the model name matches. */
            if (!errorCode)
            {
                modelHeader->GetValueAsString(modelName);
                if (messageType == AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_APPLICATION_MESSAGE)
                {
                    if (m_messageCount == 1 && m_operationModelContext.GetInitialResponseModelName() != modelName)
                    {
                        AWS_LOGF_ERROR(
                            AWS_LS_EVENT_STREAM_RPC_CLIENT,
                            "The model name of the initial response did not match its expected model name.");
                        errorCode = EVENT_STREAM_RPC_UNMAPPED_DATA;
                    }
                    else if (
                        m_messageCount > 1 && m_operationModelContext.GetStreamingResponseModelName().has_value() &&
                        m_operationModelContext.GetStreamingResponseModelName().value() != modelName)
                    {
                        AWS_LOGF_ERROR(
                            AWS_LS_EVENT_STREAM_RPC_CLIENT,
                            "The model name of a subsequent response did not match its expected model name.");
                        errorCode = EVENT_STREAM_RPC_UNMAPPED_DATA;
                    }
                }
            }

            if (!errorCode)
            {
                Crt::String contentType;
                contentHeader = GetHeaderByName(headers, Crt::String(CONTENT_TYPE_HEADER));
                if (contentHeader == nullptr)
                {
                    /* Missing required content type header. */
                    AWS_LOGF_ERROR(
                        AWS_LS_EVENT_STREAM_RPC_CLIENT,
                        "A required header (%s) could not be found in the message.",
                        CONTENT_TYPE_HEADER);
                    errorCode = EVENT_STREAM_RPC_UNSUPPORTED_CONTENT_TYPE;
                }
                else if (contentHeader->GetValueAsString(contentType) && contentType != CONTENT_TYPE_APPLICATION_JSON)
                {
                    /* Missing required content type header. */
                    AWS_LOGF_ERROR(
                        AWS_LS_EVENT_STREAM_RPC_CLIENT,
                        "The content type (%s) header was specified with an unsupported value (%s).",
                        CONTENT_TYPE_HEADER,
                        contentType.c_str());
                    errorCode = EVENT_STREAM_RPC_UNSUPPORTED_CONTENT_TYPE;
                }
            }

            if (!errorCode)
            {
                if (messageType == AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_APPLICATION_MESSAGE)
                {
                    errorCode = HandleData(payload);
                }
                else
                {
                    errorCode = HandleError(modelName, payload, messageFlags);
                }
            }

            if (errorCode)
            {
                if (m_messageCount == 1)
                {
                    const std::lock_guard<std::mutex> lock(m_continuationMutex);
                    m_resultReceived = true;
                    RpcError promiseValue = {(EventStreamRpcStatusCode)errorCode, 0};
                    m_initialResponsePromise.set_value(TaggedResult(promiseValue));
                }
                else
                {
                    bool shouldClose = true;
                    if (m_streamHandler)
                        shouldClose = m_streamHandler->OnStreamError(nullptr, {errorCode, 0});
                    if (!m_clientContinuation.IsClosed() && shouldClose)
                    {
                        Close().wait();
                    }
                }
            }
        }

        std::future<RpcError> ClientOperation::Activate(
            const AbstractShapeBase *shape,
            OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            /* Promises must be reset in case the client would like to send a subsequent request with the same
             * `ClientOperation`. */
            m_initialResponsePromise = {};
            {
                const std::lock_guard<std::mutex> lock(m_continuationMutex);
                m_resultReceived = false;
            }

            Crt::List<EventStreamHeader> headers;
            headers.emplace_back(EventStreamHeader(
                Crt::String(CONTENT_TYPE_HEADER), Crt::String(CONTENT_TYPE_APPLICATION_JSON), m_allocator));
            headers.emplace_back(
                EventStreamHeader(Crt::String(SERVICE_MODEL_TYPE_HEADER), GetModelName(), m_allocator));
            Crt::JsonObject payloadObject;
            shape->SerializeToJsonObject(payloadObject);
            Crt::String payloadString = payloadObject.View().WriteCompact();
            return m_clientContinuation.Activate(
                GetModelName(),
                headers,
                Crt::ByteBufFromCString(payloadString.c_str()),
                AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_APPLICATION_MESSAGE,
                0,
                onMessageFlushCallback);
        }

        void ClientOperation::OnContinuationClosed()
        {
            const std::lock_guard<std::mutex> lock(m_continuationMutex);
            if (!m_resultReceived)
            {
                m_initialResponsePromise.set_value(TaggedResult({EVENT_STREAM_RPC_CONTINUATION_CLOSED, 0}));
                m_resultReceived = true;
            }

            if (m_expectedCloses.load() > 0)
            {
                m_expectedCloses.fetch_sub(1);
                if (!m_streamClosedCalled.load() && m_streamHandler)
                {
                    m_streamHandler->OnStreamClosed();
                    m_streamClosedCalled.store(true);
                }
                m_closeReady.notify_one();
            }
        }

        std::future<RpcError> ClientOperation::Close(OnMessageFlushCallback onMessageFlushCallback) noexcept
        {
            const std::lock_guard<std::mutex> lock(m_continuationMutex);
            if (m_expectedCloses.load() > 0 || m_clientContinuation.IsClosed())
            {
                std::promise<RpcError> errorPromise;
                errorPromise.set_value({EVENT_STREAM_RPC_CONTINUATION_CLOSED, 0});
                return errorPromise.get_future();
            }
            else
            {
                std::promise<RpcError> onTerminatePromise;

                int errorCode = AWS_OP_ERR;
                struct aws_event_stream_rpc_message_args msg_args;
                msg_args.headers = nullptr;
                msg_args.headers_count = 0;
                msg_args.payload = nullptr;
                msg_args.message_type = AWS_EVENT_STREAM_RPC_MESSAGE_TYPE_APPLICATION_MESSAGE;
                msg_args.message_flags = AWS_EVENT_STREAM_RPC_MESSAGE_FLAG_TERMINATE_STREAM;

                /* This heap allocation is necessary so that the flush callback can still be invoked when this function
                 * returns. */
                OnMessageFlushCallbackContainer *callbackContainer =
                    Crt::New<OnMessageFlushCallbackContainer>(m_allocator, m_allocator);
                callbackContainer->onMessageFlushCallback = onMessageFlushCallback;
                callbackContainer->onFlushPromise = std::move(onTerminatePromise);

                if (m_clientContinuation.m_continuationToken)
                {
                    errorCode = aws_event_stream_rpc_client_continuation_send_message(
                        m_clientContinuation.m_continuationToken,
                        &msg_args,
                        ClientConnection::s_protocolMessageCallback,
                        reinterpret_cast<void *>(callbackContainer));
                }

                if (errorCode)
                {
                    onTerminatePromise = std::move(callbackContainer->onFlushPromise);
                    std::promise<RpcError> errorPromise;
                    AWS_LOGF_ERROR(
                        AWS_LS_EVENT_STREAM_RPC_CLIENT,
                        "A CRT error occurred while closing the stream: %s",
                        Crt::ErrorDebugString(errorCode));
                    onTerminatePromise.set_value({EVENT_STREAM_RPC_CRT_ERROR, errorCode});
                    Crt::Delete(callbackContainer, m_allocator);
                }
                else
                {
                    m_expectedCloses.fetch_add(1);
                    return callbackContainer->onFlushPromise.get_future();
                }

                return onTerminatePromise.get_future();
            }
        }

        void OperationError::s_customDeleter(OperationError *shape) noexcept
        {
            AbstractShapeBase::s_customDeleter(shape);
        }

    } /* namespace Eventstreamrpc */
} // namespace Aws