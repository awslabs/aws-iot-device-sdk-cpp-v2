#include <awstest/EchoTestRpcClient.h>

namespace Awstest
{
    DefaultConnectionConfig::DefaultConnectionConfig() noexcept
    {
        m_hostName = Aws::Crt::String("127.0.0.1");
        m_port = 8033;
        Aws::Crt::Io::SocketOptions socketOptions;
        socketOptions.SetSocketDomain(Aws::Crt::Io::SocketDomain::IPv4);
        socketOptions.SetSocketType(Aws::Crt::Io::SocketType::Stream);
        m_socketOptions = std::move(socketOptions);
        m_connectAmendment.AddHeader(
            EventStreamHeader(Aws::Crt::String("client-name"), Aws::Crt::String("accepted.testy_mc_testerson")));
    }
} // namespace Awstest
