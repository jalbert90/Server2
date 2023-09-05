#include "server.h"

#include <stdio.h>
#include <iostream>
#include <string>
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
	server::server()
	{
		code = startServer();
	}

	server::~server()
	{
		// destroy
	}

	int server::startServer()
	{
		code = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (code != 0)
		{
			exitWithError("WSAStartup failed", code);
			return 1;	// Should never get here.
		}
		return 0;
	}
}