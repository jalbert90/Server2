#include "server.h"

int main() {
	using namespace N;

	Server s = Server("0.0.0.0", "8080");
	s.startListening();

	return 0;
}