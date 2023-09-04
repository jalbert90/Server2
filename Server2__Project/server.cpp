#include "server.h"

using namespace N;

server::server()
{
	test = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (test != 0)
	{
		throwError("WSAStartup failed", test);
	}
}

server::~server()
{
	// destroy
}

void server::throwError(const char* errorMessage, int errorCode)
{
	printf("Error Message: %s\nError Code: %d\n", errorMessage, errorCode);
}