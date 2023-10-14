#include "utils.h"

#include <iostream>

namespace tools
{
	void log(const std::string& message)
	{
		std::cout << message << std::endl;
	}
}