#pragma once

#include <string>

namespace tools
{
	class Parser
	{
	public:		
		std::string getRequest();
		std::string getSearchTrigger();
		std::string getFileName();
		std::string getFileExt();

		void parse(const std::string& httpRequest);		

	private:
		std::string requestLine = "",
			method = "",
			request = "",
			searchTrigger = "",
			input = "",			
			fileName = "",
			fileExt = "";

		//std::vector<std::string> tokenize(const std::string& input, const std::string& delim = "\n");
	};
} // namespace tools