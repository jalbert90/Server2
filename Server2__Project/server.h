#pragma once
#define WIN32_LEAN_AND_MEAN
		
#include <WinSock2.h>							// API
#include <WS2tcpip.h>							// New Stuff

#pragma comment(lib, "Ws2_32.lib")				// Tell the linker we need this

namespace N
{
	class server
	{
	public:

		// Constructor Dec
		server();
		~server();

	private:
		// Parameter Decs
		int code;
		WSADATA wsaData;
		SOCKET listenSocket = INVALID_SOCKET;

		int startServer();
		void closeServer();
		int createSocket();
	};
} // namespace N