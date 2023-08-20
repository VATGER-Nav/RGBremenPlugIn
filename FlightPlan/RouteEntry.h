#pragma once

#include <string>
#include <vector>
#include <iostream>

class RouteEntry
{
public:
	std::string name;
	std::vector<RouteEntry> waypoints;
	bool airway;
	double direction;
	double distance;
	int rfl;
	int speed;

	RouteEntry();
	RouteEntry(std::string name);
	RouteEntry(std::string name, double direction, double distance);
	RouteEntry(std::string name, int rfl, int speed);

	friend std::ostream& operator<<(std::ostream& os, const RouteEntry& re);
};

