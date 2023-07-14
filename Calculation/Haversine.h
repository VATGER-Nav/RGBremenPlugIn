#pragma once

#include <numbers>

namespace Calculation {
	double haversine(const double lat1, const double lng1, const double lat2, const double lng2);
	double deg2rad(const double deg);

	constexpr double MEAN_EARTH_RADIUS_METERS = 6371008.7714;
}