#include "TrueAirspeed.h"

TrueAirspeed::TrueAirspeed(Config* m_Config):
	weatherUpdateInterval(5),
	useReportedGS(true),
	useTrueNorthHeading(true),
	prefixIAS("I"),
	prefixMach("M"),
	machDigits(2),
	machThresholdFL(24500),
	unreliableIASIndicator("DIAS"),
	unreliableIASColor(nullptr),
	unreliableMachIndicator("DMACH"),
	unreliableMachColor(nullptr),
	broadcastUnreliableSpeed(true)
{
	nlohmann::json airspeedConfiguration = nullptr;
	airspeedConfiguration = m_Config->GetAirspeedConfiguration();
	if (airspeedConfiguration == nullptr) {
		//LogMessage("Airspeed configuration not loaded. Please check RG Bremen.json file for the corresponding section.", "Configuration");
		messageMap.push_back("Airspeed configuration not loaded. Please check RG Bremen.json file for the corresponding section.");
		return;
	}

	try
	{
		auto& machCfg = airspeedConfiguration.at("mach");
		int machDigits = machCfg.value<int>("digits", this->machDigits);
		if (machDigits < MIN_MACH_DIGITS || machDigits > MAX_MACH_DIGITS) {
			//LogMessage("Invalid digit count for mach numbers. Must be between 1 and 13, falling back to default (2)", "Configuration");
			messageMap.push_back("Invalid digit count for mach numbers. Must be between 1 and 13, falling back to default (2)");
		}
		else {
			this->machDigits = machDigits;
		}
		int machThresholdFL = machCfg.value<int>("thresholdFL", this->machThresholdFL);
		if (machThresholdFL < 0) {
			//LogMessage("Invalid mach threshold flight level. Must be greater than 0, falling back to default (245)", "Configuration");
			messageMap.push_back("Invalid mach threshold flight level. Must be greater than 0, falling back to default (245)");
		}
		else {
			this->machThresholdFL = machThresholdFL * 100;
		}

		std::string prefixMach = machCfg.value<std::string>("prefix", this->prefixMach);
		if (prefixMach.size() > (size_t)(TAG_ITEM_MAX_CONTENT_LENGTH - this->machDigits)) {
			std::ostringstream msg;
			msg << "Mach number prefix is too long, must be " << (TAG_ITEM_MAX_CONTENT_LENGTH - this->machDigits) << " characters or less. Falling back to default (" << this->prefixMach << ")";
			//LogMessage(msg.str(), "Configuration");
			messageMap.push_back(msg.str());
		}
		else {
			this->prefixMach = prefixMach;
		}

		std::string unreliableMachIndicator = machCfg.value<std::string>("unreliableIndicator", this->unreliableMachIndicator);
		if (unreliableMachIndicator.size() > (size_t)(TAG_ITEM_MAX_CONTENT_LENGTH)) {
			std::ostringstream msg;
			msg << "Unreliable Mach number indicator is too long, must be " << TAG_ITEM_MAX_CONTENT_LENGTH << " characters or less. Falling back to default (" << this->unreliableMachIndicator << ")";
			//LogMessage(msg.str(), "Configuration");
			messageMap.push_back(msg.str());
		}
		else {
			this->unreliableMachIndicator = unreliableMachIndicator;
		}

		std::string unreliableMachColor = machCfg.value<std::string>("unreliableColor", "");
		if (unreliableMachColor.size() > 0) {
			this->unreliableMachColor = parseRGBString(unreliableMachColor);
			if (this->unreliableMachColor == nullptr) {
				//LogMessage("Unreliable Mach number color is invalid, must be in comma-separated integer RGB format (e.g. \"123,123,123\"). Falling back to no color", "Configuration");
				messageMap.push_back("Unreliable Mach number color is invalid, must be in comma-separated integer RGB format (e.g. \"123,123,123\"). Falling back to no color");
			}
		}
	}
	catch (const std::exception&)
	{
		if (m_Config->GetDebugMode()) {
			//LogMessage("Unable to parse 'MACH' section of airspeed configuration.", "Configuration");
			messageMap.push_back("Unable to parse 'MACH' section of airspeed configuration.");
		}
	}

	try
	{
		auto& iasCfg = airspeedConfiguration.at("ias");

		std::string prefixIAS = iasCfg.value<std::string>("prefix", this->prefixIAS);
		if (prefixIAS.size() > (size_t)(TAG_ITEM_MAX_CONTENT_LENGTH - this->machDigits)) {
			std::ostringstream msg;
			msg << "Indicated air speed prefix is too long, must be " << (TAG_ITEM_MAX_CONTENT_LENGTH - this->machDigits) << " characters or less. Falling back to default (" << this->prefixIAS << ")";
			//LogMessage(msg.str(), "Configuration");
			messageMap.push_back(msg.str());
		}
		else {
			this->prefixIAS = prefixIAS;
		}

		std::string unreliableIASIndicator = iasCfg.value<std::string>("unreliableIndicator", this->unreliableIASIndicator);
		if (unreliableIASIndicator.size() > (size_t)(TAG_ITEM_MAX_CONTENT_LENGTH)) {
			std::ostringstream msg;
			msg << "Unreliable IAS indicator is too long, must be " << TAG_ITEM_MAX_CONTENT_LENGTH << " characters or less. Falling back to default (" << this->unreliableIASIndicator << ")";
			/*this->LogMessage(msg.str(), "Configuration");*/
			messageMap.push_back(msg.str());
		}
		else {
			this->unreliableIASIndicator = unreliableIASIndicator;
		}

		std::string unreliableIASColor = iasCfg.value<std::string>("unreliableColor", "");
		if (unreliableIASColor.size() > 0) {
			this->unreliableIASColor = parseRGBString(unreliableIASColor);
			if (this->unreliableIASColor == nullptr) {
				//this->LogMessage("Unreliable IAS color is invalid, must be in comma-separated (integer) RGB format (e.g. \"123,123,123\"). Falling back to no color", "Configuration");
				messageMap.push_back("Unreliable IAS color is invalid, must be in comma-separated (integer) RGB format (e.g. \"123,123,123\"). Falling back to no color");
			}
		}
	}
	catch (const std::exception&)
	{
		if (m_Config->GetDebugMode()) {
			//LogMessage("Unable to parse 'IAS' section of airspeed configuration.", "Configuration");
			messageMap.push_back("Unable to parse 'IAS' section of airspeed configuration.");
		}
	}

	try
	{
		auto& weatherCfg = airspeedConfiguration.at("weather");

		this->weatherUpdateURL = weatherCfg.value<std::string>("url", this->weatherUpdateURL);
		this->weatherUpdateInterval = std::chrono::minutes(weatherCfg.value<int>("update", this->weatherUpdateInterval.count()));

		this->ResetWeatherUpdater();
	}
	catch (const std::exception&)
	{
		if (m_Config->GetDebugMode()) {
			//LogMessage("Unable to parse 'Weather' section of airspeed configuration.", "Configuration");
			messageMap.push_back("Unable to parse 'Weather' section of airspeed configuration.");
		}
	}

	try
	{
		auto& broadcastCfg = airspeedConfiguration.at("broadcast");

		this->broadcastUnreliableSpeed = broadcastCfg.value<bool>("unreliableSpeed", this->broadcastUnreliableSpeed);
	}
	catch (const std::exception& e)
	{
		if (m_Config->GetDebugMode()) {
			//LogMessage("Unable to parse 'Broadcast' section of airspeed configuration.", "Configuration");
			messageMap.push_back("Unable to parse 'Broadcast' section of airspeed configuration.");
		}
	}
}

