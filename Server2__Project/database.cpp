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

	int Database::initialize(const std::string& databaseFileName, const std::string& databaseSeedFileName)
	{
		log("Initializing database...");

		std::ifstream ifs(databaseSeedFileName);
		std::ofstream ofs(databaseFileName);

		if (!ifs)
		{
			log("Error: " + databaseSeedFileName + " stream state has errors.");
			return -1;
		}
		else
		{
			ofs << ifs.rdbuf();
			log("Database initialized");
			return 0;
		}
	}
}