#include "server.h"
#include "parser.h"

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
		log("Error Message: " + errorMessage + "\nError Code: " + std::to_string(errorCode));
		exit(1);
	}

	void logError(std::string errorMessage, int errorCode)
	{
		log("Error Message: " + errorMessage + "\nError Code: " + std::to_string(errorCode));
	}
} // unnamed namespace

namespace N
{
	// Content Types
	/* type/subtype */
	const std::map<std::string, std::string> Server::contentTypes = {
	{"html", "text/html"},
	{"js", "text/javascript"},
	{"jpg", "image/jpeg"},
	{"ico", "image/x-icon"}
	};

	Server::Server(std::string addr, std::string port) : m_addr(addr), m_port(port)
	{
		ZeroMemory(&hints, sizeof(hints));

		hints.ai_family = AF_UNSPEC;				// Adress family = IPv4 or IPv6
		hints.ai_socktype = SOCK_STREAM;			// Socket type = reliable, two-way, connection based byte stream, OOB data transmission mechanism.
		hints.ai_protocol = IPPROTO_TCP;			// Protocol = TCP
		hints.ai_flags = AI_PASSIVE;				// Socket will bind

		if (database.initialize("database.csv", "databaseSeed.csv", true) == -1)
		{
			log("`database.initialize()` failed");
		}

		std::vector<Database_Entry>* p_entries = database.getEntries(0, 1);
		if (p_entries == NULL)
		{
			log("`getEntries()` failed.");
		}
		else
		{
			std::vector<Database_Entry>& entries = *p_entries;		// Create reference just because
			for (auto el : entries)
			{
				log(el.lastName);
			}
		}

		if (startServer() != 0)
		{
			log("`startServer()` failed");
			closeServer();
			exit(-1);
		}
	}

	Server::~Server()
	{
		closeServer();
	}

	int Server::startServer()
	{
		int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (err != 0)
		{
			logError("`WSAStartup()` failed", err);
			return -1;
		}

		listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (listenSocket == INVALID_SOCKET)
		{
			logError("Failed to create socket", WSAGetLastError());
			return -1;
		}

		// Pass the address of the result pointer.
		// The contents at this address are then modified to contain the address
		// of the first struct addrinfo item in a linked list.
		err = getaddrinfo(m_addr.c_str(), m_port.c_str(), &hints, &result);
		if (err != 0)
		{
			logError("`getaddrinfo()` failed", err);
			return -1;
		}

		// Pointer to const sockaddr_in structure. Used to obtain ip and port.
		// Cast pointer struct sockaddr to pointer to const sockaddr_in.
		in_addr = (const struct sockaddr_in*)result->ai_addr;

		// Temporarily hold CSTR version of IP stored in `result`
		char buf[INET6_ADDRSTRLEN];

		// Get CSTR and string versions of IP that was stored in `result`:
		obtained_addr = inet_ntop(result->ai_family, &in_addr->sin_addr, buf, sizeof(buf));

		// Get integer (u_short) version of IP that was stored in `result`:
		obtained_port = ntohs(in_addr->sin_port);

		// Pointer to sockaddr found in struct addrinfo.
		if (bind(listenSocket, result->ai_addr, (int)result->ai_addrlen) != 0)
		{
			logError("Failed to bind socket to local address " + obtained_addr + ":" + std::to_string(obtained_port), WSAGetLastError());
			return -1;
		}

		return 0;
	}

	void Server::startListening()
	{
		if (listen(listenSocket, 4) != 0)
		{
			closeServer();
			exitWithError("\nFailed to listen on " + obtained_addr + ":" + std::to_string(obtained_port), WSAGetLastError());
		}

		log("\nListening on...\nAddress: " + obtained_addr + "\nPort: " + std::to_string(obtained_port) + "\n");

		while (true)
		{
			if (acceptConnection(connectSocket1) != 0)
			{
				log("`acceptConnection() failed");
				closeServer();
				exit(-1);
			}
			if (handleConnection(connectSocket1) != 0)
			{
				log("`handleConnection()` failed\n");
			}
		}
	}

	int Server::acceptConnection(SOCKET &connectSocket)
	{
		log("Waiting for connection...");
		connectSocket = accept(listenSocket, NULL, NULL);
		if (connectSocket == INVALID_SOCKET)
		{
			logError("Failed to accept connection:", WSAGetLastError());
			return -1;
		}

		log("Connection established");
		return 0;
	}

	int Server::handleConnection(SOCKET &connectSocket)
	{
		const int recvBufLen = 32768;
		char* recvBuf = new char[recvBufLen] ();
		int bytesRec;

		log("Wating to receive...");
		bytesRec = recv(connectSocket, recvBuf, recvBufLen, 0);
		if (bytesRec < 0)
		{
			logError("Error receiving data from client", WSAGetLastError());
			log("Closing socket...\n");
			closesocket(connectSocket);
			delete[] recvBuf;

			return -1;
		}
		else if (bytesRec == 0)
		{
			log("Connection gracefully closed by client");
		}
		else
		{
			log("Received " + std::to_string(bytesRec) + " bytes");

			if (sendResponse(connectSocket, recvBuf) != 0)
			{
				log("`sendResponse()` failed");
				return -1;
			}
		}

		log("Closing socket...\n");
		closesocket(connectSocket);
		delete[] recvBuf;

		return 0;
	}