TrueAirspeed::~TrueAirspeed()
{
}

void TrueAirspeed::SetReportedIAS(const EuroScopePlugIn::CFlightPlan& fp, std::string selected)
{
	int ias;
	try {
		ias = std::stoi(selected);
	}
	catch (std::exception const& ex) {
		std::ostringstream msg;
		msg << "Failed to parse reported IAS: " << ex.what();

		//this->LogMessage(msg.str(), "Airspeed");
		return;
	}

	this->reportedIAS.insert_or_assign(fp.GetCallsign(), ias);
}

void TrueAirspeed::ClearReportedIAS(const EuroScopePlugIn::CFlightPlan& fp)
{
	this->reportedIAS.erase(fp.GetCallsign());
}

void TrueAirspeed::ToggleCalculatedIAS(const EuroScopePlugIn::CFlightPlan& fp, bool abbreviated)
{
	std::string cs = fp.GetCallsign();
	if (abbreviated) {
		if (this->calculatedIASAbbreviatedToggled.contains(cs)) {
			this->calculatedIASAbbreviatedToggled.erase(cs);
		}
		else {
			this->calculatedIASAbbreviatedToggled.insert(cs);
		}
	}
	else {
		if (this->calculatedIASToggled.contains(cs)) {
			this->calculatedIASToggled.erase(cs);
		}
		else {
			this->calculatedIASToggled.insert(cs);
		}
	}
}

