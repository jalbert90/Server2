#include <string>
#include <vector>
#include "utils.h"

std::vector<std::string> tokenize(const std::string& input, const std::string& delim)
{
	std::vector<std::string> tokens;
	std::string token;

	// Parse string directly by string delim not char

	// GET /script.js something

	size_t pos = input.find(delim),
		prev = 0,
		l = 0;

	token = input.substr(0, pos);
	if (token != "")
	{
		tokens.push_back(token);
	}

	prev = pos + 1;

	while (pos != std::string::npos)
	{
		pos = input.find(delim, prev);
		l = pos - prev;
		token = input.substr(prev, l);
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
	tools::log("test");
}