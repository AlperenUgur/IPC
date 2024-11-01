#include "server.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

namespace server_ns
{

struct timeval SocketWaitTime = {5, 0};

// Global variables for flags
bool clientFlag = false;
bool serverFlag = false;
int currentTurn = 0;
map<int, int> mapClientsFdId; // Shared list of client IDs

// Function to handle SIGINT signal
void signalHandler(int signal)
{
	if(signal == SIGINT)
	{
		cout << "\nReceived SIGINT\n";
		serverFlag = true;
	}
}

ClientHandler::ClientHandler(int socket, int id, char symbol, Game& game, Board& board)
	: clientSocket(socket)
	, clientId(id)
	, playerSymbol(symbol)
	, game(game)
	, gameBoard(board)
{ }

void ClientHandler::handle()
{
	cout << "Client connected. ID: " << clientId << " as Player " << playerSymbol << endl;

	while(clientFlag == false)
	{
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(clientSocket, &readfds);
		int activity = select(clientSocket + 1, &readfds, NULL, NULL, &SocketWaitTime);
		
		if(activity > 0 && FD_ISSET(clientSocket, &readfds))
		{
			int row = 0, col = 0;
			ssize_t rowRecvSize = recv(clientSocket, &row, sizeof(int), 0);
			ssize_t colRecvSize = recv(clientSocket, &col, sizeof(int), 0);
			cout << "Received move: row = " << rowRecvSize << ", col = " << colRecvSize << endl;
			if(rowRecvSize <= 0 || colRecvSize <= 0)
			{
				cerr << "Failed to receive move data" << endl;
				close(clientSocket);  // Close the socket
            	clientFlag = true; 
				continue;
			}

			cout << "Received move: row = " << row << ", col = " << col << endl;
			if(clientId - 1 != currentTurn)
			{
				sendMessage(clientSocket, socket_message_ns::TURN_WAIT);
				continue;
			}

			if(gameBoard.move(row, col, playerSymbol))
			{
				broadcastBoardState();
				if(gameBoard.checkWin(playerSymbol))
				{
					broadcastMessage(socket_message_ns::GAME_OVER_WIN);
					serverFlag = true;
					break;
				}
				else if(gameBoard.isFull())
				{
					broadcastMessage(socket_message_ns::GAME_OVER_DRAW);
					serverFlag = true;
					break;
				}
				else
				{
					currentTurn = 1 - currentTurn;
				}
			}
			else
			{
				sendMessage(clientSocket, socket_message_ns::INVALID_MOVE);
			}
		}
	}

	close(clientSocket);
	cout << "Client handler is finished successfully.\n";
}

void ClientHandler::sendMessage(int clientSocket,
								socket_message_ns::SocketMessage msgType,
								const string& messageContent)
{
	size_t res = send(clientSocket, &msgType, sizeof(socket_message_ns::SocketMessage), 0);
	if(res <= 0)
	{
		cerr << "Sending message type is failed!" << endl;
	}
	else
	{
		cout << "Msg send size: " << res << endl;
	}
	if(!messageContent.empty())
	{
		size_t msgLen = messageContent.size();
		res = send(clientSocket, &msgLen, sizeof(size_t), 0);
		if(res <= 0)
		{
			cerr << "Sending message type is failed!" << endl;
		}
		else
		{
			cout << "Msg send size: " << res << endl;
		}
		res = send(clientSocket, messageContent.c_str(), msgLen, 0);
		if(res <= 0)
		{
			cerr << "Sending message type is failed!" << endl;
		}
		else
		{
			cout << "Msg send size: " << res << endl;
		}
	}
}

void ClientHandler::broadcastBoardState()
{
	string boardState = gameBoard.getBoardAsString();
	for(const auto& client : mapClientsFdId)
	{
		sendMessage(client.second, socket_message_ns::BOARD_STATE, boardState);
	}
}

void ClientHandler::broadcastMessage(socket_message_ns::SocketMessage msgType)
{
	for(const auto& client : mapClientsFdId)
	{
		sendMessage(client.second, msgType);
	}
}

Server::Server()
	: clientIdCounter(0)
{ }

void Server::start()
{
	setupSocket();
	cout << "Server is listening on port " << general::SocketPort << "..." << endl;
	signal(SIGINT, signalHandler);

	while(serverFlag == false)
	{
		if(acceptNewClient())
		{
			struct sockaddr_in clientAddress;
			socklen_t clientAddressLen = sizeof(clientAddress);
			int clientSocketfd =
				accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressLen);
			if(clientSocketfd < 0)
			{
				cerr << "Accept failed." << endl;
				continue;
			}
			clientIdCounter++;
			mapClientsFdId.insert({clientIdCounter, clientSocketfd});
			char playerSymbol = (clientIdCounter - 1 == 0) ? 'X' : 'O';

			ClientHandler clientHandler(
				clientSocketfd, clientIdCounter, playerSymbol, game, gameBoard);
			mapClientsIdThread.insert(
				{clientIdCounter, std::thread(&ClientHandler::handle, clientHandler)});
		}
	}
	closeServer();
	cout << "Server is finished successfully\n";
}

void Server::setupSocket()
{
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(serverSocket < 0)
	{
		cerr << "Error creating socket." << endl;
		exit(1);
	}

	struct sockaddr_in address;
	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(general::SocketPort);
	address.sin_addr.s_addr = inet_addr(general::IpAddress.c_str());

	if(bind(serverSocket, (const sockaddr*)&address, sizeof(address)) < 0)
	{
		cerr << "Bind failed." << endl;
		close(serverSocket);
		exit(1);
	}

	if(listen(serverSocket, 2) < 0)
	{
		cerr << "Listen failed." << endl;
		close(serverSocket);
		exit(1);
	}
}

bool Server::acceptNewClient()
{
	fd_set readfds;

	FD_ZERO(&readfds);
	FD_SET(serverSocket, &readfds);

	int activity = select(serverSocket + 1, &readfds, NULL, NULL, &SocketWaitTime);
	return (activity > 0 && FD_ISSET(serverSocket, &readfds));
}
void Server::sendMessage(int clientSocket,
						 socket_message_ns::SocketMessage msgType,
						 const string& messageContent)
{
	send(clientSocket, &msgType, sizeof(socket_message_ns::SocketMessage), 0);
	if(!messageContent.empty())
	{
		size_t msgLen = messageContent.size();
		send(clientSocket, &msgLen, sizeof(size_t), 0);
		send(clientSocket, messageContent.c_str(), msgLen, 0);
	}
}

void Server::closeServer()
{
	cout << "Sending disconnect command to all clients...\n";
	for(const auto& client : mapClientsFdId)
	{
		sendMessage(client.second,
					socket_message_ns::DISCONNECT); // Send DISCONNECT message
	}
	sleep(1);
	cout << "Closing all client socket's...\n";
	for(const auto& client : mapClientsFdId)
	{
		close(client.second); // Close client socket
	}
	cout << "Stopping all client thread's...\n";
	clientFlag = true;
	for(auto& client : mapClientsIdThread)
	{
		client.second.join();
	}
	close(serverSocket); // Close server socket
}

} // namespace server_ns

int main()
{
	server_ns::Server server;
	server.start();
	return 0;
}
