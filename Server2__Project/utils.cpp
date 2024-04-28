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

	std::vector<std::string> tokenize(const std::string& input, const std::string& delim)
	{
		std::vector<std::string> tokens;
		std::string token;

		size_t pos = input.find(delim),
			prev = 0;

		token = input.substr(0, pos);
		if (token != "")
		{
			tokens.push_back(token);
		}

		prev = pos + 1;

		while (pos != std::string::npos)
		{
			pos = input.find(delim, prev);
			token = input.substr(prev, pos - prev);
			if (token != "")
			{
				tokens.push_back(token);
			}
			prev = pos + 1;
		}

		return tokens;
	}
}