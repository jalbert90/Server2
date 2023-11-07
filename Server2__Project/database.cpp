#include "database.h"
#include "utils.h"

namespace N
{
	using namespace tools;

	Database::Database()
	{
		// Probably do nothing
	}

	void Database::initialize(const std::string& databaseFileName)
	{
		log("Initializing Database");

		if (fileExists(databaseFileName))
		{
			log("file exists");
		}
		else
		{
			log("file does not exists");
		}
	}
}