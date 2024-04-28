#pragma once

#include <string>

namespace N
{
	struct Database_Entry
	{
		int key;
		std::string lastName{},
			firstName{};
		int age{};
	};
} // namesace N