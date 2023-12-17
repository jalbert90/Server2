#include "database.h"
#include "utils.h"

#include <fstream>

namespace N
{
	using namespace tools;

	Database::Database()
	{
		// Probably do nothing
	}

	int Database::initialize(const std::string& databaseFileName, const std::string& databaseSeedFileName, bool override)
	{
		log("Initializing database...");

		if (fileExists(databaseFileName) && override == false)
		{
			if (seed(databaseFileName) == -1)
			{
				log("`seed()` failed");
				return -1;
			}
		}
		else
		{
			if (seed(databaseSeedFileName) == -1)
			{
				log("`seed()` failed");
				return -1;
			}

			if (writeDatabase() == -1)
			{
				log("Failed to write database.");
				return -1;
			}
			log("Database write succeeded.");
		}

		log("Database initialized.");
		return 0;
	}

	int Database::seed(const std::string& databaseSeedFileName)
	{
		log("Seeding database with " + databaseSeedFileName + "...");

		std::ifstream ifs(databaseSeedFileName);

		if (!ifs)
		{
			log("Error: " + databaseSeedFileName + " stream state has errors.");
			return -1;
		}
		else
		{
			std::string line;
			std::vector<std::string> tokens;
			Database_Entry database_Entry;

			while (std::getline(ifs, line))
			{
				tokens = tokenize(line, ",");

				database_Entry.lastName = tokens[0];
				database_Entry.firstName = tokens[1];
				database_Entry.age = std::stoi(tokens[2]);

				databaseEntries.push_back(database_Entry);
			}
		}

		log("Database seeded.");
		return 0;
	}

	int Database::writeDatabase(const std::string& databaseFileName)
	{
		std::ofstream ofs(databaseFileName);

		if (!ofs)
		{
			log("Error: " + databaseFileName + " stream state has errors");
			return -1;
		}
		else
		{
			for (auto it = databaseEntries.begin(); it != databaseEntries.end(); ++it)
			{
				ofs << it->lastName << "," << it->firstName << "," << it->age;

				if ((it + 1) != databaseEntries.end())
				{
					ofs << "\n";
				}
			}
		}
	}

	Database_Entry Database::getEntry(int entryIndex)
	{
		if (entryIndex < 0 || entryIndex >= databaseEntries.size())
		{
			log("Database entry index out of range.");
		}
		else
		{
			return databaseEntries[entryIndex];
		}
	}

	int Database::addEntry(const Database_Entry& databaseEntry)
	{
		databaseEntries.push_back(databaseEntry);
		log("Entry added.");
		return 0;
	}

	int Database::deleteEntry(int entryIndex)
	{
		if (entryIndex < 0 || entryIndex >= databaseEntries.size())
		{
			log("Database entry index out of range.");
			return -1;
		}
		else
		{
			databaseEntries.erase(databaseEntries.begin() + entryIndex);
			log("Entry erased.");
			return 0;
		}
	}

	int Database::findEntry(const std::string& lastName)
	{
		std::vector<Database_Entry>::iterator it = std::find_if(databaseEntries.begin(), databaseEntries.end(), [lastName](Database_Entry entry) { return entry.lastName == lastName; });

		if (it == databaseEntries.end())
		{
			log("Query \"" + lastName + "\" not found");
			return -1;
		}
		else
		{
			return it - databaseEntries.begin();
		}
	}
} // namespace N