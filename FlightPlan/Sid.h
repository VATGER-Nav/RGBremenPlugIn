#pragma once

#include <string>
#include <map>

struct SidInfo {
	std::string rwy;
	std::string dep;
	std::string nap;
	int prio{};
};

struct Sid {
	std::string wp;
	int cfl{};
	std::map<std::string, SidInfo> rwys;
};