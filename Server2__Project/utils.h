#pragma once

#include <string>
#include <vector>

namespace tools
{
	void log(const std::string& message);
	bool fileExists(const std::string& fileName);
	std::vector<std::string> tokenize(const std::string& input, const std::string& delim = "\n");
}