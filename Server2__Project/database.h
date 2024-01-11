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
		// Does entry exist?
		
		// std::vector<Database_Entry> getEntries(const std::vector<int>& keys);
		/*
		* Get entries
		* Convert to json (overloaded with vector or number of entries)
		* Send json
		* Decode and display on webpage (make responsive)
		* Once done ^^ big task, switch gears and make banger website.
		*/
		Database_Entry getEntry(int entryIndex);
		//std::vector<Database_Entry> getEntries(std::vector<int> entryIndexes);
		std::vector<Database_Entry>* getEntries(int startIndex, int endIndex);
		//std::vector<Database_Entry> getEntriesByKey(std::vector<int> entryKeys);
		int addEntry(const Database_Entry& databaseEntry);
		int deleteEntry(int entryIndex);
		int findEntry(const std::string& lastName);

	private:
		std::vector<Database_Entry> databaseEntries;

		int seed(const std::string& databaseSeedFileName);
		int writeDatabase(const std::string& databaseFileName = "database.csv");
	};
} // namespace N