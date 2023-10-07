#pragma once
#define WIN32_LEAN_AND_MEAN
		
#include <WinSock2.h>							// API
#include <WS2tcpip.h>							// New Stuff: `getaddrinfo()` for example
#include <string>
#include <vector>
#include <map>

#pragma comment(lib, "Ws2_32.lib")				// Tell the linker we need this

namespace N
{
	class Server
	{
	public:
		Server(std::string addr, std::string port);
		~Server();

		void startListening();

	private:
		static const std::map<std::string, std::string> contentTypes;			// Static map from file extension to MIME type.

		const std::string m_addr, m_port;										// IP and port passed to constructor.
		int code;
		WSADATA wsaData;														// Will contain info about Ws2_32.dll (Windows Sockets implementation).
		SOCKET listenSocket = INVALID_SOCKET;
		SOCKET connectSocket1 = INVALID_SOCKET;
		struct addrinfo* result = NULL,											// Pointer to data type struct addrinfo.
						  * ptr = NULL,											// Not currently used. Could be used to get next address or something like that?
						  hints;												// addrinfo structure containing info about the type of socket that is supported.
		const struct sockaddr_in* in_addr;										// Pointer to const sockaddr_in structure. Used to obtain ip and port.
		std::string obtained_addr;												// String version of IP obtained from `getaddrinfo()`.
		u_short obtained_port;													// Port obtained from `getaddrinfo()` will be stored here.
		const std::string STATUS200 = "HTTP 1.1 200 OK\r\n";

		int startServer();
		int acceptConnection(SOCKET &connectSocket);
		int handleConnection(SOCKET &connectSocket);
		std::string getRequestLine(char* recvBuf);
		int sendResponse(const SOCKET& connectSocket, const std::string& requestLine);
		std::vector<std::string> tokenize(std::string input, char delim);
		int sendFileAsBinary(const SOCKET& connectSocket, const std::string& contentType, const std::string& fileName);
		bool fileExists(const std::string& fileName);
		int sendData(const SOCKET& connectSocket, const void* data, int dataLength);
		int sendString(const SOCKET& connectSocket, const std::string& str);
		std::string getHeader(int contentLength, const std::string& contentType="");
		std::string getStatus(int statusCode);
		void closeServer();
	};
} // namespace N