double TrueAirspeed::CalculateIAS(const EuroScopePlugIn::CRadarTarget& rt)
{
	if (!rt.IsValid()) return -1;
	int hdg = this->useTrueNorthHeading ? rt.GetPosition().GetReportedHeadingTrueNorth() : rt.GetPosition().GetReportedHeading();
	int gs = this->useReportedGS ? rt.GetPosition().GetReportedGS() : rt.GetGS();
	int alt = rt.GetPosition().GetPressureAltitude();

	try
	{
		return Calculation::calculateCAS(alt, hdg, gs, this->weather.findClosest(rt.GetPosition().GetPosition().m_Latitude, rt.GetPosition().GetPosition().m_Longitude, alt));
	}
	catch (const std::exception&)
	{
		return -1;
	}
}

void TrueAirspeed::ShowCalculatedIAS(const EuroScopePlugIn::CRadarTarget& rt, char tagItemContent[16], int* tagItemColorCode, COLORREF* tagItemRGB, bool abbreviated, bool onlyToggled)
{
	if (!rt.IsValid()) {
		return;
	}

	if (onlyToggled && ((abbreviated && !this->calculatedIASAbbreviatedToggled.contains(rt.GetCallsign())) ||
		(!abbreviated && !this->calculatedIASToggled.contains(rt.GetCallsign())))) {
		return;
	}

	if (this->unreliableSpeedToggled.contains(rt.GetCallsign()) && this->unreliableIASIndicator.size() > 0) {
		// aircraft has been flagged as having unreliable speed, indicator for unreliable IAS was configured, set and skip calculations
		strcpy_s(tagItemContent, 16, this->unreliableIASIndicator.c_str());
		if (this->unreliableIASColor != nullptr) {
			*tagItemColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
			*tagItemRGB = *this->unreliableIASColor;
		}
		return;
	}

	double cas = this->CalculateIAS(rt);
	if (cas < 0) {
		// gs or alt outside of supported ranges. no value to display in tag
		return;
	}

	std::ostringstream tag;
	if (!this->prefixIAS.empty() && !abbreviated) {
		tag << this->prefixIAS;
	}
	tag << std::setfill('0');

	auto it = this->reportedIAS.find(rt.GetCallsign());
	if (it == this->reportedIAS.end()) {
		if (abbreviated) {
			tag << std::setw(2) << std::round(cas / 10.0);
		}
		else {
			tag << std::setw(3) << std::round(cas);
		}
	}
	else {
		double diff = it->second - cas;
		if (diff > 0) {
			tag << "+";
		}
		else if (diff < 0) {
			tag << "-";
		}

		if (abbreviated) {
			tag << std::setw(2) << std::round(std::abs(diff / 10.0));
		}
		else {
			tag << std::setw(3) << std::round(std::abs(diff));
		}
	}

	strcpy_s(tagItemContent, 16, tag.str().c_str());
	if (this->unreliableSpeedToggled.contains(rt.GetCallsign()) && this->unreliableIASColor != nullptr) {
		// aircraft has been flagged as having unreliable speed, but no unreliable IAS indicator was configured, but a color was set
		*tagItemColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
		*tagItemRGB = *this->unreliableIASColor;
	}
}

