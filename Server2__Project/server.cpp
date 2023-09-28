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

		createContentTypeMap();

		code = startServer();
		// Error check...failed to start with port...something something
		// closeServer() ???
		// exitwitherror
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
			// change all of this to:
			// log
			// return -1
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

		// put in loop
		// waiting for message
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
			// clean this up
			std::ostringstream oss;
			oss << "Received " << bytesRec << " bytes";
			log(oss.str());
			oss.clear();
			oss.str("");

			std::string requestLine = getRequestLine(recvBuf);
			log(requestLine);

			sendResponse(connectSocket, requestLine);
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

	void Server::sendResponse(const SOCKET& connectSocket, const std::string& requestLine)
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
			sendFileAsBinary(connectSocket, contentType, "index.html");
		}
		else
		{
			fileName = tertiaryTokens[1];
			contentType = contentTypes[secondaryTokens[1]];
			sendFileAsBinary(connectSocket, contentType, fileName);
		}
	}

	void Server::sendFileAsBinary(const SOCKET& connectSocket, const std::string& contentType, const std::string& fileName)
	{
		if (!fileExists(fileName))
		{
			log(fileName+ " not found");
			if (sendString(connectSocket, "HTTP 1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: keep-alive\r\n\r\n") == -1)
			{
				closesocket(connectSocket);
			}
		}
		else
		{
			std::ifstream f(fileName.c_str(), std::ios::binary);
			if (!f.is_open())
			{
				log("Error opening " + fileName);
				if (sendString(connectSocket, "HTTP 1.1 500 Error\r\nContent-Length: 0\r\nConnection: keep-alive\r\n\r\n") == -1)
				{
					closesocket(connectSocket);
				}
			}
			else
			{
				f.seekg(0, std::ios::end);
				int fileLength = f.tellg();
				f.seekg(0, std::ios::beg);

				if (f.fail())
				{
					log("Failed to size " + fileName);
					if (sendString(connectSocket, "HTTP 1.1 500 Error\r\nContent-Length: 0\r\nConnection: keep-alive\r\n\r\n") == -1)
					{
						closesocket(connectSocket);
					}
				}
				else if (sendString(connectSocket, STATUS200 + "Content-Length: " + std::to_string(fileLength) + "\r\nContent-Type: " + contentType + "\r\nConnection: keep-alive\r\n\r\n") == -1)
				{
					closesocket(connectSocket);
				}
				else
				{
					char buf[1024];
					int totalBytesSent = 0;
					while (fileLength > 0)
					{
						if (!f.read(buf, min(sizeof(buf), fileLength)))
						{
							log("Failed to read " + fileName);
							if (sendString(connectSocket, "HTTP 1.1 500 Error\r\nContent-Length: 0\r\nConnection: keep-alive\r\n\r\n") == -1)
							{
								closesocket(connectSocket);
							}

							break;
						}

						int bytesRead = f.gcount();
						int bytesSent = sendData(connectSocket, buf, bytesRead);
						if (bytesSent == -1)
						{
							log("Failed to send " + fileName);
							if (sendString(connectSocket, "HTTP 1.1 500 Error\r\nContent-Length: 0\r\nConnection: keep-alive\r\n\r\n") == -1)
							{
								closesocket(connectSocket);
							}

							break;
						}

						fileLength -= bytesRead;
						totalBytesSent += bytesSent;
					}

					log("Sent " + std::to_string(totalBytesSent) + " bytes of binary in body");
				}
			}
		}
	}

	bool Server::fileExists(const std::string& fileName)
	{
		std::ifstream f(fileName.c_str());
		return f.good();
	}

	int Server::sendData(const SOCKET& connectSocket, const void* data, int dataLength)
	{
		const char* ptr = static_cast<const char*>(data);
		int bytesSent = 0;
		int totalBytesSent = 0;

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
			totalBytesSent += bytesSent;
		}
		return totalBytesSent;
	}

	int Server::sendString(const SOCKET& connectSocket, const std::string& str)
	{
		return sendData(connectSocket, str.c_str(), str.length());
	}

	void Server::createContentTypeMap()
	{
		// type/substype

		contentTypes["html"] = "text/html";
		contentTypes["js"] = "text/javascript";
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