#pragma once
#define WIN32_LEAN_AND_MEAN
		
#include <WinSock2.h>							// API
#include <WS2tcpip.h>							// New Stuff
#include <string>

#pragma comment(lib, "Ws2_32.lib")				// Tell the linker we need this

namespace N
{
	class Server
	{
	public:

		// Constructor Dec
		Server(std::string addr, std::string port);
		~Server();

	private:
		// Parameter Decs
		int code;
		WSADATA wsaData;
		SOCKET listenSocket = INVALID_SOCKET;
		struct addrinfo* result = NULL,				// Pointer to data type struct addrinfo.
						  * ptr = NULL,
						  hints;
		const std::string m_addr, m_port;

		int startServer();
		void closeServer();
	};
} // namespace N