void TrueAirspeed::SetReportedMach(const EuroScopePlugIn::CFlightPlan& fp, std::string selected)
{
	int mach;
	try {
		mach = std::stoi(selected);
	}
	catch (std::exception const& ex) {
		std::ostringstream msg;
		msg << "Failed to parse reported Mach: " << ex.what();

		//this->LogMessage(msg.str(), "Airspeed");
		return;
	}

	this->reportedMach.insert_or_assign(fp.GetCallsign(), (double)mach / 100.0);
}

void TrueAirspeed::ClearReportedMach(const EuroScopePlugIn::CFlightPlan& fp)
{
	this->reportedMach.erase(fp.GetCallsign());
}

void TrueAirspeed::ToggleCalculatedMach(const EuroScopePlugIn::CFlightPlan& fp, bool aboveThreshold)
{
	std::string cs = fp.GetCallsign();
	if (aboveThreshold) {
		if (this->calculatedMachAboveThresholdToggled.contains(cs)) {
			this->calculatedMachAboveThresholdToggled.erase(cs);
		}
		else {
			this->calculatedMachAboveThresholdToggled.insert(cs);
		}
	}
	else {
		if (this->calculatedMachToggled.contains(cs)) {
			this->calculatedMachToggled.erase(cs);
		}
		else {
			this->calculatedMachToggled.insert(cs);
		}
	}
}

double TrueAirspeed::CalculateMach(const EuroScopePlugIn::CRadarTarget& rt)
{
	if (!rt.IsValid()) {
		return -1;
	}

	int hdg = this->useTrueNorthHeading ? rt.GetPosition().GetReportedHeadingTrueNorth() : rt.GetPosition().GetReportedHeading(); // heading in degrees
	int gs = this->useReportedGS ? rt.GetPosition().GetReportedGS() : rt.GetGS(); // ground speed in knots
	int alt = rt.GetPosition().GetPressureAltitude(); // altitude in feet

	try {
		return Calculation::calculateMach(alt, hdg, gs, this->weather.findClosest(rt.GetPosition().GetPosition().m_Latitude, rt.GetPosition().GetPosition().m_Longitude, alt));
	}
	catch (std::exception const&) {
		// gs or alt outside of supported ranges. no value to display in tag
		return -1;
	}
}

