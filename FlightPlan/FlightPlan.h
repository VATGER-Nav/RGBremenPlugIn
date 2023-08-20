#pragma once

#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include <wtypes.h>

#include "RouteEntry.h"
#include <EuroScopePlugIn.h>

class FlightPlan
{
public:
	std::string callsign;
	std::vector<RouteEntry> route;
	double direction;
	double distance;

	FlightPlan(std::string callsign, const EuroScopePlugIn::CFlightPlanExtractedRoute& route, std::string rawRoute);
	FlightPlan(const char* callsign, const EuroScopePlugIn::CFlightPlanExtractedRoute& route, const char* rawRoute);

	void ParseRoute(const EuroScopePlugIn::CFlightPlanExtractedRoute& route, std::string rawRoute);

	friend std::ostream& operator<<(std::ostream& os, const FlightPlan& fp);
};