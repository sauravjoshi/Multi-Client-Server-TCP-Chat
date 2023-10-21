#include <boost/asio.hpp>
#include <iostream>
#include <Networking/server/tcp_server.h>
#include <Networking/server/tcp_connection.h>

namespace networking
{
    using boost::asio::ip::tcp;
    networking::TCPServer::TCPServer(IPV ipv, int port) : _ipVersion(ipv), _port(port), _acceptor(_ioContext, tcp::endpoint(ipv == IPV::V4 ? tcp::v4() : tcp::v6(), _port))
    {
    }

    int TCPServer::Run()
    {
        try
        {
            startAccept();
            _ioContext.run();
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
            return -1;
        }
        return 0;
    }

    void TCPServer::Broadcast(const std::string &message)
    {
        for (auto &connection : _connections)
        {
            connection->Post(message);
        }
    }
    void TCPServer::startAccept()
    {
        _socket.emplace(_ioContext);
        _acceptor.async_accept(*_socket, [this](const boost::system::error_code &error)
                               {
        auto connection = TCPConnection::Create(std::move(*_socket));
        if(OnJoin){
            OnJoin(connection);
        }
        _connections.insert(connection);
        if(!error){
            connection->Start([this](const std::string& message){
                if(OnClientMessage){
                    OnClientMessage(message);
                }
            },
            [&, weak = std::weak_ptr(connection)] {
                if(auto shared = weak.lock(); shared && _connections.erase(shared)){
                    if(OnLeave){
                        OnLeave(shared);
                    }
                }
            });
        }
        startAccept(); });
    }
}