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
		ZeroMemory(&hints, sizeof(hints));

		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;	// Use TCP
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		code = startServer();
		// Failed to start with port?
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

		// Pass the address of the result pointer.
		// The contents at this address are then modified to contain the address
		// of the first struct addrinfo item in a linked list.
		code = getaddrinfo(m_addr.c_str(), m_port.c_str(), &hints, &result);
		if (code != 0)
		{
			closeServer();
			exitWithError("getaddrinfo() failed", code);
		}

		// Pointer to sockaddr found in struct addrinfo.
		code = bind(listenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (code != 0)
		{
			closeServer();
			exitWithError("Failed to bind server to socket", WSAGetLastError());
		}

		return 0;
	}

	void Server::startListening()
	{
		code = listen(listenSocket, 20);
		if (code != 0)
		{
			closeServer();
			exitWithError("Couldn't start listening", WSAGetLastError());
		}


		// Comment this all up.
		// Also, add to notebook.
		in_addr = (struct sockaddr_in*)result->ai_addr;
		char buf[INET_ADDRSTRLEN];

		std::ostringstream os;
		os << "Listening on ...\n"
			<< "Address: " << inet_ntop(result->ai_family, &in_addr->sin_addr, buf, sizeof(buf)) << "\n"
			<< "Port: " << ntohs(in_addr->sin_port);

		// End comment this all up.

		log(os.str());
	}

	void Server::closeServer()
	{
		freeaddrinfo(result);
		closesocket(listenSocket);
		WSACleanup();
		// If I exit here then the destructor won't finish.
	}
}