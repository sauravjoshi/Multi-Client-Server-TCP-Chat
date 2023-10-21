#pragma once
#include <boost/asio.hpp>
#include <iostream>
#include <queue>

namespace networking
{
    using MessageHandler = std::function<void(std::string)>;

    class TCPClient
    {
    public:
        TCPClient(const std::string &address, int port);
        void Run();
        void Stop();
        void Post(const std::string &message);

    private:
        void asyncRead();
        void onRead(boost::system::error_code ec, size_t bytesTransferred);
        void asyncWrite();
        void onWrite(boost::system::error_code ec, size_t bytesTransferred);

    public:
        MessageHandler OnMessage;

    private:
        boost::asio::io_context _ioContext{};
        boost::asio::ip::tcp::socket _socket;
        boost::asio::ip::tcp::resolver::results_type _endpoints;

        boost::asio::streambuf _streamBuf{65536};
        std::queue<std::string> _outgoingMessages{};
    };
}
