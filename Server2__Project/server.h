#pragma once
#define WIN32_LEAN_AND_MEAN
		
#include <WinSock2.h>							// API
#include <WS2tcpip.h>							// New Stuff
#include <string>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")				// Tell the linker we need this

namespace N
{
	class Server
	{
	public:

		// Constructor Dec
		Server(std::string addr, std::string port);
		~Server();

		void startListening();

	private:
		// Parameter Decs
		int code;
		WSADATA wsaData;
		SOCKET listenSocket = INVALID_SOCKET;
		SOCKET connectSocket1 = INVALID_SOCKET;
		struct addrinfo* result = NULL,				// Pointer to data type struct addrinfo.
						  * ptr = NULL,
						  hints;					// addrinfo structure containing info about the type of socket that is supported.
		struct sockaddr_in* in_addr;				// Pointer to sockaddr_in structure. Used to obtain ip and port.
		char buf[INET_ADDRSTRLEN];					// Used in inet_ntop().
		const std::string m_addr, m_port;			// Node and service.

		int startServer();
		void closeServer();
		void acceptConnection(SOCKET &connectSocket);
		void handleConnection(SOCKET &connectSocket);
		std::string buildResponse();
		std::string getRequestLine(char* recvBuf);
		std::vector<std::string> tokenize(std::string input);
		void sendResponse(std::string response);
	};
} // namespace N