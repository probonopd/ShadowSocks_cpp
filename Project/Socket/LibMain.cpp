//
// Created by hackerl on 10/13/18.
//

#include "TCPSocket.h"
#include "Socket/ISocket.h"
#include "Socket/LibSocketExport.h"
#include "EventLoop/LibEventExport.h"

class CServer : public CTCPSocket , public ISocketAcceptCallback
{
    void OnAccpet(int fd ,short Event) override
    {
        int client = Accpet();
        std::cout << "Accpet" << std::endl;
    }
};

void TestServer()
{
    CServer Server;

    if (!Server.Bind("192.168.1.102", 3333))
        std::cout << "Bind Faild" << std::endl;

    if (!Server.Listen(255))
        std::cout << "Listen Faild" << std::endl;

    IEventLoop * Loop = NewEventLoop();

    Loop->AddServer(Server.GetSocket(), &Server);

    Loop->Loop();

    DeleteEventLoop(Loop);
}

void TestClient()
{
    ITCPSocket * Client = NewTCPSocket();

    do
    {
        if (!Client->Connect("127.0.0.1", 3333))
        {
            std::cout << "Connect Faild" << std::endl;
            break;
        }

        std::string data= "Hello";

        Client->Send(data.data(), data.length(), 0);

    } while (false);

    DeleteTCPSocket(Client);
}

class CClient : public CTCPSocket , public ISocketEventCallback
{
    void OnRead(int fd ,short Event) override
    {
        std::cout << "Can Read" << std::endl;

        char Buffer[1024];

        int len = Recv(Buffer, 1024, 0);
    }

    void OnWrite(int fd ,short Event) override
    {
        std::cout << "Can Write" << std::endl;
    }

    void OnClose(int fd ,short Event) override
    {
        std::cout << "Closed" << std::endl;
        m_Loop->RemoveEvent(fd);
    }

public:
    IEventLoop * m_Loop;
};

void TestEventCallback()
{
    CClient Client;

    do
    {
        if (!Client.Connect("127.0.0.1", 3333))
        {
            std::cout << "Connect Faild" << std::endl;
            break;
        }

        std::string data= "Hello";

        Client.Send(data.data(), data.length(), 0);

        IEventLoop * Loop = NewEventLoop();

        Client.m_Loop = Loop;

        Loop->AddClient(Client.GetSocket(), &Client);

        //Loop->AddServer(Server.GetSocket(), &Server);

        Loop->Loop();

        DeleteEventLoop(Loop);
    } while (false);
}

int main()
{
    TestEventCallback();
    return 0;
}