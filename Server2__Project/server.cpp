#include "server.h"

#include <stdio.h>
#include <iostream>
#include <sstream>

namespace
{
	void log(std::string message)
	{
		std::cout << message << std::endl;
	}

	void exitWithError(std::string errorMessage, int errorCode)
	{
		std::stringstream ss;
		ss << errorCode;
		log("Error Message: " + errorMessage + "\nError Code: " + ss.str());
		exit(1);
	}
} // unnamed namespace

namespace N
{
	Server::Server(std::string addr, std::string port) : m_addr(addr), m_port(port)
	{
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		code = startServer();
	}

	Server::~Server()
	{
		closeServer();
	}

	int Server::startServer()
	{
		code = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (code != 0)
		{
			closeServer();
			exitWithError("WSAStartup failed", code);
			return 1;	// Should never get here.
		}

		listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (listenSocket == INVALID_SOCKET)
		{
			closeServer();
			exitWithError("Failed to create socket", WSAGetLastError());
			return 1;
		}

		return 0;
	}

	void Server::closeServer()
	{
		// free addrinfo
		closesocket(listenSocket);
		WSACleanup();
		// If I exit here then the destructor won't finish.
	}
}