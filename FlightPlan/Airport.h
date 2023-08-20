#pragma once

#include <string>
#include <map>
#include <set>
#include <regex>

#include "sid.h"
#include "routing.h"

struct Airport {
	std::string icao;
	int elevation;
	bool active;
	std::map<std::string, Sid> sids;
	std::map<std::string, bool> rwys;
	std::vector<Routing> validroutes;
	std::regex rwy_regex;
};