#pragma once
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>		
#include <WinSock2.h>		// API
#include <WS2tcpip.h>		// New Stuff
#include <stdio.h>
#include <iostream>			// Debugging!

#pragma comment(lib, "Ws2_32.lib")	// Tell the linker we need this

namespace N
{
	class server
	{
		// Parameter Decs
		WSADATA wsaData;
		int test;
		const char* s = "test";

	public:

		// Constructor Dec
		server();
		~server();

	};
}