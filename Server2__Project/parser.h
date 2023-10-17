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
		std::string requestLine = "",
			method = "",
			request = "",
			fileName = "",
			fileExt = "",
			input = "";

		std::vector<std::string> tokenize(const std::string& input, const std::string& delim = "\n");
	};
} // namespace tools