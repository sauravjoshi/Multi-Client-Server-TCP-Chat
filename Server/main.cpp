#include <iostream>
#include <boost/asio.hpp>
#include <Networking/server/tcp_server.h>

using boost::asio::ip::tcp;

int main(int, char **)
{
    networking::TCPServer server{networking::IPV::V4, 1337};

    server.OnJoin = [](networking::TCPConnection::pointer server)
    {
        std::cout << "User has joined the server: " << server->GetUserName() << std::endl;
    };

    server.OnLeave = [](networking::TCPConnection::pointer server)
    {
        std::cout << "User has left the server: " << server->GetUserName() << std::endl;
    };

    server.OnClientMessage = [&server](const std::string &message)
    {
        server.Broadcast(message);
    };
    server.Run();
    return 0;
}
