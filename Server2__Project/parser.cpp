#include "parser.h"
#include "utils.h"

namespace tools
{
	/* This is what the request lines look like */
	// GET / HTTP/1.1
	// GET /myScript.js HTTP/1.1
	// GET /favicon.ico HTTP/1.1
	// GET /?input=space+test HTTP/1.1
	void Parser::parse(const std::string& httpRequest)
	{
		std::vector<std::string> v, w;

		requestLine = tokenize(httpRequest)[0];
		log("Request Line: " + requestLine);
		
		v = tokenize(requestLine, " ");

		method = v[0];
		log("Method: " + method);

		request = v[1];
		log("Request: " + request);

		if (request == "/")
		{
			return;
		}
		else if (request[1] == '?')
		{
			input = tokenize(request, "=")[1];
			log("Input: " + input);
		}
		else
		{
			fileName = tokenize(request, "/")[0];
			fileExt = tokenize(fileName, ".")[1];

			log("File Name: " + fileName);
			log("File Ext: " + fileExt);
		}
	}

	std::vector<std::string> Parser::tokenize(const std::string& input, const std::string& delim)
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
} // namespace tools