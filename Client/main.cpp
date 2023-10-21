#include <iostream>
#include <Networking/client/tcp_client.h>
#include <thread>

using namespace networking;

int main(int, char **)
{
    try
    {
        TCPClient client{"localhost", 1337};

        client.OnMessage = [](const std::string &message)
        {
            std::cout << message;
        };

        std::thread t{[&client]()
                      { client.Run(); }};

        while (true)
        {
            std::string message;
            getline(std::cin, message);

            if (message == "\\q")
                break;
            message += "\n";

            client.Post(message);
        }

        client.Stop();
        t.join();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}
