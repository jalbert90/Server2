#pragma once

#include "database_entry.h"

#include <string>
#include <vector>

namespace N
{
	class Database
	{
	public:
		Database();
		int initialize(const std::string& databaseFileName, const std::string& databaseSeedFileName);
		int seed(const std::string& databaseSeedFileName);
		// writeDatabase()

	private:
		std::vector<Database_Entry> databaseEntries;
	};
} // namespace N