#pragma once

#include <string>
#include <vector>

namespace tools
{
	class Parser
	{
	public:		
		void parse(const std::string& httpRequest);

	private:
		std::string requestLine;
		std::string method;
		std::string request;
		std::string fileName;
		std::string fileExtension;

		std::vector<std::string> tokenize(const std::string& input, const char delim);
	};
} // namespace P