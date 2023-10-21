#include <Networking/server/tcp_connection.h>
#include <boost/asio.hpp>
#include <iostream>

namespace networking
{

    TCPConnection::TCPConnection(boost::asio::ip::tcp::socket &&socket) : _socket(std::move(socket))
    {
        boost::system::error_code ec;
        std::stringstream name;
        name << _socket.remote_endpoint();

        _username = name.str();
    }

    void networking::TCPConnection::Start(MessageHandler &&messageHandler, ErrorHandler &&errorHandler)
    {
        _messageHandler = std::move(messageHandler);
        _errorHandler = std::move(errorHandler);
        asyncRead();
    }

    void networking::TCPConnection::Post(const std::string &message)
    {

        bool queueIdle = _outgoingMessages.empty();
        _outgoingMessages.push(message);

        if (queueIdle)
        {
            asyncWrite();
        }
    }

    void TCPConnection::asyncRead()
    {
        boost::asio::async_read_until(_socket, _streamBuf, "\n", [self = shared_from_this()](boost::system::error_code ec, size_t bytesTransferred)
                                      {
                                          self->onRead(ec, bytesTransferred);
                                          //
                                      });
    }

    void TCPConnection::onRead(boost::system::error_code ec, size_t bytesTransferred)
    {
        if (ec)
        {
            _socket.close(ec);

            _errorHandler();
            return;
        }

        std::stringstream message;
        message << _username << ": " << std::istream(&_streamBuf).rdbuf();
        _streamBuf.consume(bytesTransferred);
        // std::cout << message.str();

        _messageHandler(message.str());
        asyncRead();
    }
    void TCPConnection::asyncWrite()
    {
        boost::asio::async_write(_socket, boost::asio::buffer(_outgoingMessages.front()), [self = shared_from_this()](boost::system::error_code ec, size_t bytesTransferred)
                                 {
                                     self->onWrite(ec, bytesTransferred);
                                     //
                                 });
    }

    void TCPConnection::onWrite(boost::system::error_code ec, size_t bytesTransferred)
    {
        if (ec)
        {
            _socket.close(ec);
            _errorHandler();
            return;
        }

        _outgoingMessages.pop();
        if (!_outgoingMessages.empty())
        {
            asyncWrite();
        }
    }

}
