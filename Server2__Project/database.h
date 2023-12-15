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
		int initialize(const std::string& databaseFileName, const std::string& databaseSeedFileName, bool override = false);

		// get, add, delete

	private:
		std::vector<Database_Entry> databaseEntries;

		int seed(const std::string& databaseSeedFileName);
		int writeDatabase(const std::string& databaseFileName = "database.csv");
	};
} // namespace N