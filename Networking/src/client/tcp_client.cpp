#include <Networking/client/tcp_client.h>

namespace networking
{

    TCPClient::TCPClient(const std::string &address, int port) : _socket(_ioContext)
    {
        boost::asio::ip::tcp::resolver resolver{_ioContext};
        _endpoints = resolver.resolve(address, std::to_string(port));
    }
    void TCPClient::Run()
    {
        boost::asio::async_connect(_socket, _endpoints, [this](boost::system::error_code ec, boost::asio::ip::tcp::endpoint ep)
                                   {
                                        if(!ec){
                                            asyncRead();
                                        }
                                        _ioContext.run(); });
    }

    void TCPClient::Stop()
    {
        boost::system::error_code ec;
        _socket.close(ec);

        if (ec)
        {
        }
    }

    void TCPClient::Post(const std::string &message)
    {
        bool queueIdle = _outgoingMessages.empty();
        _outgoingMessages.push(message);

        if (queueIdle)
        {
            asyncWrite();
        }
    }
    void TCPClient::asyncRead()
    {
        boost::asio::async_read_until(_socket, _streamBuf, "\n", [this](boost::system::error_code ec, size_t bytesTransferred)
                                      { onRead(ec, bytesTransferred); });
    }
    void TCPClient::onRead(boost::system::error_code ec, size_t bytesTransferred)
    {
        if (ec)
        {
            Stop();
            return;
        }

        std::stringstream message;
        message << std::istream(&_streamBuf).rdbuf();
        OnMessage(message.str());
        asyncRead();
    }
    void TCPClient::asyncWrite()
    {
        boost::asio::async_write(_socket, boost::asio::buffer(_outgoingMessages.front()), [this](boost::system::error_code ec, size_t bytesTransferred)
                                 {
                                     onWrite(ec, bytesTransferred);
                                     //
                                 });
    }
    void TCPClient::onWrite(boost::system::error_code ec, size_t bytesTransferred)
    {
        if (ec)
        {
            Stop();
            return;
        }

        _outgoingMessages.pop();
        if (!_outgoingMessages.empty())
        {
            asyncWrite();
        }
    }
}
