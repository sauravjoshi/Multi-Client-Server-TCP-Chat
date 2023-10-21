#pragma once
#include <iostream>
#include <optional>
#include <unordered_set>
#include <boost/asio.hpp>
#include <Networking/server/tcp_connection.h>

namespace networking
{
    enum class IPV
    {
        V4,
        V6
    };

    class TCPServer
    {
        using OnJoinHandler = std::function<void(TCPConnection::pointer)>;
        using OnLeaveHandler = std::function<void(TCPConnection::pointer)>;
        using OnClientMessageHandler = std::function<void(std::string)>;

    private:
        void startAccept();

    private:
        IPV _ipVersion;
        int _port;
        boost::asio::io_context _ioContext;
        boost::asio::ip::tcp::acceptor _acceptor;
        std::optional<boost::asio::ip::tcp::socket> _socket;
        std::unordered_set<TCPConnection::pointer> _connections{};

    public:
        OnJoinHandler OnJoin;
        OnLeaveHandler OnLeave;
        OnClientMessageHandler OnClientMessage;

    public:
        TCPServer(IPV ipv, int port);

        int Run();
        void Broadcast(const std::string &message);
    };
}