	int Server::sendResponse(const SOCKET& connectSocket, const std::string& recvBuf)
	{
		/* This is what the request lines look like */
		// GET / HTTP/1.1
		// GET /myScript.js HTTP/1.1
		// GET /favicon.ico HTTP/1.1
		// GET /?input=space+test HTTP/1.1

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

		tools::Parser parser;
		parser.parse(recvBuf);

		std::string contentType;

		if (parser.getRequest() == "/") // Send index.html
		{
			contentType = "text/html";
			if (sendFileAsBinary(connectSocket, contentType, "index.html") != 0)
			{
				log("`sendFileAsBinary()` failed");
				return -1;
			}
		}
		else if (parser.getSearchTrigger() == "?")
		{
			// Find result
			// Send string

			// Send as JSON if found with number found
			// If not found, send found 0
			contentType = "text/html";
			std::string tempMessage = "check";
			
			if (sendString(connectSocket, getStatus(200) + getHeader(tempMessage.length(), contentType) + tempMessage) == -1)
			{
				log("`sendString()` failed while sending search result");
				return -1;
			}
		}
		else
		{
			std::map<std::string, std::string>::const_iterator ci = contentTypes.find(parser.getFileExt());
			if (ci == contentTypes.end())
			{
				log("Invalid File Type");
				if (sendString(connectSocket, "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: keep-alive\r\n\r\n") == -1)
				{
					closesocket(connectSocket);
				}

				return -1;
			}
			else
			{
				contentType = ci->second;
				if (sendFileAsBinary(connectSocket, contentType, parser.getFileName()) != 0)
				{
					log("`sendFileAsBinary()` failed");
					return -1;
				}
			}
		}

		return 0;
	}

	int Server::sendFileAsBinary(const SOCKET& connectSocket, const std::string& contentType, const std::string& fileName)
	{
		if (!fileExists(fileName))
		{
			log(fileName + " not found");
			if (sendString(connectSocket, "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: keep-alive\r\n\r\n") == -1)
			{
				closesocket(connectSocket);
			}

			return -1;
		}
		else
		{
			std::ifstream f(fileName.c_str(), std::ios::binary);
			if (!f.is_open())
			{
				log("Error opening " + fileName);
				if (sendString(connectSocket, "HTTP/1.1 500 Error\r\nContent-Length: 0\r\nConnection: keep-alive\r\n\r\n") == -1)
				{
					closesocket(connectSocket);
				}

				return -1;
			}
			else
			{
				f.seekg(0, std::ios::end);
				int fileLength = f.tellg();
				f.seekg(0, std::ios::beg);

				if (f.fail())
				{
					log("Failed to size " + fileName);
					if (sendString(connectSocket, "HTTP/1.1 500 Error\r\nContent-Length: 0\r\nConnection: keep-alive\r\n\r\n") == -1)
					{
						closesocket(connectSocket);
					}

					return -1;
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
							if (sendString(connectSocket, "HTTP/1.1 500 Error\r\nContent-Length: 0\r\nConnection: keep-alive\r\n\r\n") == -1)
							{
								closesocket(connectSocket);
							}

							return -1;
						}

						int bytesRead = f.gcount();
						int bytesSent = sendData(connectSocket, buf, bytesRead);
						if (bytesSent == -1)
						{
							log("`sendData()` failed");
							log("Failed to send " + fileName);
							if (sendString(connectSocket, "HTTP/1.1 500 Error\r\nContent-Length: 0\r\nConnection: keep-alive\r\n\r\n") == -1)
							{
								closesocket(connectSocket);
							}

							return -1;
						}

						fileLength -= bytesRead;
						totalBytesSent += bytesSent;
					}

					log("Sent " + std::to_string(totalBytesSent) + " bytes of binary in body");
					return 0;
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
				logError("`send()` failed", WSAGetLastError());
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

	std::string Server::getHeader(int contentLength, const std::string& contentType)
	{
		if (contentType == "")
		{
			return "Content-Length: " + std::to_string(contentLength) + "\r\nConnection: keep-alive\r\n\r\n";
		}
		else
		{
			return "Content-Length: " + std::to_string(contentLength) + "\r\nContent-Type: " + contentType + "\r\nConnection: keep-alive\r\n\r\n";
		}
	}

	std::string Server::getStatus(int statusCode)
	{
		switch (statusCode) {
		case 200:
			return "HTTP/1.1 200 OK\r\n";
			break;
		default:
			return "HTTP/1.1 500 Error\r\n";
		}
	}

	void Server::closeServer()
	{
		if (shutdown(connectSocket1, SD_SEND) == SOCKET_ERROR)
		{
			logError("Error shutting down send side", WSAGetLastError());
		}

		freeaddrinfo(result);
		closesocket(listenSocket);
		closesocket(connectSocket1);
		WSACleanup();
	}
} // namespace N