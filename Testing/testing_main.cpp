#include <string>
#include <vector>
#include "utils.h"
#include <fstream>

std::vector<std::string> tokenize(const std::string& input, const std::string& delim = "\n")
{
	std::vector<std::string> tokens;
	std::string token;

	// Parse string directly by string delim not char

	// GET /script.js something

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

int main()
{
	std::vector<std::string> v = tokenize("GET /script.js something", ".");
	
	for (auto el : v)
	{
		tools::log(el);
	}

	std::ifstream f("/test.txt");

	if (!f.good())
	{
		tools::log("failed to open file");
	}
	else
	{
		tools::log("a ok");
	}
}