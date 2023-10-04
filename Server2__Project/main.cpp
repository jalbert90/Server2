#include "server.h"

int main() {
	using namespace N;

	// 0.0.0.0 = both
	Server s = Server("127.0.0.1", "8080");
	s.startListening();

	return 0;
}