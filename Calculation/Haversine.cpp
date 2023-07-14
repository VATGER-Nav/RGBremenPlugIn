#include "Haversine.h"

double Calculation::haversine(const double lat1, const double lng1, const double lat2, const double lng2) {
	double phi1 = deg2rad(lat1);
	double phi2 = deg2rad(lat2);
	double deltaPhi = deg2rad(lat2 - lat1);
	double deltaLam = deg2rad(lng2 - lng1);

	double a = sin(deltaPhi / 2) * sin(deltaPhi / 2) + cos(phi1) * cos(phi2) * sin(deltaLam / 2) * sin(deltaLam / 2);
	double c = 2 * atan2(sqrt(a), sqrt(1 - a));
	return c * MEAN_EARTH_RADIUS_METERS;
}

double Calculation::deg2rad(const double deg) {
	return deg * (std::numbers::pi / 180);
}