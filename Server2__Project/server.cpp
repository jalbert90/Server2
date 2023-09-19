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

		// Cast pointer to struct sockaddr to pointer to sockaddr_in.
		in_addr = (struct sockaddr_in*)result->ai_addr;

		std::ostringstream os;
		os << "Listening on ...\n"
			<< "Address: " << inet_ntop(result->ai_family, &in_addr->sin_addr, buf, sizeof(buf)) << "\n"		// Convert ip to printable format (PCSTR)
			<< "Port: " << ntohs(in_addr->sin_port) << "\n";

		log(os.str());

		while (true)
		{
			acceptConnection(connectSocket1);
			handleConnection(connectSocket1);
		}
	}

	void Server::acceptConnection(SOCKET &connectSocket)
	{
		log("Waiting for connection...");
		connectSocket = accept(listenSocket, NULL, NULL);
		if (connectSocket == INVALID_SOCKET)
		{
			closeServer();
			exitWithError("Failed to accept connection:", WSAGetLastError());
		}
		log("Connection established");
	}

	void Server::handleConnection(SOCKET &connectSocket)
	{
		const int recvBufLen = 32768;
		char* recvBuf = new char[recvBufLen] ();
		int bytesRec, bytesSent;

		bytesRec = recv(connectSocket, recvBuf, recvBufLen, 0);
		if (bytesRec < 0)
		{
			closeServer();
			exitWithError("Error receiving data from client", WSAGetLastError());
		}
		else if (bytesRec == 0)
		{
			log("Connection closed...");
		}
		else
		{
			std::ostringstream oss;
			oss << "Received " << bytesRec << " bytes";
			log(oss.str());
			oss.clear();
			oss.str("");

			std::string sendMessage = buildResponse();
			bytesSent = send(connectSocket, sendMessage.c_str(), (int)size(sendMessage), 0);
			if (bytesSent == SOCKET_ERROR)
			{
				closeServer();
				exitWithError("Failed to send message", WSAGetLastError());
			}
			oss << "Sent " << bytesSent << " bytes";
			log(oss.str());
		}

		log("Connection closed\n");
		closesocket(connectSocket);
		delete[] recvBuf;
	}

	std::string Server::buildResponse()
	{
		// fopen(), fseek(), ftell()
		// Status line CRLF= \r\n
		// Headers
		// Body

		// Bare minimum
		/*
			HTTP/1.1 200 OK
			Content-Length: 12
			Content-Type: text/plain; charset=utf-8

			Hello World!
		*/

		std::ostringstream oss;
		std::string html = "<!DOCTYPE HTML PUBLIC><html><body><h1> hello browser </h1></body></html>";
		oss << "HTTP/1.1 200 OK\nContent-Length: " << html.size() << "\nContent-Type: text/html\n\n" << html;
		return oss.str();
	}

	void Server::closeServer()
	{
		freeaddrinfo(result);
		closesocket(listenSocket);
		closesocket(connectSocket1);
		WSACleanup();
		// If I exit here then the destructor won't finish.
	}
}