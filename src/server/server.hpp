#pragma once

#include "socket_info.hpp"
#include "socket_message.hpp"
#include "ticTacToe.cpp"
#include <map>
#include <thread>

namespace server_ns {

// ClientHandler class for handling clients side
class ClientHandler {
    private:
        private:
        int clientSocket;
        int clientId;
        char playerSymbol;
        Game& game;
        Board& gameBoard;

        void sendMessage(int clientSocket,
                        socket_message_ns::SocketMessage msgType,
                        const string& messageContent = "");

        void broadcastBoardState();

        void broadcastMessage(socket_message_ns::SocketMessage msgType);

    public:
        ClientHandler(int socket, int id, char symbol, Game& game, Board& board);
        void handle();
};

// Server class for managing the server and client connections
class Server
{
public:
	Server();
    void start();
private:
    int serverSocket;
	int clientIdCounter;
	Game game;
	Board gameBoard;
    map<int, std::thread> mapClientsIdThread;

	void setupSocket();
    bool acceptNewClient();
    void sendMessage(int clientSocket,
					 socket_message_ns::SocketMessage msgType,
					 const string& messageContent = "");
    void closeServer();
};

}