void TrueAirspeed::ShowCalculatedMach(const EuroScopePlugIn::CRadarTarget& rt, char tagItemContent[16], int* tagItemColorCode, COLORREF* tagItemRGB, bool aboveThreshold, bool onlyToggled)
{
	if (!rt.IsValid()) {
		return;
	}

	if (onlyToggled && ((aboveThreshold && !this->calculatedMachAboveThresholdToggled.contains(rt.GetCallsign())) ||
		(!aboveThreshold && !this->calculatedMachToggled.contains(rt.GetCallsign())))) {
		return;
	}

	if (aboveThreshold && rt.GetPosition().GetFlightLevel() < this->machThresholdFL) {
		return;
	}

	if (this->unreliableSpeedToggled.contains(rt.GetCallsign()) && this->unreliableMachIndicator.size() > 0) {
		// aircraft has been flagged as having unreliable speed, indicator for unreliable Mach number was configured, set and skip calculations
		strcpy_s(tagItemContent, 16, this->unreliableMachIndicator.c_str());
		if (this->unreliableMachColor != nullptr) {
			*tagItemColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
			*tagItemRGB = *this->unreliableMachColor;
		}
		return;
	}

	double mach = this->CalculateMach(rt);
	if (mach < 0) {
		// gs or alt outside of supported ranges. no value to display in tag
		return;
	}

	std::ostringstream tag;
	if (!this->prefixMach.empty()) {
		tag << this->prefixMach;
	}

	auto it = this->reportedMach.find(rt.GetCallsign());
	if (it == this->reportedMach.end()) {
		tag << std::setfill('0') << std::setw(this->machDigits) << std::round(mach * std::pow(10, this->machDigits));
	}
	else {
		double diff = it->second - mach;
		if (diff > 0) {
			tag << "+";
		}
		else if (diff < 0) {
			tag << "-";
		}

		tag << std::setfill('0') << std::setw(this->machDigits) << std::round(std::abs(diff * std::pow(10, this->machDigits)));
	}

	strcpy_s(tagItemContent, 16, tag.str().c_str());
	if (this->unreliableSpeedToggled.contains(rt.GetCallsign()) && this->unreliableMachColor != nullptr) {
		// aircraft has been flagged as having unreliable speed, but no unreliable Mach number indicator was configured, but a color was set
		*tagItemColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
		*tagItemRGB = *this->unreliableMachColor;
	}
}

void TrueAirspeed::ToggleUnreliableSpeed(const EuroScopePlugIn::CFlightPlan& fp)
{
	std::string cs = fp.GetCallsign();
	bool enabled = false;
	if (this->unreliableSpeedToggled.contains(cs)) {
		this->unreliableSpeedToggled.erase(cs);
	}
	else {
		this->unreliableSpeedToggled.insert(cs);
		enabled = true;
	}

	if (this->broadcastUnreliableSpeed) {
		std::ostringstream msg;
		msg << BROADCAST_PREFIX << BROADCAST_DELIMITER
			<< BROADCAST_UNRELIABLE_SPEED;

		this->SetFlightStripAnnotation(fp, enabled ? msg.str() : "");

		msg << BROADCAST_DELIMITER << enabled;
		this->BroadcastScratchPad(fp, msg.str());
	}
}

void TrueAirspeed::BroadcastScratchPad(const EuroScopePlugIn::CFlightPlan& fp, std::string msg)
{
	if (!fp.IsValid()) {
		return;
	}

	if (!fp.GetTrackingControllerIsMe() && strcmp(fp.GetTrackingControllerId(), "") != 0) {
		return;
	}

	auto cad = fp.GetControllerAssignedData();
	std::string scratch = cad.GetScratchPadString();

	if (!cad.SetScratchPadString(msg.c_str())) {
		//this->LogMessage("Failed to set broadcast message in scratch pad", fp.GetCallsign());
	}

	if (!cad.SetScratchPadString(scratch.c_str())) {
		//this->LogMessage("Failed to reset scratch pad after setting broadcast message", fp.GetCallsign());
	}
}

void TrueAirspeed::CheckScratchPadBroadcast(const EuroScopePlugIn::CFlightPlan& fp)
{
	std::vector<std::string> scratch = split(fp.GetControllerAssignedData().GetScratchPadString(), BROADCAST_DELIMITER);

	if (scratch.size() < 3 || scratch[0] != BROADCAST_PREFIX) {
		return;
	}

	if (this->broadcastUnreliableSpeed && scratch[1] == BROADCAST_UNRELIABLE_SPEED) {
		if (scratch[2] == "1") {
			//this->LogDebugMessage("Enabling unreliable speed indication for aircraft after broadcast", fp.GetCallsign());
			this->unreliableSpeedToggled.insert(fp.GetCallsign());
		}
		else {
			//this->LogDebugMessage("Disabling unreliable speed indication for aircraft after broadcast", fp.GetCallsign());
			this->unreliableSpeedToggled.erase(fp.GetCallsign());
		}
	}
}

