#include "client.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
using namespace std;

namespace client_ns
{

struct timeval SocketWaitTime = {0, 10000};

bool serverHandlerFlag = false;
bool clientFlag = false;

void signalHandler(int signal)
{
	if(signal == SIGINT)
	{
		cout << "\nreceived SIGINT\n";
		clientFlag = true;
	}
}

ServerHandler::ServerHandler() { }

void ServerHandler::handleServer()
{
	socket_message_ns::SocketMessage msgType;
	fd_set readfds;
	while(serverHandlerFlag == false)
	{
		FD_ZERO(&readfds);
		FD_SET(socketFd, &readfds);

		int activity = select(socketFd + 1, &readfds, NULL, NULL, &SocketWaitTime);
		if(activity > 0 && FD_ISSET(socketFd, &readfds))
		{
			if(recv(socketFd, &msgType, sizeof(socket_message_ns::SocketMessage), 0) <= 0)
			{
				cout << "Server disconnected. Closing client." << endl;
				serverHandlerFlag = true;
				break;
			}
			handleMessage(msgType);
		}
	}
}

void ServerHandler::setSocketFd(int newSocketFd)
{
	socketFd = newSocketFd;
}

void ServerHandler::handleMessage(socket_message_ns::SocketMessage msgType)
{
	size_t msgLen;
	char buffer[1024] = {0};

	switch(msgType)
	{
	case socket_message_ns::TURN_WAIT:
		cout << "Wait for your turn..." << endl;
		break;
	case socket_message_ns::GAME_OVER_WIN:
		cout << "Game over! You win!" << endl;
		clientFlag = true;
		break;
	case socket_message_ns::GAME_OVER_DRAW:
		cout << "Game over! It's a draw!" << endl;
		clientFlag = true;
		break;
	case socket_message_ns::BOARD_STATE:
		// Receive message length and board state content
		if(recv(socketFd, &msgLen, sizeof(size_t), 0) <= 0)
		{
			cerr << "Could not read message length!" << endl;
			return;
		}
		if(recv(socketFd, buffer, msgLen, 0) <= 0)
		{
			cerr << "Could not read message content!" << endl;
			return;
		}
		buffer[msgLen] = '\0'; // Null-terminate the message
		cout << "Board State:\n" << buffer << endl;
		break;
	case socket_message_ns::INVALID_MOVE:
		cout << "Invalid move! Try again." << endl;
		break;
	case socket_message_ns::DISCONNECT:
		cout << "Server is disconnecting. Closing client." << endl;
		clientFlag = true;
		break;
	default:
		cerr << "Unknown message type received." << endl;
		break;
	}
}

Client::Client()
	: socketfd(-1)
{ }

bool Client::connectServer()
{
	struct sockaddr_in address;
	address.sin_port = htons(general::SocketPort);
	address.sin_family = AF_INET;
	inet_pton(AF_INET, general::IpAddress.c_str(), &address.sin_addr);

	// Use the class member socketfd
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(socketfd < 0)
	{
		cerr << "Error creating socket." << endl;
		return false;
	}

	cout << "Client socket : " << socketfd << endl;

	int result = connect(socketfd, (const sockaddr*)&address, sizeof(address));
	if(result < 0)
	{
		cerr << "Connection failed. Error: " << strerror(errno) << endl;
		close(socketfd);
		socketfd = -1; // Reset socketfd to invalid state
		return false;
	}

	cout << "Connected to the server successfully." << endl;
	return true;
}

void Client::start()
{
	serverHandler.setSocketFd(socketfd);
	serverHandlerThread = std::thread(&ServerHandler::handleServer, &serverHandler);

	signal(SIGINT, signalHandler);

	while(clientFlag == false)
	{
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(STDIN_FILENO, &readfds);

		int activity = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &SocketWaitTime);
		if(activity > 0 && FD_ISSET(STDIN_FILENO, &readfds))
		{
			int row, col;
			cout << "Enter your move (row col): ";
			cin >> row >> col;

			if(send(socketfd, &row, sizeof(int), 0) <= 0)
			{
				cerr << "Error sending row data. Error: " << strerror(errno) << endl;
			}
			if(send(socketfd, &col, sizeof(int), 0) <= 0)
			{
				cerr << "Error sending col data. Error: " << strerror(errno) << endl;
			}
		}
	}
	serverHandlerFlag = true;
	serverHandlerThread.join();
	close(socketfd);
	cout << "Client is finished successfully!\n";
}

} // namespace client_ns

int main()
{
	signal(SIGINT, client_ns::signalHandler);
	client_ns::Client client;

	if(!client.connectServer())
	{
		return 1;
	}

	client.start();
	return 0;
}