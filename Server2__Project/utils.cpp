#include "utils.h"

#include <iostream>
#include <fstream>

namespace tools
{
	void log(const std::string& message)
	{
		std::cout << message << std::endl;
	}

	bool fileExists(const std::string& fileName)
	{
		std::ifstream f(fileName.c_str());
		return f.good();
	}
}