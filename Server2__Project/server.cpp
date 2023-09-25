#include "server.h"

#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>

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

	void logError(std::string errorMessage, int errorCode)
	{
		log("Error Message: " + errorMessage + "\nError Code: " + std::to_string(errorCode));
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
		int bytesRec;

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

			std::string requestLine = getRequestLine(recvBuf);
			log(requestLine);

			std::string serverMessage = selectResponse(requestLine);

			sendResponseTemp(connectSocket, serverMessage);
		}

		log("Connection closed\n");
		closesocket(connectSocket);
		delete[] recvBuf;
	}

	std::string Server::getRequestLine(char* recvBuf)
	{
		std::stringstream ss;
		std::string requestLine;

		ss << recvBuf;
		std::getline(ss, requestLine, '\n');

		return requestLine;
	}

	std::vector<std::string> Server::tokenize(std::string input, char delim)
	{
		std::vector<std::string> tokens;
		std::string token;
		std::stringstream ss;

		ss << input;

		while (std::getline(ss, token, delim))
		{
			tokens.push_back(token);
		}

		return tokens;
	}

	std::string Server::selectResponse(std::string requestLine)
	{
		/* This is what the request lines look like */
		// GET / HTTP/1.1
		// GET /myScript.js HTTP/1.1
		// GET /favicon.ico HTTP/1.1

		// Bare minimum response with body:
		/*
			HTTP/1.1 200 OK
			Content-Length: 12
			Content-Type: text/plain; charset=utf-8

			Hello World!
		*/

		// Response Structure:
		// Status_Line\r\n
		// Headers\r\n\r\n
		// Body

		std::vector<std::string> primaryTokens = tokenize(requestLine, ' ');
		std::vector<std::string> secondaryTokens = tokenize(primaryTokens[1], '.');
		std::vector<std::string> tertiaryTokens = tokenize(primaryTokens[1], '/');

		//for (auto& el : primaryTokens)
		//{
		//	std::cout << el << "\n";
		//}

		//for (auto& el : secondaryTokens)
		//{
		//	log(el);
		//}

		// Potential tools to read file:
		// fopen(), fseek(), ftell()

		std::string statusLine, contentType, fileName;

		// Send index.html
		if (primaryTokens[1] == "/")
		{
			statusLine = "HTTP/1.1 200 OK\r\n";
			contentType = "text/html";

			return buildResponse(statusLine, contentType, "index.html");
		}
		else
		{
			// Send file, if it is accessible.
			fileName = tertiaryTokens[1];
			std::ifstream f(fileName);
			if (f)
			{
				statusLine = "HTTP/1.1 200 OK\r\n";
				contentType = contentTypes[secondaryTokens[1]];

				return buildResponse(statusLine, contentType, fileName);
			}
			else // Otherwise, send the index again :shrug:
			{
				statusLine = "HTTP/1.1 200 OK\r\n";
				contentType = "text/html";

				//return buildResponse(statusLine, contentType, "index.html");
				return "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: keep-alive\r\n\r\n";
			}
		}
	}

	std::string Server::buildResponse(std::string statusLine, std::string contentType, std::string fileName)
	{
		std::string header, body, line;
		std::ostringstream oss;

		std::ifstream input;
		input.open(fileName);
		while (input)
		{
			std::getline(input, line);
			oss << line;
		}
		input.close();

		body = oss.str();
		oss.clear();
		oss.str("");

		oss << "Content-Length: " << body.size() << "\r\nContent-Type: " << contentType << "\r\n\r\n";
		header = oss.str();
		oss.clear();
		oss.str("");

		oss << statusLine << header << body;

		return oss.str();
	}

	void Server::sendResponseTemp(SOCKET& connectSocket, std::string response)
	{
		std::ostringstream oss;
		int bytesSent = 0;
		int totalBytesSent = 0;

		while (totalBytesSent < (int)size(response))
		{
			bytesSent = send(connectSocket, response.c_str(), (int)size(response), 0);
			if (bytesSent < 0)
			{
				log("Failed to send response, breaking");
				break;
			}
			totalBytesSent += bytesSent;

			if (totalBytesSent == (int)size(response))
			{
				oss << "Sent " << totalBytesSent << " bytes out of " << (int)size(response) << " bytes";
				log(oss.str());
			}
			else
			{
				log("Error sending response");
				oss << "Sent " << totalBytesSent << " bytes out of " << (int)size(response) << " bytes";
				log(oss.str());
			}
		}
	}

	void Server::sendResponse(SOCKET& connectSocket, std::string requestLine)
	{
		/* This is what the request lines look like */
		// GET / HTTP/1.1
		// GET /myScript.js HTTP/1.1
		// GET /favicon.ico HTTP/1.1

		// Bare minimum response with body:
		/*
			HTTP/1.1 200 OK
			Content-Length: 12
			Content-Type: text/plain; charset=utf-8

			Hello World!
		*/

		// Response Structure:
		// Status_Line\r\n
		// Headers\r\n\r\n
		// Body

		std::vector<std::string> primaryTokens = tokenize(requestLine, ' ');
		std::vector<std::string> secondaryTokens = tokenize(primaryTokens[1], '.');
		std::vector<std::string> tertiaryTokens = tokenize(primaryTokens[1], '/');

		std::string contentType, fileName;

		if (primaryTokens[1] == "/") // Send index.html
		{
			contentType = "text/html";

			sendTextFile(connectSocket, contentType, "index.html");
		}
		else if (tertiaryTokens[1] == "jpg") // Send binary file
		{
			contentType = contentTypes[secondaryTokens[1]];

			sendBinaryFile(connectSocket, contentType, fileName);
		}
		else // Send text file
		{
			contentType = contentTypes[secondaryTokens[1]];

			sendTextFile(connectSocket, contentType, fileName);
		}
	}

	void Server::sendTextFile(SOCKET& connectSocket, std::string contentType, std::string fileName)
	{
		//
	}

	void Server::sendBinaryFile(SOCKET& connectSocket, std::string contentType, std::string fileName)
	{
		//
	}

	int Server::sendData(SOCKET& connectSocket, const void* data, int dataLength)
	{
		const char* ptr = static_cast<const char*>(data);
		int bytesSent = 0;

		while (dataLength > 0)
		{
			bytesSent = send(connectSocket, ptr, dataLength, 0);
			if (bytesSent == SOCKET_ERROR)
			{
				logError("`sendData()` failed", WSAGetLastError());
				return -1;
			}
			ptr += bytesSent;
			dataLength -= bytesSent;
		}
		return 0;
	}

	int Server::sendString(SOCKET& connectSocket, std::string str)
	{
		return sendData(connectSocket, str.c_str(), str.length());
	}

	void Server::createContentTypeMap()
	{
		// type/substype

		contentTypes["html"] = "text/html";
		contentTypes["js"] = "text/js";
		contentTypes["jpg"] = "image/jpeg";
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