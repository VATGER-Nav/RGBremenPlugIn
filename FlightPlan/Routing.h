#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "RouteEntry.h"

struct Routing {
	std::string adep;
	std::string ades;
	int maxlvl{};
	int minlvl{};
	std::vector<std::string> waypts;

	friend std::ostream& operator<<(std::ostream& os, const Routing& rt);
};