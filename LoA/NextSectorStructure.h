#pragma once

#include <string>

struct NextSectorStructure
{
	std::string nextSectorId;
	int copAltitude;
	int clbDesc = 0;
	std::string copn;
	bool isValid = false;
};