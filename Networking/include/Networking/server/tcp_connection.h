#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <queue>

namespace networking
{
    using MessageHandler = std::function<void(std::string)>;
    using ErrorHandler = std::function<void()>;

    class TCPConnection : public std::enable_shared_from_this<TCPConnection>
    {
    public:
        using pointer = std::shared_ptr<TCPConnection>;
        static pointer Create(boost::asio::ip::tcp::socket &&socket)
        {
            return pointer(new TCPConnection(std::move(socket)));
        }

        inline const std::string &GetUserName() const { return _username; }
        boost::asio::ip::tcp::socket &Socket()
        {
            return _socket;
        }

        void Start(MessageHandler &&messageHandler, ErrorHandler &&errorHandler);
        void Post(const std::string &message);

    private:
        explicit TCPConnection(boost::asio::ip::tcp::socket &&socket);

        void asyncRead();
        void onRead(boost::system::error_code ec, size_t bytesTransferred);

        void asyncWrite();
        void onWrite(boost::system::error_code ec, size_t bytesTransferred);

    private:
        boost::asio::ip::tcp::socket _socket;
        std::string _username;
        std::queue<std::string> _outgoingMessages;
        boost::asio::streambuf _streamBuf{65536};

        MessageHandler _messageHandler;
        ErrorHandler _errorHandler;
    };
}
