#include <aws/greengrass/GreengrassCoreIpcClient.h>

#include <sstream>

#if defined(_MSC_VER)
#    pragma warning(disable : 4996)
#endif

namespace Aws
{
    namespace Greengrass
    {
        DefaultConnectionConfig::DefaultConnectionConfig() noexcept
        {
            const char *ipcSocketCStr = std::getenv("AWS_GG_NUCLEUS_DOMAIN_SOCKET_FILEPATH_FOR_COMPONENT");
            if (ipcSocketCStr != nullptr)
            {
                m_hostName = Crt::String(ipcSocketCStr);
            }

            const char *authTokenCStr = std::getenv("SVCUID");
            if (authTokenCStr != nullptr)
            {
                /* Encode authToken as JSON. */
                Crt::StringStream authTokenPayloadSS;
                authTokenPayloadSS << "{\"authToken\":\"" << Crt::String(authTokenCStr) << "\"}";
                m_connectAmendment = MessageAmendment(Crt::ByteBufFromCString(authTokenPayloadSS.str().c_str()));
            }

            m_port = static_cast<uint16_t>(0);
            Crt::Io::SocketOptions socketOptions;
            socketOptions.SetSocketDomain(Crt::Io::SocketDomain::Local);
            socketOptions.SetSocketType(Crt::Io::SocketType::Stream);
            m_socketOptions = std::move(socketOptions);
        }
    } // namespace Greengrass
} // namespace Aws
