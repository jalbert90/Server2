#include "utils.h"

#include <iostream>
#include <fstream>

namespace tools
{
	void log(const std::string& message)
	{
		std::cout << message << std::endl;
	}
}