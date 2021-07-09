#include <aws/crt/Api.h>
#include <awstest/EchoTestRpcModel.h>

namespace Awstest
{
    void Product::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
    {
        if (m_name.has_value())
        {
            payloadObject.WithString("name", m_name.value());
        }
        if (m_price.has_value())
        {
            payloadObject.WithDouble("price", static_cast<double>(m_price.value()));
        }
    }

    void Product::s_loadFromJsonView(Product &product, const Aws::Crt::JsonView &jsonView) noexcept
    {
        if (jsonView.ValueExists("name"))
        {
            product.m_name = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("name"));
        }
        if (jsonView.ValueExists("price"))
        {
            product.m_price = Aws::Crt::Optional<float>(static_cast<float>(jsonView.GetDouble("price")));
        }
    }

    const char *Product::MODEL_NAME = "awstest#Product";

    Aws::Crt::String Product::GetModelName() const noexcept { return Product::MODEL_NAME; }

    Aws::Crt::ScopedResource<AbstractShapeBase> Product::s_allocateFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) noexcept
    {
        Aws::Crt::String payload = {stringView.begin(), stringView.end()};
        Aws::Crt::JsonObject jsonObject(payload);
        Aws::Crt::JsonView jsonView(jsonObject);

        Aws::Crt::ScopedResource<Product> shape(Aws::Crt::New<Product>(allocator), Product::s_customDeleter);
        shape->m_allocator = allocator;
        Product::s_loadFromJsonView(*shape, jsonView);
        auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
        return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
    }

    void Product::s_customDeleter(Product *shape) noexcept
    {
        AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
    }

    void Customer::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
    {
        if (m_id.has_value())
        {
            payloadObject.WithInt64("id", m_id.value());
        }
        if (m_firstName.has_value())
        {
            payloadObject.WithString("firstName", m_firstName.value());
        }
        if (m_lastName.has_value())
        {
            payloadObject.WithString("lastName", m_lastName.value());
        }
    }

    void Customer::s_loadFromJsonView(Customer &customer, const Aws::Crt::JsonView &jsonView) noexcept
    {
        if (jsonView.ValueExists("id"))
        {
            customer.m_id = Aws::Crt::Optional<int64_t>(jsonView.GetInt64("id"));
        }
        if (jsonView.ValueExists("firstName"))
        {
            customer.m_firstName = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("firstName"));
        }
        if (jsonView.ValueExists("lastName"))
        {
            customer.m_lastName = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("lastName"));
        }
    }

    const char *Customer::MODEL_NAME = "awstest#Customer";

    Aws::Crt::String Customer::GetModelName() const noexcept { return Customer::MODEL_NAME; }

    Aws::Crt::ScopedResource<AbstractShapeBase> Customer::s_allocateFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) noexcept
    {
        Aws::Crt::String payload = {stringView.begin(), stringView.end()};
        Aws::Crt::JsonObject jsonObject(payload);
        Aws::Crt::JsonView jsonView(jsonObject);

        Aws::Crt::ScopedResource<Customer> shape(Aws::Crt::New<Customer>(allocator), Customer::s_customDeleter);
        shape->m_allocator = allocator;
        Customer::s_loadFromJsonView(*shape, jsonView);
        auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
        return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
    }

    void Customer::s_customDeleter(Customer *shape) noexcept
    {
        AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
    }

    void Pair::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
    {
        if (m_key.has_value())
        {
            payloadObject.WithString("key", m_key.value());
        }
        if (m_value.has_value())
        {
            payloadObject.WithString("value", m_value.value());
        }
    }

    void Pair::s_loadFromJsonView(Pair &pair, const Aws::Crt::JsonView &jsonView) noexcept
    {
        if (jsonView.ValueExists("key"))
        {
            pair.m_key = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("key"));
        }
        if (jsonView.ValueExists("value"))
        {
            pair.m_value = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("value"));
        }
    }

    const char *Pair::MODEL_NAME = "awstest#Pair";

    Aws::Crt::String Pair::GetModelName() const noexcept { return Pair::MODEL_NAME; }

    Aws::Crt::ScopedResource<AbstractShapeBase> Pair::s_allocateFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) noexcept
    {
        Aws::Crt::String payload = {stringView.begin(), stringView.end()};
        Aws::Crt::JsonObject jsonObject(payload);
        Aws::Crt::JsonView jsonView(jsonObject);

        Aws::Crt::ScopedResource<Pair> shape(Aws::Crt::New<Pair>(allocator), Pair::s_customDeleter);
        shape->m_allocator = allocator;
        Pair::s_loadFromJsonView(*shape, jsonView);
        auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
        return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
    }

    void Pair::s_customDeleter(Pair *shape) noexcept
    {
        AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
    }

    void MessageData::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
    {
        if (m_stringMessage.has_value())
        {
            payloadObject.WithString("stringMessage", m_stringMessage.value());
        }
        if (m_booleanMessage.has_value())
        {
            payloadObject.WithBool("booleanMessage", m_booleanMessage.value());
        }
        if (m_timeMessage.has_value())
        {
            payloadObject.WithDouble("timeMessage", m_timeMessage.value().SecondsWithMSPrecision());
        }
        if (m_documentMessage.has_value())
        {
            payloadObject.WithObject("documentMessage", m_documentMessage.value());
        }
        if (m_enumMessage.has_value())
        {
            payloadObject.WithString("enumMessage", m_enumMessage.value());
        }
        if (m_blobMessage.has_value())
        {
            if (m_blobMessage.value().size() > 0)
            {
                payloadObject.WithString("blobMessage", Aws::Crt::Base64Encode(m_blobMessage.value()));
            }
        }
        if (m_stringListMessage.has_value())
        {
            Aws::Crt::JsonObject stringList;
            Aws::Crt::Vector<Aws::Crt::JsonObject> stringListJsonArray;
            for (const auto &stringListItem : m_stringListMessage.value())
            {
                Aws::Crt::JsonObject stringListJsonArrayItem;
                stringListJsonArrayItem.AsString(stringListItem);
                stringListJsonArray.emplace_back(std::move(stringListJsonArrayItem));
            }
            stringList.AsArray(std::move(stringListJsonArray));
            payloadObject.WithObject("stringListMessage", std::move(stringList));
        }
        if (m_keyValuePairList.has_value())
        {
            Aws::Crt::JsonObject keyValuePairList;
            Aws::Crt::Vector<Aws::Crt::JsonObject> keyValuePairListJsonArray;
            for (const auto &keyValuePairListItem : m_keyValuePairList.value())
            {
                Aws::Crt::JsonObject keyValuePairListJsonArrayItem;
                keyValuePairListItem.SerializeToJsonObject(keyValuePairListJsonArrayItem);
                keyValuePairListJsonArray.emplace_back(std::move(keyValuePairListJsonArrayItem));
            }
            keyValuePairList.AsArray(std::move(keyValuePairListJsonArray));
            payloadObject.WithObject("keyValuePairList", std::move(keyValuePairList));
        }
        if (m_stringToValue.has_value())
        {
            Aws::Crt::JsonObject stringToValueValue;
            for (const auto &stringToValueItem : m_stringToValue.value())
            {
                Aws::Crt::JsonObject stringToValueJsonObject;
                stringToValueItem.second.SerializeToJsonObject(stringToValueJsonObject);
                stringToValueValue.WithObject(stringToValueItem.first, std::move(stringToValueJsonObject));
            }
            payloadObject.WithObject("stringToValue", std::move(stringToValueValue));
        }
    }

    void MessageData::s_loadFromJsonView(MessageData &messageData, const Aws::Crt::JsonView &jsonView) noexcept
    {
        if (jsonView.ValueExists("stringMessage"))
        {
            messageData.m_stringMessage = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("stringMessage"));
        }
        if (jsonView.ValueExists("booleanMessage"))
        {
            messageData.m_booleanMessage = Aws::Crt::Optional<bool>(jsonView.GetBool("booleanMessage"));
        }
        if (jsonView.ValueExists("timeMessage"))
        {
            messageData.m_timeMessage =
                Aws::Crt::Optional<Aws::Crt::DateTime>(Aws::Crt::DateTime(jsonView.GetDouble("timeMessage")));
        }
        if (jsonView.ValueExists("documentMessage"))
        {
            messageData.m_documentMessage =
                Aws::Crt::Optional<Aws::Crt::JsonObject>(jsonView.GetJsonObject("documentMessage").Materialize());
        }
        if (jsonView.ValueExists("enumMessage"))
        {
            messageData.m_enumMessage = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("enumMessage"));
        }
        if (jsonView.ValueExists("blobMessage"))
        {
            if (jsonView.GetString("blobMessage").size() > 0)
            {
                messageData.m_blobMessage = Aws::Crt::Optional<Aws::Crt::Vector<uint8_t>>(
                    Aws::Crt::Base64Decode(jsonView.GetString("blobMessage")));
            }
        }
        if (jsonView.ValueExists("stringListMessage"))
        {
            messageData.m_stringListMessage = Aws::Crt::Vector<Aws::Crt::String>();
            for (const Aws::Crt::JsonView &stringListJsonView : jsonView.GetArray("stringListMessage"))
            {
                Aws::Crt::Optional<Aws::Crt::String> stringListItem;
                stringListItem = Aws::Crt::Optional<Aws::Crt::String>(stringListJsonView.AsString());
                messageData.m_stringListMessage.value().push_back(stringListItem.value());
            }
        }
        if (jsonView.ValueExists("keyValuePairList"))
        {
            messageData.m_keyValuePairList = Aws::Crt::Vector<Pair>();
            for (const Aws::Crt::JsonView &keyValuePairListJsonView : jsonView.GetArray("keyValuePairList"))
            {
                Aws::Crt::Optional<Pair> keyValuePairListItem;
                keyValuePairListItem = Pair();
                Pair::s_loadFromJsonView(keyValuePairListItem.value(), keyValuePairListJsonView);
                messageData.m_keyValuePairList.value().push_back(keyValuePairListItem.value());
            }
        }
        if (jsonView.ValueExists("stringToValue"))
        {
            messageData.m_stringToValue = Aws::Crt::Map<Aws::Crt::String, Product>();
            for (const auto &stringToValuePair : jsonView.GetJsonObject("stringToValue").GetAllObjects())
            {
                Aws::Crt::Optional<Product> stringToValueValue;
                stringToValueValue = Product();
                Product::s_loadFromJsonView(stringToValueValue.value(), stringToValuePair.second);
                messageData.m_stringToValue.value()[stringToValuePair.first] = stringToValueValue.value();
            }
        }
    }

    void MessageData::SetEnumMessage(FruitEnum enumMessage) noexcept
    {
        switch (enumMessage)
        {
            case FRUIT_ENUM_APPLE:
                m_enumMessage = Aws::Crt::String("apl");
                break;
            case FRUIT_ENUM_ORANGE:
                m_enumMessage = Aws::Crt::String("org");
                break;
            case FRUIT_ENUM_BANANA:
                m_enumMessage = Aws::Crt::String("ban");
                break;
            case FRUIT_ENUM_PINEAPPLE:
                m_enumMessage = Aws::Crt::String("pin");
                break;
            default:
                break;
        }
    }

    Aws::Crt::Optional<FruitEnum> MessageData::GetEnumMessage() noexcept
    {
        if (!m_enumMessage.has_value())
            return Aws::Crt::Optional<FruitEnum>();
        if (m_enumMessage.value() == Aws::Crt::String("apl"))
        {
            return Aws::Crt::Optional<FruitEnum>(FRUIT_ENUM_APPLE);
        }
        if (m_enumMessage.value() == Aws::Crt::String("org"))
        {
            return Aws::Crt::Optional<FruitEnum>(FRUIT_ENUM_ORANGE);
        }
        if (m_enumMessage.value() == Aws::Crt::String("ban"))
        {
            return Aws::Crt::Optional<FruitEnum>(FRUIT_ENUM_BANANA);
        }
        if (m_enumMessage.value() == Aws::Crt::String("pin"))
        {
            return Aws::Crt::Optional<FruitEnum>(FRUIT_ENUM_PINEAPPLE);
        }

        return Aws::Crt::Optional<FruitEnum>();
    }

    const char *MessageData::MODEL_NAME = "awstest#MessageData";

    Aws::Crt::String MessageData::GetModelName() const noexcept { return MessageData::MODEL_NAME; }

    Aws::Crt::ScopedResource<AbstractShapeBase> MessageData::s_allocateFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) noexcept
    {
        Aws::Crt::String payload = {stringView.begin(), stringView.end()};
        Aws::Crt::JsonObject jsonObject(payload);
        Aws::Crt::JsonView jsonView(jsonObject);

        Aws::Crt::ScopedResource<MessageData> shape(
            Aws::Crt::New<MessageData>(allocator), MessageData::s_customDeleter);
        shape->m_allocator = allocator;
        MessageData::s_loadFromJsonView(*shape, jsonView);
        auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
        return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
    }

    void MessageData::s_customDeleter(MessageData *shape) noexcept
    {
        AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
    }

    void EchoStreamingMessage::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
    {
        if (m_chosenMember == TAG_STREAM_MESSAGE && m_streamMessage.has_value())
        {
            Aws::Crt::JsonObject messageDataValue;
            m_streamMessage.value().SerializeToJsonObject(messageDataValue);
            payloadObject.WithObject("streamMessage", std::move(messageDataValue));
        }
        else if (m_chosenMember == TAG_KEY_VALUE_PAIR && m_keyValuePair.has_value())
        {
            Aws::Crt::JsonObject pairValue;
            m_keyValuePair.value().SerializeToJsonObject(pairValue);
            payloadObject.WithObject("keyValuePair", std::move(pairValue));
        }
    }

    void EchoStreamingMessage::s_loadFromJsonView(
        EchoStreamingMessage &echoStreamingMessage,
        const Aws::Crt::JsonView &jsonView) noexcept
    {
        if (jsonView.ValueExists("streamMessage"))
        {
            echoStreamingMessage.m_streamMessage = MessageData();
            MessageData::s_loadFromJsonView(
                echoStreamingMessage.m_streamMessage.value(), jsonView.GetJsonObject("streamMessage"));
            echoStreamingMessage.m_chosenMember = TAG_STREAM_MESSAGE;
        }
        else if (jsonView.ValueExists("keyValuePair"))
        {
            echoStreamingMessage.m_keyValuePair = Pair();
            Pair::s_loadFromJsonView(
                echoStreamingMessage.m_keyValuePair.value(), jsonView.GetJsonObject("keyValuePair"));
            echoStreamingMessage.m_chosenMember = TAG_KEY_VALUE_PAIR;
        }
    }

    EchoStreamingMessage &EchoStreamingMessage::operator=(const EchoStreamingMessage &objectToCopy) noexcept
    {
        if (objectToCopy.m_chosenMember == TAG_STREAM_MESSAGE)
        {
            m_streamMessage = objectToCopy.m_streamMessage;
            m_chosenMember = objectToCopy.m_chosenMember;
        }
        else if (objectToCopy.m_chosenMember == TAG_KEY_VALUE_PAIR)
        {
            m_keyValuePair = objectToCopy.m_keyValuePair;
            m_chosenMember = objectToCopy.m_chosenMember;
        }
        return *this;
    }

    const char *EchoStreamingMessage::MODEL_NAME = "awstest#EchoStreamingMessage";

    Aws::Crt::String EchoStreamingMessage::GetModelName() const noexcept { return EchoStreamingMessage::MODEL_NAME; }

    Aws::Crt::ScopedResource<AbstractShapeBase> EchoStreamingMessage::s_allocateFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) noexcept
    {
        Aws::Crt::String payload = {stringView.begin(), stringView.end()};
        Aws::Crt::JsonObject jsonObject(payload);
        Aws::Crt::JsonView jsonView(jsonObject);

        Aws::Crt::ScopedResource<EchoStreamingMessage> shape(
            Aws::Crt::New<EchoStreamingMessage>(allocator), EchoStreamingMessage::s_customDeleter);
        shape->m_allocator = allocator;
        EchoStreamingMessage::s_loadFromJsonView(*shape, jsonView);
        auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
        return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
    }

    void EchoStreamingMessage::s_customDeleter(EchoStreamingMessage *shape) noexcept
    {
        AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
    }

    void GetAllProductsResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
    {
        if (m_products.has_value())
        {
            Aws::Crt::JsonObject productMapValue;
            for (const auto &productMapItem : m_products.value())
            {
                Aws::Crt::JsonObject productMapJsonObject;
                productMapItem.second.SerializeToJsonObject(productMapJsonObject);
                productMapValue.WithObject(productMapItem.first, std::move(productMapJsonObject));
            }
            payloadObject.WithObject("products", std::move(productMapValue));
        }
    }

    void GetAllProductsResponse::s_loadFromJsonView(
        GetAllProductsResponse &getAllProductsResponse,
        const Aws::Crt::JsonView &jsonView) noexcept
    {
        if (jsonView.ValueExists("products"))
        {
            getAllProductsResponse.m_products = Aws::Crt::Map<Aws::Crt::String, Product>();
            for (const auto &productMapPair : jsonView.GetJsonObject("products").GetAllObjects())
            {
                Aws::Crt::Optional<Product> productMapValue;
                productMapValue = Product();
                Product::s_loadFromJsonView(productMapValue.value(), productMapPair.second);
                getAllProductsResponse.m_products.value()[productMapPair.first] = productMapValue.value();
            }
        }
    }

    const char *GetAllProductsResponse::MODEL_NAME = "awstest#GetAllProductsResponse";

    Aws::Crt::String GetAllProductsResponse::GetModelName() const noexcept
    {
        return GetAllProductsResponse::MODEL_NAME;
    }

    Aws::Crt::ScopedResource<AbstractShapeBase> GetAllProductsResponse::s_allocateFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) noexcept
    {
        Aws::Crt::String payload = {stringView.begin(), stringView.end()};
        Aws::Crt::JsonObject jsonObject(payload);
        Aws::Crt::JsonView jsonView(jsonObject);

        Aws::Crt::ScopedResource<GetAllProductsResponse> shape(
            Aws::Crt::New<GetAllProductsResponse>(allocator), GetAllProductsResponse::s_customDeleter);
        shape->m_allocator = allocator;
        GetAllProductsResponse::s_loadFromJsonView(*shape, jsonView);
        auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
        return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
    }

    void GetAllProductsResponse::s_customDeleter(GetAllProductsResponse *shape) noexcept
    {
        AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
    }

    void GetAllProductsRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
    {
        (void)payloadObject;
    }

    void GetAllProductsRequest::s_loadFromJsonView(
        GetAllProductsRequest &getAllProductsRequest,
        const Aws::Crt::JsonView &jsonView) noexcept
    {
        (void)getAllProductsRequest;
        (void)jsonView;
    }

    const char *GetAllProductsRequest::MODEL_NAME = "awstest#GetAllProductsRequest";

    Aws::Crt::String GetAllProductsRequest::GetModelName() const noexcept { return GetAllProductsRequest::MODEL_NAME; }

    Aws::Crt::ScopedResource<AbstractShapeBase> GetAllProductsRequest::s_allocateFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) noexcept
    {
        Aws::Crt::String payload = {stringView.begin(), stringView.end()};
        Aws::Crt::JsonObject jsonObject(payload);
        Aws::Crt::JsonView jsonView(jsonObject);

        Aws::Crt::ScopedResource<GetAllProductsRequest> shape(
            Aws::Crt::New<GetAllProductsRequest>(allocator), GetAllProductsRequest::s_customDeleter);
        shape->m_allocator = allocator;
        GetAllProductsRequest::s_loadFromJsonView(*shape, jsonView);
        auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
        return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
    }

    void GetAllProductsRequest::s_customDeleter(GetAllProductsRequest *shape) noexcept
    {
        AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
    }

    void GetAllCustomersResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
    {
        if (m_customers.has_value())
        {
            Aws::Crt::JsonObject customerList;
            Aws::Crt::Vector<Aws::Crt::JsonObject> customerListJsonArray;
            for (const auto &customerListItem : m_customers.value())
            {
                Aws::Crt::JsonObject customerListJsonArrayItem;
                customerListItem.SerializeToJsonObject(customerListJsonArrayItem);
                customerListJsonArray.emplace_back(std::move(customerListJsonArrayItem));
            }
            customerList.AsArray(std::move(customerListJsonArray));
            payloadObject.WithObject("customers", std::move(customerList));
        }
    }

    void GetAllCustomersResponse::s_loadFromJsonView(
        GetAllCustomersResponse &getAllCustomersResponse,
        const Aws::Crt::JsonView &jsonView) noexcept
    {
        if (jsonView.ValueExists("customers"))
        {
            getAllCustomersResponse.m_customers = Aws::Crt::Vector<Customer>();
            for (const Aws::Crt::JsonView &customerListJsonView : jsonView.GetArray("customers"))
            {
                Aws::Crt::Optional<Customer> customerListItem;
                customerListItem = Customer();
                Customer::s_loadFromJsonView(customerListItem.value(), customerListJsonView);
                getAllCustomersResponse.m_customers.value().push_back(customerListItem.value());
            }
        }
    }

    const char *GetAllCustomersResponse::MODEL_NAME = "awstest#GetAllCustomersResponse";

    Aws::Crt::String GetAllCustomersResponse::GetModelName() const noexcept
    {
        return GetAllCustomersResponse::MODEL_NAME;
    }

    Aws::Crt::ScopedResource<AbstractShapeBase> GetAllCustomersResponse::s_allocateFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) noexcept
    {
        Aws::Crt::String payload = {stringView.begin(), stringView.end()};
        Aws::Crt::JsonObject jsonObject(payload);
        Aws::Crt::JsonView jsonView(jsonObject);

        Aws::Crt::ScopedResource<GetAllCustomersResponse> shape(
            Aws::Crt::New<GetAllCustomersResponse>(allocator), GetAllCustomersResponse::s_customDeleter);
        shape->m_allocator = allocator;
        GetAllCustomersResponse::s_loadFromJsonView(*shape, jsonView);
        auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
        return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
    }

    void GetAllCustomersResponse::s_customDeleter(GetAllCustomersResponse *shape) noexcept
    {
        AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
    }

    void GetAllCustomersRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
    {
        (void)payloadObject;
    }

    void GetAllCustomersRequest::s_loadFromJsonView(
        GetAllCustomersRequest &getAllCustomersRequest,
        const Aws::Crt::JsonView &jsonView) noexcept
    {
        (void)getAllCustomersRequest;
        (void)jsonView;
    }

    const char *GetAllCustomersRequest::MODEL_NAME = "awstest#GetAllCustomersRequest";

    Aws::Crt::String GetAllCustomersRequest::GetModelName() const noexcept
    {
        return GetAllCustomersRequest::MODEL_NAME;
    }

    Aws::Crt::ScopedResource<AbstractShapeBase> GetAllCustomersRequest::s_allocateFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) noexcept
    {
        Aws::Crt::String payload = {stringView.begin(), stringView.end()};
        Aws::Crt::JsonObject jsonObject(payload);
        Aws::Crt::JsonView jsonView(jsonObject);

        Aws::Crt::ScopedResource<GetAllCustomersRequest> shape(
            Aws::Crt::New<GetAllCustomersRequest>(allocator), GetAllCustomersRequest::s_customDeleter);
        shape->m_allocator = allocator;
        GetAllCustomersRequest::s_loadFromJsonView(*shape, jsonView);
        auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
        return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
    }

    void GetAllCustomersRequest::s_customDeleter(GetAllCustomersRequest *shape) noexcept
    {
        AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
    }

    void EchoMessageResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
    {
        if (m_message.has_value())
        {
            Aws::Crt::JsonObject messageDataValue;
            m_message.value().SerializeToJsonObject(messageDataValue);
            payloadObject.WithObject("message", std::move(messageDataValue));
        }
    }

    void EchoMessageResponse::s_loadFromJsonView(
        EchoMessageResponse &echoMessageResponse,
        const Aws::Crt::JsonView &jsonView) noexcept
    {
        if (jsonView.ValueExists("message"))
        {
            echoMessageResponse.m_message = MessageData();
            MessageData::s_loadFromJsonView(echoMessageResponse.m_message.value(), jsonView.GetJsonObject("message"));
        }
    }

    const char *EchoMessageResponse::MODEL_NAME = "awstest#EchoMessageResponse";

    Aws::Crt::String EchoMessageResponse::GetModelName() const noexcept { return EchoMessageResponse::MODEL_NAME; }

    Aws::Crt::ScopedResource<AbstractShapeBase> EchoMessageResponse::s_allocateFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) noexcept
    {
        Aws::Crt::String payload = {stringView.begin(), stringView.end()};
        Aws::Crt::JsonObject jsonObject(payload);
        Aws::Crt::JsonView jsonView(jsonObject);

        Aws::Crt::ScopedResource<EchoMessageResponse> shape(
            Aws::Crt::New<EchoMessageResponse>(allocator), EchoMessageResponse::s_customDeleter);
        shape->m_allocator = allocator;
        EchoMessageResponse::s_loadFromJsonView(*shape, jsonView);
        auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
        return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
    }

    void EchoMessageResponse::s_customDeleter(EchoMessageResponse *shape) noexcept
    {
        AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
    }

    void EchoMessageRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
    {
        if (m_message.has_value())
        {
            Aws::Crt::JsonObject messageDataValue;
            m_message.value().SerializeToJsonObject(messageDataValue);
            payloadObject.WithObject("message", std::move(messageDataValue));
        }
    }

    void EchoMessageRequest::s_loadFromJsonView(
        EchoMessageRequest &echoMessageRequest,
        const Aws::Crt::JsonView &jsonView) noexcept
    {
        if (jsonView.ValueExists("message"))
        {
            echoMessageRequest.m_message = MessageData();
            MessageData::s_loadFromJsonView(echoMessageRequest.m_message.value(), jsonView.GetJsonObject("message"));
        }
    }

    const char *EchoMessageRequest::MODEL_NAME = "awstest#EchoMessageRequest";

    Aws::Crt::String EchoMessageRequest::GetModelName() const noexcept { return EchoMessageRequest::MODEL_NAME; }

    Aws::Crt::ScopedResource<AbstractShapeBase> EchoMessageRequest::s_allocateFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) noexcept
    {
        Aws::Crt::String payload = {stringView.begin(), stringView.end()};
        Aws::Crt::JsonObject jsonObject(payload);
        Aws::Crt::JsonView jsonView(jsonObject);

        Aws::Crt::ScopedResource<EchoMessageRequest> shape(
            Aws::Crt::New<EchoMessageRequest>(allocator), EchoMessageRequest::s_customDeleter);
        shape->m_allocator = allocator;
        EchoMessageRequest::s_loadFromJsonView(*shape, jsonView);
        auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
        return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
    }

    void EchoMessageRequest::s_customDeleter(EchoMessageRequest *shape) noexcept
    {
        AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
    }

    void EchoStreamingResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
    {
        (void)payloadObject;
    }

    void EchoStreamingResponse::s_loadFromJsonView(
        EchoStreamingResponse &echoStreamingResponse,
        const Aws::Crt::JsonView &jsonView) noexcept
    {
        (void)echoStreamingResponse;
        (void)jsonView;
    }

    const char *EchoStreamingResponse::MODEL_NAME = "awstest#EchoStreamingResponse";

    Aws::Crt::String EchoStreamingResponse::GetModelName() const noexcept { return EchoStreamingResponse::MODEL_NAME; }

    Aws::Crt::ScopedResource<AbstractShapeBase> EchoStreamingResponse::s_allocateFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) noexcept
    {
        Aws::Crt::String payload = {stringView.begin(), stringView.end()};
        Aws::Crt::JsonObject jsonObject(payload);
        Aws::Crt::JsonView jsonView(jsonObject);

        Aws::Crt::ScopedResource<EchoStreamingResponse> shape(
            Aws::Crt::New<EchoStreamingResponse>(allocator), EchoStreamingResponse::s_customDeleter);
        shape->m_allocator = allocator;
        EchoStreamingResponse::s_loadFromJsonView(*shape, jsonView);
        auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
        return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
    }

    void EchoStreamingResponse::s_customDeleter(EchoStreamingResponse *shape) noexcept
    {
        AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
    }

    void EchoStreamingRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
    {
        (void)payloadObject;
    }

    void EchoStreamingRequest::s_loadFromJsonView(
        EchoStreamingRequest &echoStreamingRequest,
        const Aws::Crt::JsonView &jsonView) noexcept
    {
        (void)echoStreamingRequest;
        (void)jsonView;
    }

    const char *EchoStreamingRequest::MODEL_NAME = "awstest#EchoStreamingRequest";

    Aws::Crt::String EchoStreamingRequest::GetModelName() const noexcept { return EchoStreamingRequest::MODEL_NAME; }

    Aws::Crt::ScopedResource<AbstractShapeBase> EchoStreamingRequest::s_allocateFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) noexcept
    {
        Aws::Crt::String payload = {stringView.begin(), stringView.end()};
        Aws::Crt::JsonObject jsonObject(payload);
        Aws::Crt::JsonView jsonView(jsonObject);

        Aws::Crt::ScopedResource<EchoStreamingRequest> shape(
            Aws::Crt::New<EchoStreamingRequest>(allocator), EchoStreamingRequest::s_customDeleter);
        shape->m_allocator = allocator;
        EchoStreamingRequest::s_loadFromJsonView(*shape, jsonView);
        auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
        return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
    }

    void EchoStreamingRequest::s_customDeleter(EchoStreamingRequest *shape) noexcept
    {
        AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
    }

    void ServiceError::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
    {
        if (m_message.has_value())
        {
            payloadObject.WithString("message", m_message.value());
        }
        if (m_value.has_value())
        {
            payloadObject.WithString("value", m_value.value());
        }
    }

    void ServiceError::s_loadFromJsonView(ServiceError &serviceError, const Aws::Crt::JsonView &jsonView) noexcept
    {
        if (jsonView.ValueExists("message"))
        {
            serviceError.m_message = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("message"));
        }
        if (jsonView.ValueExists("value"))
        {
            serviceError.m_value = Aws::Crt::Optional<Aws::Crt::String>(jsonView.GetString("value"));
        }
    }

    const char *ServiceError::MODEL_NAME = "awstest#ServiceError";

    Aws::Crt::String ServiceError::GetModelName() const noexcept { return ServiceError::MODEL_NAME; }

    Aws::Crt::ScopedResource<OperationError> ServiceError::s_allocateFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) noexcept
    {
        Aws::Crt::String payload = {stringView.begin(), stringView.end()};
        Aws::Crt::JsonObject jsonObject(payload);
        Aws::Crt::JsonView jsonView(jsonObject);

        Aws::Crt::ScopedResource<ServiceError> shape(
            Aws::Crt::New<ServiceError>(allocator), ServiceError::s_customDeleter);
        shape->m_allocator = allocator;
        ServiceError::s_loadFromJsonView(*shape, jsonView);
        auto operationResponse = static_cast<OperationError *>(shape.release());
        return Aws::Crt::ScopedResource<OperationError>(operationResponse, OperationError::s_customDeleter);
    }

    void ServiceError::s_customDeleter(ServiceError *shape) noexcept
    {
        OperationError::s_customDeleter(static_cast<OperationError *>(shape));
    }

    void CauseServiceErrorResponse::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
    {
        (void)payloadObject;
    }

    void CauseServiceErrorResponse::s_loadFromJsonView(
        CauseServiceErrorResponse &causeServiceErrorResponse,
        const Aws::Crt::JsonView &jsonView) noexcept
    {
        (void)causeServiceErrorResponse;
        (void)jsonView;
    }

    const char *CauseServiceErrorResponse::MODEL_NAME = "awstest#CauseServiceErrorResponse";

    Aws::Crt::String CauseServiceErrorResponse::GetModelName() const noexcept
    {
        return CauseServiceErrorResponse::MODEL_NAME;
    }

    Aws::Crt::ScopedResource<AbstractShapeBase> CauseServiceErrorResponse::s_allocateFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) noexcept
    {
        Aws::Crt::String payload = {stringView.begin(), stringView.end()};
        Aws::Crt::JsonObject jsonObject(payload);
        Aws::Crt::JsonView jsonView(jsonObject);

        Aws::Crt::ScopedResource<CauseServiceErrorResponse> shape(
            Aws::Crt::New<CauseServiceErrorResponse>(allocator), CauseServiceErrorResponse::s_customDeleter);
        shape->m_allocator = allocator;
        CauseServiceErrorResponse::s_loadFromJsonView(*shape, jsonView);
        auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
        return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
    }

    void CauseServiceErrorResponse::s_customDeleter(CauseServiceErrorResponse *shape) noexcept
    {
        AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
    }

    void CauseServiceErrorRequest::SerializeToJsonObject(Aws::Crt::JsonObject &payloadObject) const noexcept
    {
        (void)payloadObject;
    }

    void CauseServiceErrorRequest::s_loadFromJsonView(
        CauseServiceErrorRequest &causeServiceErrorRequest,
        const Aws::Crt::JsonView &jsonView) noexcept
    {
        (void)causeServiceErrorRequest;
        (void)jsonView;
    }

    const char *CauseServiceErrorRequest::MODEL_NAME = "awstest#CauseServiceErrorRequest";

    Aws::Crt::String CauseServiceErrorRequest::GetModelName() const noexcept
    {
        return CauseServiceErrorRequest::MODEL_NAME;
    }

    Aws::Crt::ScopedResource<AbstractShapeBase> CauseServiceErrorRequest::s_allocateFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) noexcept
    {
        Aws::Crt::String payload = {stringView.begin(), stringView.end()};
        Aws::Crt::JsonObject jsonObject(payload);
        Aws::Crt::JsonView jsonView(jsonObject);

        Aws::Crt::ScopedResource<CauseServiceErrorRequest> shape(
            Aws::Crt::New<CauseServiceErrorRequest>(allocator), CauseServiceErrorRequest::s_customDeleter);
        shape->m_allocator = allocator;
        CauseServiceErrorRequest::s_loadFromJsonView(*shape, jsonView);
        auto operationResponse = static_cast<AbstractShapeBase *>(shape.release());
        return Aws::Crt::ScopedResource<AbstractShapeBase>(operationResponse, AbstractShapeBase::s_customDeleter);
    }

    void CauseServiceErrorRequest::s_customDeleter(CauseServiceErrorRequest *shape) noexcept
    {
        AbstractShapeBase::s_customDeleter(static_cast<AbstractShapeBase *>(shape));
    }

    GetAllProductsOperationContext::GetAllProductsOperationContext(const EchoTestRpcServiceModel &serviceModel) noexcept
        : OperationModelContext(serviceModel)
    {
    }

    Aws::Crt::ScopedResource<AbstractShapeBase> GetAllProductsOperationContext::AllocateInitialResponseFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) const noexcept
    {
        return GetAllProductsResponse::s_allocateFromPayload(stringView, allocator);
    }

    Aws::Crt::ScopedResource<AbstractShapeBase> GetAllProductsOperationContext::AllocateStreamingResponseFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) const noexcept
    {
        (void)stringView;
        (void)allocator;
        return nullptr;
    }

    Aws::Crt::String GetAllProductsOperationContext::GetRequestModelName() const noexcept
    {
        return Aws::Crt::String("awstest#GetAllProductsRequest");
    }

    Aws::Crt::String GetAllProductsOperationContext::GetInitialResponseModelName() const noexcept
    {
        return Aws::Crt::String("awstest#GetAllProductsResponse");
    }

    Aws::Crt::Optional<Aws::Crt::String> GetAllProductsOperationContext::GetStreamingResponseModelName() const noexcept
    {
        return Aws::Crt::Optional<Aws::Crt::String>();
    }

    Aws::Crt::String GetAllProductsOperationContext::GetOperationName() const noexcept
    {
        return Aws::Crt::String("awstest#GetAllProducts");
    }

    std::future<GetAllProductsResult> GetAllProductsOperation::GetResult() noexcept
    {
        return std::async(std::launch::deferred, [this]() { return GetAllProductsResult(GetOperationResult().get()); });
    }

    GetAllProductsOperation::GetAllProductsOperation(
        ClientConnection &connection,
        const GetAllProductsOperationContext &operationContext,
        Aws::Crt::Allocator *allocator) noexcept
        : ClientOperation(connection, nullptr, operationContext, allocator)
    {
    }

    std::future<RpcError> GetAllProductsOperation::Activate(
        const GetAllProductsRequest &request,
        OnMessageFlushCallback onMessageFlushCallback) noexcept
    {
        return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
    }

    Aws::Crt::String GetAllProductsOperation::GetModelName() const noexcept
    {
        return m_operationModelContext.GetOperationName();
    }

    CauseServiceErrorOperationContext::CauseServiceErrorOperationContext(
        const EchoTestRpcServiceModel &serviceModel) noexcept
        : OperationModelContext(serviceModel)
    {
    }

    Aws::Crt::ScopedResource<AbstractShapeBase> CauseServiceErrorOperationContext::AllocateInitialResponseFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) const noexcept
    {
        return CauseServiceErrorResponse::s_allocateFromPayload(stringView, allocator);
    }

    Aws::Crt::ScopedResource<AbstractShapeBase> CauseServiceErrorOperationContext::AllocateStreamingResponseFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) const noexcept
    {
        (void)stringView;
        (void)allocator;
        return nullptr;
    }

    Aws::Crt::String CauseServiceErrorOperationContext::GetRequestModelName() const noexcept
    {
        return Aws::Crt::String("awstest#CauseServiceErrorRequest");
    }

    Aws::Crt::String CauseServiceErrorOperationContext::GetInitialResponseModelName() const noexcept
    {
        return Aws::Crt::String("awstest#CauseServiceErrorResponse");
    }

    Aws::Crt::Optional<Aws::Crt::String> CauseServiceErrorOperationContext::GetStreamingResponseModelName() const
        noexcept
    {
        return Aws::Crt::Optional<Aws::Crt::String>();
    }

    Aws::Crt::String CauseServiceErrorOperationContext::GetOperationName() const noexcept
    {
        return Aws::Crt::String("awstest#CauseServiceError");
    }

    std::future<CauseServiceErrorResult> CauseServiceErrorOperation::GetResult() noexcept
    {
        return std::async(
            std::launch::deferred, [this]() { return CauseServiceErrorResult(GetOperationResult().get()); });
    }

    CauseServiceErrorOperation::CauseServiceErrorOperation(
        ClientConnection &connection,
        const CauseServiceErrorOperationContext &operationContext,
        Aws::Crt::Allocator *allocator) noexcept
        : ClientOperation(connection, nullptr, operationContext, allocator)
    {
    }

    std::future<RpcError> CauseServiceErrorOperation::Activate(
        const CauseServiceErrorRequest &request,
        OnMessageFlushCallback onMessageFlushCallback) noexcept
    {
        return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
    }

    Aws::Crt::String CauseServiceErrorOperation::GetModelName() const noexcept
    {
        return m_operationModelContext.GetOperationName();
    }

    void CauseStreamServiceToErrorStreamHandler::OnStreamEvent(Aws::Crt::ScopedResource<AbstractShapeBase> response)
    {
        OnStreamEvent(static_cast<EchoStreamingMessage *>(response.get()));
    }

    bool CauseStreamServiceToErrorStreamHandler::OnStreamError(
        Aws::Crt::ScopedResource<OperationError> operationError,
        RpcError rpcError)
    {
        bool streamShouldTerminate = false;
        if (rpcError.baseStatus != EVENT_STREAM_RPC_SUCCESS)
        {
            streamShouldTerminate = OnStreamError(rpcError);
        }
        if (operationError != nullptr && operationError->GetModelName() == Aws::Crt::String("awstest#ServiceError") &&
            !streamShouldTerminate)
        {
            streamShouldTerminate = OnStreamError(static_cast<ServiceError *>(operationError.get()));
        }
        if (operationError != nullptr && !streamShouldTerminate)
            streamShouldTerminate = OnStreamError(operationError.get());
        return streamShouldTerminate;
    }

    CauseStreamServiceToErrorOperationContext::CauseStreamServiceToErrorOperationContext(
        const EchoTestRpcServiceModel &serviceModel) noexcept
        : OperationModelContext(serviceModel)
    {
    }

    Aws::Crt::ScopedResource<AbstractShapeBase> CauseStreamServiceToErrorOperationContext::
        AllocateInitialResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator) const
        noexcept
    {
        return EchoStreamingResponse::s_allocateFromPayload(stringView, allocator);
    }

    Aws::Crt::ScopedResource<AbstractShapeBase> CauseStreamServiceToErrorOperationContext::
        AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator) const
        noexcept
    {
        return EchoStreamingMessage::s_allocateFromPayload(stringView, allocator);
    }

    Aws::Crt::String CauseStreamServiceToErrorOperationContext::GetRequestModelName() const noexcept
    {
        return Aws::Crt::String("awstest#EchoStreamingRequest");
    }

    Aws::Crt::String CauseStreamServiceToErrorOperationContext::GetInitialResponseModelName() const noexcept
    {
        return Aws::Crt::String("awstest#EchoStreamingResponse");
    }

    Aws::Crt::Optional<Aws::Crt::String> CauseStreamServiceToErrorOperationContext::GetStreamingResponseModelName()
        const noexcept
    {
        return Aws::Crt::String("awstest#EchoStreamingMessage");
    }

    Aws::Crt::String CauseStreamServiceToErrorOperationContext::GetOperationName() const noexcept
    {
        return Aws::Crt::String("awstest#CauseStreamServiceToError");
    }

    std::future<CauseStreamServiceToErrorResult> CauseStreamServiceToErrorOperation::GetResult() noexcept
    {
        return std::async(
            std::launch::deferred, [this]() { return CauseStreamServiceToErrorResult(GetOperationResult().get()); });
    }

    CauseStreamServiceToErrorOperation::CauseStreamServiceToErrorOperation(
        ClientConnection &connection,
        CauseStreamServiceToErrorStreamHandler *streamHandler,
        const CauseStreamServiceToErrorOperationContext &operationContext,
        Aws::Crt::Allocator *allocator) noexcept
        : ClientOperation(connection, streamHandler, operationContext, allocator)
    {
    }

    std::future<RpcError> CauseStreamServiceToErrorOperation::Activate(
        const EchoStreamingRequest &request,
        OnMessageFlushCallback onMessageFlushCallback) noexcept
    {
        return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
    }

    Aws::Crt::String CauseStreamServiceToErrorOperation::GetModelName() const noexcept
    {
        return m_operationModelContext.GetOperationName();
    }

    void EchoStreamMessagesStreamHandler::OnStreamEvent(Aws::Crt::ScopedResource<AbstractShapeBase> response)
    {
        OnStreamEvent(static_cast<EchoStreamingMessage *>(response.get()));
    }

    bool EchoStreamMessagesStreamHandler::OnStreamError(
        Aws::Crt::ScopedResource<OperationError> operationError,
        RpcError rpcError)
    {
        bool streamShouldTerminate = false;
        if (rpcError.baseStatus != EVENT_STREAM_RPC_SUCCESS)
        {
            streamShouldTerminate = OnStreamError(rpcError);
        }
        if (operationError != nullptr && !streamShouldTerminate)
            streamShouldTerminate = OnStreamError(operationError.get());
        return streamShouldTerminate;
    }

    EchoStreamMessagesOperationContext::EchoStreamMessagesOperationContext(
        const EchoTestRpcServiceModel &serviceModel) noexcept
        : OperationModelContext(serviceModel)
    {
    }

    Aws::Crt::ScopedResource<AbstractShapeBase> EchoStreamMessagesOperationContext::AllocateInitialResponseFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) const noexcept
    {
        return EchoStreamingResponse::s_allocateFromPayload(stringView, allocator);
    }

    Aws::Crt::ScopedResource<AbstractShapeBase> EchoStreamMessagesOperationContext::
        AllocateStreamingResponseFromPayload(Aws::Crt::StringView stringView, Aws::Crt::Allocator *allocator) const
        noexcept
    {
        return EchoStreamingMessage::s_allocateFromPayload(stringView, allocator);
    }

    Aws::Crt::String EchoStreamMessagesOperationContext::GetRequestModelName() const noexcept
    {
        return Aws::Crt::String("awstest#EchoStreamingRequest");
    }

    Aws::Crt::String EchoStreamMessagesOperationContext::GetInitialResponseModelName() const noexcept
    {
        return Aws::Crt::String("awstest#EchoStreamingResponse");
    }

    Aws::Crt::Optional<Aws::Crt::String> EchoStreamMessagesOperationContext::GetStreamingResponseModelName() const
        noexcept
    {
        return Aws::Crt::String("awstest#EchoStreamingMessage");
    }

    Aws::Crt::String EchoStreamMessagesOperationContext::GetOperationName() const noexcept
    {
        return Aws::Crt::String("awstest#EchoStreamMessages");
    }

    std::future<EchoStreamMessagesResult> EchoStreamMessagesOperation::GetResult() noexcept
    {
        return std::async(
            std::launch::deferred, [this]() { return EchoStreamMessagesResult(GetOperationResult().get()); });
    }

    EchoStreamMessagesOperation::EchoStreamMessagesOperation(
        ClientConnection &connection,
        EchoStreamMessagesStreamHandler *streamHandler,
        const EchoStreamMessagesOperationContext &operationContext,
        Aws::Crt::Allocator *allocator) noexcept
        : ClientOperation(connection, streamHandler, operationContext, allocator)
    {
    }

    std::future<RpcError> EchoStreamMessagesOperation::Activate(
        const EchoStreamingRequest &request,
        OnMessageFlushCallback onMessageFlushCallback) noexcept
    {
        return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
    }

    Aws::Crt::String EchoStreamMessagesOperation::GetModelName() const noexcept
    {
        return m_operationModelContext.GetOperationName();
    }

    EchoMessageOperationContext::EchoMessageOperationContext(const EchoTestRpcServiceModel &serviceModel) noexcept
        : OperationModelContext(serviceModel)
    {
    }

    Aws::Crt::ScopedResource<AbstractShapeBase> EchoMessageOperationContext::AllocateInitialResponseFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) const noexcept
    {
        return EchoMessageResponse::s_allocateFromPayload(stringView, allocator);
    }

    Aws::Crt::ScopedResource<AbstractShapeBase> EchoMessageOperationContext::AllocateStreamingResponseFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) const noexcept
    {
        (void)stringView;
        (void)allocator;
        return nullptr;
    }

    Aws::Crt::String EchoMessageOperationContext::GetRequestModelName() const noexcept
    {
        return Aws::Crt::String("awstest#EchoMessageRequest");
    }

    Aws::Crt::String EchoMessageOperationContext::GetInitialResponseModelName() const noexcept
    {
        return Aws::Crt::String("awstest#EchoMessageResponse");
    }

    Aws::Crt::Optional<Aws::Crt::String> EchoMessageOperationContext::GetStreamingResponseModelName() const noexcept
    {
        return Aws::Crt::Optional<Aws::Crt::String>();
    }

    Aws::Crt::String EchoMessageOperationContext::GetOperationName() const noexcept
    {
        return Aws::Crt::String("awstest#EchoMessage");
    }

    std::future<EchoMessageResult> EchoMessageOperation::GetResult() noexcept
    {
        return std::async(std::launch::deferred, [this]() { return EchoMessageResult(GetOperationResult().get()); });
    }

    EchoMessageOperation::EchoMessageOperation(
        ClientConnection &connection,
        const EchoMessageOperationContext &operationContext,
        Aws::Crt::Allocator *allocator) noexcept
        : ClientOperation(connection, nullptr, operationContext, allocator)
    {
    }

    std::future<RpcError> EchoMessageOperation::Activate(
        const EchoMessageRequest &request,
        OnMessageFlushCallback onMessageFlushCallback) noexcept
    {
        return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
    }

    Aws::Crt::String EchoMessageOperation::GetModelName() const noexcept
    {
        return m_operationModelContext.GetOperationName();
    }

    GetAllCustomersOperationContext::GetAllCustomersOperationContext(
        const EchoTestRpcServiceModel &serviceModel) noexcept
        : OperationModelContext(serviceModel)
    {
    }

    Aws::Crt::ScopedResource<AbstractShapeBase> GetAllCustomersOperationContext::AllocateInitialResponseFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) const noexcept
    {
        return GetAllCustomersResponse::s_allocateFromPayload(stringView, allocator);
    }

    Aws::Crt::ScopedResource<AbstractShapeBase> GetAllCustomersOperationContext::AllocateStreamingResponseFromPayload(
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) const noexcept
    {
        (void)stringView;
        (void)allocator;
        return nullptr;
    }

    Aws::Crt::String GetAllCustomersOperationContext::GetRequestModelName() const noexcept
    {
        return Aws::Crt::String("awstest#GetAllCustomersRequest");
    }

    Aws::Crt::String GetAllCustomersOperationContext::GetInitialResponseModelName() const noexcept
    {
        return Aws::Crt::String("awstest#GetAllCustomersResponse");
    }

    Aws::Crt::Optional<Aws::Crt::String> GetAllCustomersOperationContext::GetStreamingResponseModelName() const noexcept
    {
        return Aws::Crt::Optional<Aws::Crt::String>();
    }

    Aws::Crt::String GetAllCustomersOperationContext::GetOperationName() const noexcept
    {
        return Aws::Crt::String("awstest#GetAllCustomers");
    }

    std::future<GetAllCustomersResult> GetAllCustomersOperation::GetResult() noexcept
    {
        return std::async(
            std::launch::deferred, [this]() { return GetAllCustomersResult(GetOperationResult().get()); });
    }

    GetAllCustomersOperation::GetAllCustomersOperation(
        ClientConnection &connection,
        const GetAllCustomersOperationContext &operationContext,
        Aws::Crt::Allocator *allocator) noexcept
        : ClientOperation(connection, nullptr, operationContext, allocator)
    {
    }

    std::future<RpcError> GetAllCustomersOperation::Activate(
        const GetAllCustomersRequest &request,
        OnMessageFlushCallback onMessageFlushCallback) noexcept
    {
        return ClientOperation::Activate(static_cast<const AbstractShapeBase *>(&request), onMessageFlushCallback);
    }

    Aws::Crt::String GetAllCustomersOperation::GetModelName() const noexcept
    {
        return m_operationModelContext.GetOperationName();
    }

    EchoTestRpcServiceModel::EchoTestRpcServiceModel() noexcept
        : m_getAllProductsOperationContext(*this), m_causeServiceErrorOperationContext(*this),
          m_causeStreamServiceToErrorOperationContext(*this), m_echoStreamMessagesOperationContext(*this),
          m_echoMessageOperationContext(*this), m_getAllCustomersOperationContext(*this)
    {
    }

    Aws::Crt::ScopedResource<OperationError> EchoTestRpcServiceModel::AllocateOperationErrorFromPayload(
        const Aws::Crt::String &errorModelName,
        Aws::Crt::StringView stringView,
        Aws::Crt::Allocator *allocator) const noexcept
    {
        auto it = m_modelNameToErrorResponse.find(errorModelName);
        if (it == m_modelNameToErrorResponse.end())
        {
            return nullptr;
        }
        else
        {
            return it->second(stringView, allocator);
        }
    }

    void EchoTestRpcServiceModel::AssignModelNameToErrorResponse(
        Aws::Crt::String modelName,
        ErrorResponseFactory factory) noexcept
    {
        m_modelNameToErrorResponse[modelName] = factory;
    }
} // namespace Awstest
