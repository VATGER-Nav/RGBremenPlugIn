#include "Weather.h"

void Weather::from_json(const nlohmann::json& j, Weather& weather) {
	j.at("info").get_to<WeatherInfo>(weather.info);
	j.at("data").get_to<std::map<std::string, WeatherReferencePoint>>(weather.points);
}

void Weather::from_json(const nlohmann::json& j, WeatherInfo& info)
{
	j.at("date").get_to(info.date);
	j.at("datestring").get_to(info.datestring);
}

void Weather::from_json(const nlohmann::json& j, WeatherReferencePoint& point)
{
	auto& coords = j.at("coords");
	point.latitude = std::stod(coords.at("lat").get<std::string>());
	point.longitude = std::stod(coords.at("long").get<std::string>());

	for (auto const& [key, val] : j.at("levels").items()) {
		point.levels.insert({ std::stoi(key), val.get<WeatherReferenceLevel>() });
	}
}

void Weather::from_json(const nlohmann::json& j, WeatherReferenceLevel& level)
{
	level.temperature = std::stod(j.at("T(K)").get<std::string>());
	level.windSpeed = std::stod(j.at("windspeed").get<std::string>());
	level.windDirection = std::stod(j.at("windhdg").get<std::string>());
}

Weather::Weather::Weather() : hash(-1)
{
}

Weather::Weather::Weather(std::string rawJSON) : hash(-1)
{
	this->parse(rawJSON);
}

Weather::Weather::Weather(std::istream& rawJSON) : hash(-1)
{
	this->parse(rawJSON);
}

void Weather::Weather::parse(std::string rawJSON)
{
	this->update(nlohmann::json::parse(rawJSON));
}

void Weather::Weather::parse(std::istream& rawJSON)
{
	this->update(nlohmann::json::parse(rawJSON));
}

void Weather::Weather::clear()
{
	std::scoped_lock<std::shared_mutex> lock(this->mutex);
	this->points.clear();
	this->hash = 0;
}

Weather::WeatherReferenceLevel Weather::Weather::findClosest(double latitude, double longitude, int altitude) const
{
	if (!this->mutex.try_lock_shared()) {
		// cannot acquire read lock (weather data is being updated right now), fallback to no winds in order to not block EuroScope
		return WeatherReferenceLevel();
	}

	if (this->points.empty()) {
		this->mutex.unlock_shared();

		// no reference points available, return empty reference level containing zero winds/temperature
		return WeatherReferenceLevel();
	}

	double distance = -1;
	WeatherReferencePoint closest;

	for (auto const& [wp, point] : this->points) {
		double d = Calculation::haversine(latitude, longitude, point.latitude, point.longitude);
		if (distance < 0 || d < distance) {
			distance = d;
			closest = point;
		}
	}

	// closest reference point has been found, unlock weather map for updates as we have a copy of altitude data available locally
	this->mutex.unlock_shared();

	return closest.findClosest(altitude);
}

void Weather::Weather::update(const nlohmann::json& j)
{
	size_t newHash = 0;
	{
		std::shared_lock<std::shared_mutex> slock(this->mutex, std::defer_lock);
		std::scoped_lock lock(slock);

		newHash = std::hash<nlohmann::json>{}(j);
		// check if hash matches currently stored data as we don't need to exclusively lock weather data if no update is required
		if (this->hash == newHash) {
			return;
		}
	}

	std::scoped_lock<std::shared_mutex> lock(this->mutex);

	this->points.clear();

	j.get_to<Weather>(*this);
	this->hash = newHash;
}

Weather::WeatherReferenceLevel Weather::WeatherReferencePoint::findClosest(int altitude) const
{
	if (this->levels.empty()) {
		// no data available for reference point, return empty reference level containing zero winds/temperature
		return WeatherReferenceLevel();
	}

	long fl = std::lround((double)altitude / 100.0);

	int prevDiff = -1;
	// initialise with highest level available in case the altitude is greater than highest flight level available.
	// levels map contains ordered (ascending) keys, highest available data will be last in map.
	// reverse iterator starts from the end of the map, returning last item.
	WeatherReferenceLevel closest = this->levels.rbegin()->second;
	for (auto const& [refFL, level] : this->levels) {
		if (fl == refFL) {
			return level;
		}

		int diff = std::abs(refFL - fl);
		if (prevDiff > 0 && diff > prevDiff) {
			// distances will continue increasing, no need to search further
			break;
		}

		prevDiff = diff;
		closest = level;
	}

	return closest;
}

bool Weather::WeatherReferenceLevel::isZero() {
	return this->temperature == 0 && this->windDirection == 0 && this->windSpeed == 0;
}
