#pragma once

#include <string>

namespace N
{
	class Database
	{
	public:
		Database();
		int initialize(const std::string& databaseFileName, const std::string& databaseSeedFileName);

	private:
		//
	};
} // namespace N