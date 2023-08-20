#include "FlightPlan.h"

FlightPlan::FlightPlan(std::string callsign, const EuroScopePlugIn::CFlightPlanExtractedRoute& route, std::string rawRoute)
{
	this->ParseRoute(route, rawRoute);
}

FlightPlan::FlightPlan(const char* callsign, const EuroScopePlugIn::CFlightPlanExtractedRoute& route, const char* rawRoute)
{
}

void FlightPlan::ParseRoute(const EuroScopePlugIn::CFlightPlanExtractedRoute& route, std::string rawRoute)
{
	this->route.clear();
	this->direction = 0.0;
	this->distance = 0.0;

	EuroScopePlugIn::CPosition lastPos;
	EuroScopePlugIn::CPosition rePos;
	std::string airway;
	std::vector<RouteEntry> wps;
	int numPoints = route.GetPointsNumber();

	for (int i = 0; i < numPoints; i++)
	{
		std::string name = route.GetPointName(i);

		EuroScopePlugIn::CPosition pos = route.GetPointPosition(i);

		double dir = 0.0;
		double dist = 0.0;
		if (i > 0) {
			dir = pos.DirectionTo(lastPos);
			dist = pos.DistanceTo(lastPos);
		}

		lastPos = pos;

		std::string aw = route.GetPointAirwayName(i);
		if (aw.size() > 0) {
			if (airway.size() == 0) {
				airway = aw;
				rePos = route.GetPointPosition(i);
			}
			else if (airway != aw) {
				double reDir = 0.0;
				double reDist = 0.0;
				if (i > 0) {
					EuroScopePlugIn::CPosition p = route.GetPointPosition(i - 1);

					reDir = rePos.DirectionTo(p);
					reDist = rePos.DistanceTo(p);
				}

				RouteEntry re(airway, reDir, reDist);
				re.airway = true;
				re.waypoints = wps;

				this->route.push_back(re);

				airway = aw;
				rePos = route.GetPointPosition(i);
				wps.clear();
			}

			wps.push_back(RouteEntry(name, dir, dist));
		}
		else {
			if (airway.size() > 0) {
				double reDir = 0.0;
				double reDist = 0.0;
				if (i > 0) {
					EuroScopePlugIn::CPosition p = route.GetPointPosition(i - 1);

					reDir = rePos.DirectionTo(p);
					reDist = rePos.DistanceTo(p);
				}

				RouteEntry re(airway, reDir, reDist);
				re.airway = true;
				re.waypoints = wps;

				this->route.push_back(re);

				airway.clear();
				wps.clear();
			}

			this->route.push_back(RouteEntry(name, dir, dist));
		}
	}

	if (numPoints >= 2) {
		this->direction = route.GetPointPosition(0).DirectionTo(route.GetPointPosition(numPoints - 1));
		this->distance = route.GetPointPosition(0).DistanceTo(route.GetPointPosition(numPoints - 1));
	}
}

std::ostream& operator<<(std::ostream& os, const FlightPlan& fp)
{
	os << "Flight(" << fp.callsign << "): ";

	for (int i = 0; i < fp.route.size(); i++) {
		os << fp.route[i];
		if (i < fp.route.size() - 1) {
			os << ";";
		}
	}

	return os;
}
