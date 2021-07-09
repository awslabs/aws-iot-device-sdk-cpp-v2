#pragma once
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

/* This file is generated. */

#include <aws/eventstreamrpc/EventStreamClient.h>

#include <aws/crt/DateTime.h>
#include <awstest/Exports.h>

using namespace Aws::Eventstreamrpc;

namespace Awstest
{
    class EchoTestRpcClient;
    class EchoTestRpcServiceModel;
    class Product : public AbstractShapeBase
    {
      public:
        Product() noexcept {}
        Product(const Product &) = default;
        void SetName(const Aws::Crt::String &name) noexcept { m_name = name; }
        Aws::Crt::Optional<Aws::Crt::String> GetName() noexcept { return m_name; }
        void SetPrice(const float &price) noexcept { m_price = price; }
        Aws::Crt::Optional<float> GetPrice() noexcept { return m_price; }
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(Product &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(Product *) noexcept;
        /* This needs to be defined so that `Product` can be used as a key in maps. */
        bool operator<(const Product &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
        Aws::Crt::Optional<Aws::Crt::String> m_name;
        Aws::Crt::Optional<float> m_price;
    };

    class Customer : public AbstractShapeBase
    {
      public:
        Customer() noexcept {}
        Customer(const Customer &) = default;
        void SetId(const int64_t &id) noexcept { m_id = id; }
        Aws::Crt::Optional<int64_t> GetId() noexcept { return m_id; }
        void SetFirstName(const Aws::Crt::String &firstName) noexcept { m_firstName = firstName; }
        Aws::Crt::Optional<Aws::Crt::String> GetFirstName() noexcept { return m_firstName; }
        void SetLastName(const Aws::Crt::String &lastName) noexcept { m_lastName = lastName; }
        Aws::Crt::Optional<Aws::Crt::String> GetLastName() noexcept { return m_lastName; }
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(Customer &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(Customer *) noexcept;
        /* This needs to be defined so that `Customer` can be used as a key in maps. */
        bool operator<(const Customer &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
        Aws::Crt::Optional<int64_t> m_id;
        Aws::Crt::Optional<Aws::Crt::String> m_firstName;
        Aws::Crt::Optional<Aws::Crt::String> m_lastName;
    };

    enum FruitEnum
    {
        FRUIT_ENUM_APPLE,
        FRUIT_ENUM_ORANGE,
        FRUIT_ENUM_BANANA,
        FRUIT_ENUM_PINEAPPLE
    };

    class Pair : public AbstractShapeBase
    {
      public:
        Pair() noexcept {}
        Pair(const Pair &) = default;
        void SetKey(const Aws::Crt::String &key) noexcept { m_key = key; }
        Aws::Crt::Optional<Aws::Crt::String> GetKey() noexcept { return m_key; }
        void SetValue(const Aws::Crt::String &value) noexcept { m_value = value; }
        Aws::Crt::Optional<Aws::Crt::String> GetValue() noexcept { return m_value; }
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(Pair &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(Pair *) noexcept;
        /* This needs to be defined so that `Pair` can be used as a key in maps. */
        bool operator<(const Pair &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
        Aws::Crt::Optional<Aws::Crt::String> m_key;
        Aws::Crt::Optional<Aws::Crt::String> m_value;
    };

    class MessageData : public AbstractShapeBase
    {
      public:
        MessageData() noexcept {}
        MessageData(const MessageData &) = default;
        void SetStringMessage(const Aws::Crt::String &stringMessage) noexcept { m_stringMessage = stringMessage; }
        Aws::Crt::Optional<Aws::Crt::String> GetStringMessage() noexcept { return m_stringMessage; }
        void SetBooleanMessage(const bool &booleanMessage) noexcept { m_booleanMessage = booleanMessage; }
        Aws::Crt::Optional<bool> GetBooleanMessage() noexcept { return m_booleanMessage; }
        void SetTimeMessage(const Aws::Crt::DateTime &timeMessage) noexcept { m_timeMessage = timeMessage; }
        Aws::Crt::Optional<Aws::Crt::DateTime> GetTimeMessage() noexcept { return m_timeMessage; }
        void SetDocumentMessage(const Aws::Crt::JsonObject &documentMessage) noexcept
        {
            m_documentMessage = documentMessage;
        }
        Aws::Crt::Optional<Aws::Crt::JsonObject> GetDocumentMessage() noexcept { return m_documentMessage; }
        void SetEnumMessage(FruitEnum enumMessage) noexcept;
        Aws::Crt::Optional<FruitEnum> GetEnumMessage() noexcept;
        void SetBlobMessage(const Aws::Crt::Vector<uint8_t> &blobMessage) noexcept { m_blobMessage = blobMessage; }
        Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> GetBlobMessage() noexcept { return m_blobMessage; }
        void SetStringListMessage(const Aws::Crt::Vector<Aws::Crt::String> &stringListMessage) noexcept
        {
            m_stringListMessage = stringListMessage;
        }
        Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> GetStringListMessage() noexcept
        {
            return m_stringListMessage;
        }
        void SetKeyValuePairList(const Aws::Crt::Vector<Pair> &keyValuePairList) noexcept
        {
            m_keyValuePairList = keyValuePairList;
        }
        Aws::Crt::Optional<Aws::Crt::Vector<Pair>> GetKeyValuePairList() noexcept { return m_keyValuePairList; }
        void SetStringToValue(const Aws::Crt::Map<Aws::Crt::String, Product> &stringToValue) noexcept
        {
            m_stringToValue = stringToValue;
        }
        Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Product>> GetStringToValue() noexcept
        {
            return m_stringToValue;
        }
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(MessageData &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(MessageData *) noexcept;
        /* This needs to be defined so that `MessageData` can be used as a key in maps. */
        bool operator<(const MessageData &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
        Aws::Crt::Optional<Aws::Crt::String> m_stringMessage;
        Aws::Crt::Optional<bool> m_booleanMessage;
        Aws::Crt::Optional<Aws::Crt::DateTime> m_timeMessage;
        Aws::Crt::Optional<Aws::Crt::JsonObject> m_documentMessage;
        Aws::Crt::Optional<Aws::Crt::String> m_enumMessage;
        Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>> m_blobMessage;
        Aws::Crt::Optional<Aws::Crt::Vector<Aws::Crt::String>> m_stringListMessage;
        Aws::Crt::Optional<Aws::Crt::Vector<Pair>> m_keyValuePairList;
        Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Product>> m_stringToValue;
    };

    class EchoStreamingMessage : public AbstractShapeBase
    {
      public:
        EchoStreamingMessage() noexcept {}
        EchoStreamingMessage &operator=(const EchoStreamingMessage &) noexcept;
        EchoStreamingMessage(const EchoStreamingMessage &objectToCopy) { *this = objectToCopy; }
        void SetStreamMessage(const MessageData &streamMessage) noexcept
        {
            m_streamMessage = streamMessage;
            m_chosenMember = TAG_STREAM_MESSAGE;
        }
        Aws::Crt::Optional<MessageData> GetStreamMessage() noexcept
        {
            if (m_chosenMember == TAG_STREAM_MESSAGE)
            {
                return m_streamMessage;
            }
            else
            {
                return Aws::Crt::Optional<MessageData>();
            }
        }
        void SetKeyValuePair(const Pair &keyValuePair) noexcept
        {
            m_keyValuePair = keyValuePair;
            m_chosenMember = TAG_KEY_VALUE_PAIR;
        }
        Aws::Crt::Optional<Pair> GetKeyValuePair() noexcept
        {
            if (m_chosenMember == TAG_KEY_VALUE_PAIR)
            {
                return m_keyValuePair;
            }
            else
            {
                return Aws::Crt::Optional<Pair>();
            }
        }
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(EchoStreamingMessage &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(EchoStreamingMessage *) noexcept;
        /* This needs to be defined so that `EchoStreamingMessage` can be used as a key in maps. */
        bool operator<(const EchoStreamingMessage &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
        enum ChosenMember
        {
            TAG_STREAM_MESSAGE,
            TAG_KEY_VALUE_PAIR
        } m_chosenMember;
        Aws::Crt::Optional<MessageData> m_streamMessage;
        Aws::Crt::Optional<Pair> m_keyValuePair;
    };

    class GetAllProductsResponse : public AbstractShapeBase
    {
      public:
        GetAllProductsResponse() noexcept {}
        GetAllProductsResponse(const GetAllProductsResponse &) = default;
        void SetProducts(const Aws::Crt::Map<Aws::Crt::String, Product> &products) noexcept { m_products = products; }
        Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Product>> GetProducts() noexcept { return m_products; }
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(GetAllProductsResponse &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(GetAllProductsResponse *) noexcept;
        /* This needs to be defined so that `GetAllProductsResponse` can be used as a key in maps. */
        bool operator<(const GetAllProductsResponse &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
        Aws::Crt::Optional<Aws::Crt::Map<Aws::Crt::String, Product>> m_products;
    };

    class GetAllProductsRequest : public AbstractShapeBase
    {
      public:
        GetAllProductsRequest() noexcept {}
        GetAllProductsRequest(const GetAllProductsRequest &) = default;
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(GetAllProductsRequest &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(GetAllProductsRequest *) noexcept;
        /* This needs to be defined so that `GetAllProductsRequest` can be used as a key in maps. */
        bool operator<(const GetAllProductsRequest &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
    };

    class GetAllCustomersResponse : public AbstractShapeBase
    {
      public:
        GetAllCustomersResponse() noexcept {}
        GetAllCustomersResponse(const GetAllCustomersResponse &) = default;
        void SetCustomers(const Aws::Crt::Vector<Customer> &customers) noexcept { m_customers = customers; }
        Aws::Crt::Optional<Aws::Crt::Vector<Customer>> GetCustomers() noexcept { return m_customers; }
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(GetAllCustomersResponse &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(GetAllCustomersResponse *) noexcept;
        /* This needs to be defined so that `GetAllCustomersResponse` can be used as a key in maps. */
        bool operator<(const GetAllCustomersResponse &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
        Aws::Crt::Optional<Aws::Crt::Vector<Customer>> m_customers;
    };

    class GetAllCustomersRequest : public AbstractShapeBase
    {
      public:
        GetAllCustomersRequest() noexcept {}
        GetAllCustomersRequest(const GetAllCustomersRequest &) = default;
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(GetAllCustomersRequest &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(GetAllCustomersRequest *) noexcept;
        /* This needs to be defined so that `GetAllCustomersRequest` can be used as a key in maps. */
        bool operator<(const GetAllCustomersRequest &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
    };

    class EchoMessageResponse : public AbstractShapeBase
    {
      public:
        EchoMessageResponse() noexcept {}
        EchoMessageResponse(const EchoMessageResponse &) = default;
        void SetMessage(const MessageData &message) noexcept { m_message = message; }
        Aws::Crt::Optional<MessageData> GetMessage() noexcept { return m_message; }
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(EchoMessageResponse &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(EchoMessageResponse *) noexcept;
        /* This needs to be defined so that `EchoMessageResponse` can be used as a key in maps. */
        bool operator<(const EchoMessageResponse &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
        Aws::Crt::Optional<MessageData> m_message;
    };

    class EchoMessageRequest : public AbstractShapeBase
    {
      public:
        EchoMessageRequest() noexcept {}
        EchoMessageRequest(const EchoMessageRequest &) = default;
        void SetMessage(const MessageData &message) noexcept { m_message = message; }
        Aws::Crt::Optional<MessageData> GetMessage() noexcept { return m_message; }
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(EchoMessageRequest &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(EchoMessageRequest *) noexcept;
        /* This needs to be defined so that `EchoMessageRequest` can be used as a key in maps. */
        bool operator<(const EchoMessageRequest &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
        Aws::Crt::Optional<MessageData> m_message;
    };

    class EchoStreamingResponse : public AbstractShapeBase
    {
      public:
        EchoStreamingResponse() noexcept {}
        EchoStreamingResponse(const EchoStreamingResponse &) = default;
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(EchoStreamingResponse &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(EchoStreamingResponse *) noexcept;
        /* This needs to be defined so that `EchoStreamingResponse` can be used as a key in maps. */
        bool operator<(const EchoStreamingResponse &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
    };

    class EchoStreamingRequest : public AbstractShapeBase
    {
      public:
        EchoStreamingRequest() noexcept {}
        EchoStreamingRequest(const EchoStreamingRequest &) = default;
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(EchoStreamingRequest &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(EchoStreamingRequest *) noexcept;
        /* This needs to be defined so that `EchoStreamingRequest` can be used as a key in maps. */
        bool operator<(const EchoStreamingRequest &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
    };

    class ServiceError : public OperationError
    {
      public:
        ServiceError() noexcept {}
        ServiceError(const ServiceError &) = default;
        void SetMessage(const Aws::Crt::String &message) noexcept { m_message = message; }
        Aws::Crt::Optional<Aws::Crt::String> GetMessage() noexcept override { return m_message; }
        void SetValue(const Aws::Crt::String &value) noexcept { m_value = value; }
        Aws::Crt::Optional<Aws::Crt::String> GetValue() noexcept { return m_value; }
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(ServiceError &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<OperationError> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(ServiceError *) noexcept;
        /* This needs to be defined so that `ServiceError` can be used as a key in maps. */
        bool operator<(const ServiceError &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
        Aws::Crt::Optional<Aws::Crt::String> m_message;
        Aws::Crt::Optional<Aws::Crt::String> m_value;
    };

    class CauseServiceErrorResponse : public AbstractShapeBase
    {
      public:
        CauseServiceErrorResponse() noexcept {}
        CauseServiceErrorResponse(const CauseServiceErrorResponse &) = default;
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(CauseServiceErrorResponse &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(CauseServiceErrorResponse *) noexcept;
        /* This needs to be defined so that `CauseServiceErrorResponse` can be used as a key in maps. */
        bool operator<(const CauseServiceErrorResponse &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
    };

    class CauseServiceErrorRequest : public AbstractShapeBase
    {
      public:
        CauseServiceErrorRequest() noexcept {}
        CauseServiceErrorRequest(const CauseServiceErrorRequest &) = default;
        void SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept override;
        static void s_loadFromJsonView(CauseServiceErrorRequest &, const Aws::Crt::JsonView &) noexcept;
        static Aws::Crt::ScopedResource<AbstractShapeBase> s_allocateFromPayload(
            Aws::Crt::StringView,
            Aws::Crt::Allocator *) noexcept;
        static void s_customDeleter(CauseServiceErrorRequest *) noexcept;
        /* This needs to be defined so that `CauseServiceErrorRequest` can be used as a key in maps. */
        bool operator<(const CauseServiceErrorRequest &) const noexcept;
        static const char *MODEL_NAME;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;

      private:
    };

    class GetAllProductsOperationContext : public OperationModelContext
    {
      public:
        GetAllProductsOperationContext(const EchoTestRpcServiceModel &serviceModel) noexcept;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::String GetRequestModelName() const noexcept override;
        Aws::Crt::String GetInitialResponseModelName() const noexcept override;
        Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
        Aws::Crt::String GetOperationName() const noexcept override;
    };

    class GetAllProductsResult
    {
      public:
        GetAllProductsResult() noexcept {}
        GetAllProductsResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
        GetAllProductsResponse *GetOperationResponse() const noexcept
        {
            return static_cast<GetAllProductsResponse *>(m_taggedResult.GetOperationResponse());
        }
        /**
         * @return true if the response is associated with an expected response;
         * false if the response is associated with an error.
         */
        operator bool() const noexcept { return m_taggedResult == true; }
        OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
        RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
        ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

      private:
        TaggedResult m_taggedResult;
    };

    class GetAllProductsOperation : public ClientOperation
    {
      public:
        GetAllProductsOperation(
            ClientConnection &connection,
            const GetAllProductsOperationContext &operationContext,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
        /**
         * Used to activate a stream for the `GetAllProductsOperation`
         * @param request The request used for the `GetAllProductsOperation`
         * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
         * @return An `RpcError` that can be used to check whether the stream was activated.
         */
        std::future<RpcError> Activate(
            const GetAllProductsRequest &request,
            OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
        /**
         * Retrieve the result from activating the stream.
         */
        std::future<GetAllProductsResult> GetResult() noexcept;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;
    };

    class CauseServiceErrorOperationContext : public OperationModelContext
    {
      public:
        CauseServiceErrorOperationContext(const EchoTestRpcServiceModel &serviceModel) noexcept;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::String GetRequestModelName() const noexcept override;
        Aws::Crt::String GetInitialResponseModelName() const noexcept override;
        Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
        Aws::Crt::String GetOperationName() const noexcept override;
    };

    class CauseServiceErrorResult
    {
      public:
        CauseServiceErrorResult() noexcept {}
        CauseServiceErrorResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
        CauseServiceErrorResponse *GetOperationResponse() const noexcept
        {
            return static_cast<CauseServiceErrorResponse *>(m_taggedResult.GetOperationResponse());
        }
        /**
         * @return true if the response is associated with an expected response;
         * false if the response is associated with an error.
         */
        operator bool() const noexcept { return m_taggedResult == true; }
        OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
        RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
        ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

      private:
        TaggedResult m_taggedResult;
    };

    class CauseServiceErrorOperation : public ClientOperation
    {
      public:
        CauseServiceErrorOperation(
            ClientConnection &connection,
            const CauseServiceErrorOperationContext &operationContext,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
        /**
         * Used to activate a stream for the `CauseServiceErrorOperation`
         * @param request The request used for the `CauseServiceErrorOperation`
         * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
         * @return An `RpcError` that can be used to check whether the stream was activated.
         */
        std::future<RpcError> Activate(
            const CauseServiceErrorRequest &request,
            OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
        /**
         * Retrieve the result from activating the stream.
         */
        std::future<CauseServiceErrorResult> GetResult() noexcept;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;
    };

    class CauseStreamServiceToErrorStreamHandler : public StreamResponseHandler
    {
      public:
        virtual void OnStreamEvent(EchoStreamingMessage *response) { (void)response; }

        /**
         * A callback that is invoked when an error occurs while parsing a message from the stream.
         * @param rpcError The RPC error containing the status and possibly a CRT error.
         */
        virtual bool OnStreamError(RpcError rpcError)
        {
            (void)rpcError;
            return true;
        }

        /**
         * A callback that is invoked upon receiving an error of type `ServiceError`.
         * @param operationError The error message being received.
         */
        virtual bool OnStreamError(ServiceError *operationError)
        {
            (void)operationError;
            return true;
        }

        /**
         * A callback that is invoked upon receiving ANY error response from the server.
         * @param operationError The error message being received.
         */
        virtual bool OnStreamError(OperationError *operationError)
        {
            (void)operationError;
            return true;
        }

      private:
        /**
         * Invoked when a message is received on this continuation.
         */
        void OnStreamEvent(Aws::Crt::ScopedResource<AbstractShapeBase> response) override;
        /**
         * Invoked when a message is received on this continuation but results in an error.
         *
         * This callback can return true so that the stream is closed afterwards.
         */
        bool OnStreamError(Aws::Crt::ScopedResource<OperationError> error, RpcError rpcError) override;
    };
    class CauseStreamServiceToErrorOperationContext : public OperationModelContext
    {
      public:
        CauseStreamServiceToErrorOperationContext(const EchoTestRpcServiceModel &serviceModel) noexcept;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::String GetRequestModelName() const noexcept override;
        Aws::Crt::String GetInitialResponseModelName() const noexcept override;
        Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
        Aws::Crt::String GetOperationName() const noexcept override;
    };

    class CauseStreamServiceToErrorResult
    {
      public:
        CauseStreamServiceToErrorResult() noexcept {}
        CauseStreamServiceToErrorResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult))
        {
        }
        EchoStreamingResponse *GetOperationResponse() const noexcept
        {
            return static_cast<EchoStreamingResponse *>(m_taggedResult.GetOperationResponse());
        }
        /**
         * @return true if the response is associated with an expected response;
         * false if the response is associated with an error.
         */
        operator bool() const noexcept { return m_taggedResult == true; }
        OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
        RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
        ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

      private:
        TaggedResult m_taggedResult;
    };

    class CauseStreamServiceToErrorOperation : public ClientOperation
    {
      public:
        CauseStreamServiceToErrorOperation(
            ClientConnection &connection,
            CauseStreamServiceToErrorStreamHandler *streamHandler,
            const CauseStreamServiceToErrorOperationContext &operationContext,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
        /**
         * Used to activate a stream for the `CauseStreamServiceToErrorOperation`
         * @param request The request used for the `CauseStreamServiceToErrorOperation`
         * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
         * @return An `RpcError` that can be used to check whether the stream was activated.
         */
        std::future<RpcError> Activate(
            const EchoStreamingRequest &request,
            OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
        /**
         * Retrieve the result from activating the stream.
         */
        std::future<CauseStreamServiceToErrorResult> GetResult() noexcept;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;
    };

    class EchoStreamMessagesStreamHandler : public StreamResponseHandler
    {
      public:
        virtual void OnStreamEvent(EchoStreamingMessage *response) { (void)response; }

        /**
         * A callback that is invoked when an error occurs while parsing a message from the stream.
         * @param rpcError The RPC error containing the status and possibly a CRT error.
         */
        virtual bool OnStreamError(RpcError rpcError)
        {
            (void)rpcError;
            return true;
        }

        /**
         * A callback that is invoked upon receiving ANY error response from the server.
         * @param operationError The error message being received.
         */
        virtual bool OnStreamError(OperationError *operationError)
        {
            (void)operationError;
            return true;
        }

      private:
        /**
         * Invoked when a message is received on this continuation.
         */
        void OnStreamEvent(Aws::Crt::ScopedResource<AbstractShapeBase> response) override;
        /**
         * Invoked when a message is received on this continuation but results in an error.
         *
         * This callback can return true so that the stream is closed afterwards.
         */
        bool OnStreamError(Aws::Crt::ScopedResource<OperationError> error, RpcError rpcError) override;
    };
    class EchoStreamMessagesOperationContext : public OperationModelContext
    {
      public:
        EchoStreamMessagesOperationContext(const EchoTestRpcServiceModel &serviceModel) noexcept;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::String GetRequestModelName() const noexcept override;
        Aws::Crt::String GetInitialResponseModelName() const noexcept override;
        Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
        Aws::Crt::String GetOperationName() const noexcept override;
    };

    class EchoStreamMessagesResult
    {
      public:
        EchoStreamMessagesResult() noexcept {}
        EchoStreamMessagesResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
        EchoStreamingResponse *GetOperationResponse() const noexcept
        {
            return static_cast<EchoStreamingResponse *>(m_taggedResult.GetOperationResponse());
        }
        /**
         * @return true if the response is associated with an expected response;
         * false if the response is associated with an error.
         */
        operator bool() const noexcept { return m_taggedResult == true; }
        OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
        RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
        ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

      private:
        TaggedResult m_taggedResult;
    };

    class EchoStreamMessagesOperation : public ClientOperation
    {
      public:
        EchoStreamMessagesOperation(
            ClientConnection &connection,
            EchoStreamMessagesStreamHandler *streamHandler,
            const EchoStreamMessagesOperationContext &operationContext,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
        /**
         * Used to activate a stream for the `EchoStreamMessagesOperation`
         * @param request The request used for the `EchoStreamMessagesOperation`
         * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
         * @return An `RpcError` that can be used to check whether the stream was activated.
         */
        std::future<RpcError> Activate(
            const EchoStreamingRequest &request,
            OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
        /**
         * Retrieve the result from activating the stream.
         */
        std::future<EchoStreamMessagesResult> GetResult() noexcept;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;
    };

    class EchoMessageOperationContext : public OperationModelContext
    {
      public:
        EchoMessageOperationContext(const EchoTestRpcServiceModel &serviceModel) noexcept;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::String GetRequestModelName() const noexcept override;
        Aws::Crt::String GetInitialResponseModelName() const noexcept override;
        Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
        Aws::Crt::String GetOperationName() const noexcept override;
    };

    class EchoMessageResult
    {
      public:
        EchoMessageResult() noexcept {}
        EchoMessageResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
        EchoMessageResponse *GetOperationResponse() const noexcept
        {
            return static_cast<EchoMessageResponse *>(m_taggedResult.GetOperationResponse());
        }
        /**
         * @return true if the response is associated with an expected response;
         * false if the response is associated with an error.
         */
        operator bool() const noexcept { return m_taggedResult == true; }
        OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
        RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
        ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

      private:
        TaggedResult m_taggedResult;
    };

    class EchoMessageOperation : public ClientOperation
    {
      public:
        EchoMessageOperation(
            ClientConnection &connection,
            const EchoMessageOperationContext &operationContext,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
        /**
         * Used to activate a stream for the `EchoMessageOperation`
         * @param request The request used for the `EchoMessageOperation`
         * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
         * @return An `RpcError` that can be used to check whether the stream was activated.
         */
        std::future<RpcError> Activate(
            const EchoMessageRequest &request,
            OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
        /**
         * Retrieve the result from activating the stream.
         */
        std::future<EchoMessageResult> GetResult() noexcept;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;
    };

    class GetAllCustomersOperationContext : public OperationModelContext
    {
      public:
        GetAllCustomersOperationContext(const EchoTestRpcServiceModel &serviceModel) noexcept;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateInitialResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::ScopedResource<AbstractShapeBase> AllocateStreamingResponseFromPayload(
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        Aws::Crt::String GetRequestModelName() const noexcept override;
        Aws::Crt::String GetInitialResponseModelName() const noexcept override;
        Aws::Crt::Optional<Aws::Crt::String> GetStreamingResponseModelName() const noexcept override;
        Aws::Crt::String GetOperationName() const noexcept override;
    };

    class GetAllCustomersResult
    {
      public:
        GetAllCustomersResult() noexcept {}
        GetAllCustomersResult(TaggedResult &&taggedResult) noexcept : m_taggedResult(std::move(taggedResult)) {}
        GetAllCustomersResponse *GetOperationResponse() const noexcept
        {
            return static_cast<GetAllCustomersResponse *>(m_taggedResult.GetOperationResponse());
        }
        /**
         * @return true if the response is associated with an expected response;
         * false if the response is associated with an error.
         */
        operator bool() const noexcept { return m_taggedResult == true; }
        OperationError *GetOperationError() const noexcept { return m_taggedResult.GetOperationError(); }
        RpcError GetRpcError() const noexcept { return m_taggedResult.GetRpcError(); }
        ResultType GetResultType() const noexcept { return m_taggedResult.GetResultType(); }

      private:
        TaggedResult m_taggedResult;
    };

    class GetAllCustomersOperation : public ClientOperation
    {
      public:
        GetAllCustomersOperation(
            ClientConnection &connection,
            const GetAllCustomersOperationContext &operationContext,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) noexcept;
        /**
         * Used to activate a stream for the `GetAllCustomersOperation`
         * @param request The request used for the `GetAllCustomersOperation`
         * @param onMessageFlushCallback An optional callback that is invoked when the request is flushed.
         * @return An `RpcError` that can be used to check whether the stream was activated.
         */
        std::future<RpcError> Activate(
            const GetAllCustomersRequest &request,
            OnMessageFlushCallback onMessageFlushCallback = nullptr) noexcept;
        /**
         * Retrieve the result from activating the stream.
         */
        std::future<GetAllCustomersResult> GetResult() noexcept;

      protected:
        Aws::Crt::String GetModelName() const noexcept override;
    };

    class EchoTestRpcServiceModel : public ServiceModel
    {
      public:
        EchoTestRpcServiceModel() noexcept;
        Aws::Crt::ScopedResource<OperationError> AllocateOperationErrorFromPayload(
            const Aws::Crt::String &errorModelName,
            Aws::Crt::StringView stringView,
            Aws::Crt::Allocator *allocator = Aws::Crt::g_allocator) const noexcept override;
        void AssignModelNameToErrorResponse(Aws::Crt::String, ErrorResponseFactory) noexcept;

      private:
        friend class EchoTestRpcClient;
        GetAllProductsOperationContext m_getAllProductsOperationContext;
        CauseServiceErrorOperationContext m_causeServiceErrorOperationContext;
        CauseStreamServiceToErrorOperationContext m_causeStreamServiceToErrorOperationContext;
        EchoStreamMessagesOperationContext m_echoStreamMessagesOperationContext;
        EchoMessageOperationContext m_echoMessageOperationContext;
        GetAllCustomersOperationContext m_getAllCustomersOperationContext;
        Aws::Crt::Map<Aws::Crt::String, ErrorResponseFactory> m_modelNameToErrorResponse;
    };
} // namespace Awstest
