#pragma once

#include <string>
#include <vector>

struct NextSectorStructure
{
	std::string nextSectorId = "";
	int copAltitude = 0;
	int clbDesc = 0;
	std::string copn = "";
	bool isValid = false;
};

static std::unordered_map<std::string, std::vector<std::string>> sectorOwnership {
	{
		"ALR",
		{ "HEI", "EIDE", "FRI", "EIDW", "DST", "EMS", "HRZ", "HAME", "HAMW", "HAN" }
	}
};