#pragma once

#include "socket_info.hpp"
#include "socket_message.hpp"
#include <thread>

namespace client_ns {

class ServerHandler {
    private:
        int socketFd;
        void handleMessage(socket_message_ns::SocketMessage msgType);
    public:
        ServerHandler();
        void setSocketFd(int newSocketFd);
        void handleServer();
};

class Client
{
public:
	Client();
	bool connectServer();
	void start();

private:
	int socketfd;
    ServerHandler serverHandler;
    std::thread serverHandlerThread;
};

}