void TrueAirspeed::SetFlightStripAnnotation(const EuroScopePlugIn::CFlightPlan& fp, std::string msg, int index)
{
	if (!fp.IsValid()) {
		return;
	}

	if (!fp.GetTrackingControllerIsMe() && strcmp(fp.GetTrackingControllerId(), "") != 0) {
		return;
	}

	auto cad = fp.GetControllerAssignedData();

	if (!cad.SetFlightStripAnnotation(index, msg.c_str())) {
		//this->LogMessage("Failed to set message in flight strip annotations", fp.GetCallsign());
	}
}

void TrueAirspeed::CheckFlightStripAnnotations(const EuroScopePlugIn::CFlightPlan& fp)
{
	if (!fp.IsValid()) {
		return;
	}

	std::string annotation = fp.GetControllerAssignedData().GetFlightStripAnnotation(BROADCAST_FLIGHT_STRIP_INDEX);
	std::vector<std::string> msg = split(annotation, BROADCAST_DELIMITER);

	if (msg.size() < 2 || msg[0] != BROADCAST_PREFIX) {
		return;
	}

	if (this->broadcastUnreliableSpeed) {
		if (msg[1] == BROADCAST_UNRELIABLE_SPEED) {
			//this->LogDebugMessage("Enabling unreliable speed indication for aircraft due to flight strip annotation", fp.GetCallsign());
			this->unreliableSpeedToggled.insert(fp.GetCallsign());
		}
		else {
			//this->LogDebugMessage("Disabling unreliable speed indication for aircraft due to empty flight strip annotation", fp.GetCallsign());
			this->unreliableSpeedToggled.erase(fp.GetCallsign());
		}
	}
}

void TrueAirspeed::CheckFlightStripAnnotationsForAllAircraft(std::vector<EuroScopePlugIn::CFlightPlan> flightPlans)
{
	if (this->broadcastUnreliableSpeed) {
		this->unreliableSpeedToggled.clear();

		for (auto fp = flightPlans.begin(); fp != flightPlans.end(); fp++) {
			this->CheckFlightStripAnnotations(*fp);
		}
	}
}

void TrueAirspeed::UpdateWeather()
{
	//this->LogDebugMessage("Retrieving weather data", "Weather");

	std::string weatherJSON;
	try {
		weatherJSON = Http::get(this->weatherUpdateURL);
	}
	catch (std::exception ex) {
		/*this->LogMessage("Failed to load weather data", "Weather");
		this->LogDebugMessage(ex.what(), "Weather");*/
		return;
	}

	//this->LogDebugMessage("Parsing weather data", "Weather");
	try {
		this->weather.parse(weatherJSON);
	}
	catch (std::exception ex) {
		/*this->LogMessage("Failed to parse weather data", "Weather");
		this->LogDebugMessage(ex.what(), "Weather");*/
		return;
	}

	//this->LogDebugMessage("Successfully updated weather data", "Weather");
}

void TrueAirspeed::StartWeatherUpdater()
{
	if (this->weatherUpdateURL.empty() && this->weatherUpdateInterval.count() > 0) {
		//this->LogMessage("Weather update URL is empty, cannot fetch weather data for calculations. Configure via config file (RG Bremen.json in same directory as RG Bremen EuroScopePlugIn.dll).", "Configuration");
		return;
	}

	if (this->weatherUpdateHandler == nullptr && this->weatherUpdateInterval.count() > 0) {
		this->weatherUpdateHandler = new Threading::PeriodicAction(std::chrono::milliseconds(0), std::chrono::milliseconds(this->weatherUpdateInterval), std::bind(&TrueAirspeed::UpdateWeather, this));
	}
}

void TrueAirspeed::StopWeatherUpdater()
{
	if (this->weatherUpdateHandler != nullptr) {
		this->weatherUpdateHandler->Stop();
		delete this->weatherUpdateHandler;
		this->weatherUpdateHandler = nullptr;
	}
}

void TrueAirspeed::ResetWeatherUpdater()
{
	this->StopWeatherUpdater();
	this->StartWeatherUpdater();
}