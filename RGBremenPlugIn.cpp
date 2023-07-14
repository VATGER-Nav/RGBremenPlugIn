#include "stdafx.h"
#include "RGBremenPlugIn.h"
#include <string>

void RGBremenPlugIn::InitializeTagItemHandler(std::string& pluginName)
{
	m_TagHandler = new TagHandler(m_Config, m_SidStarHandler, GetTransitionAltitude());
	DisplayUserMessage(pluginName.c_str(), "TagItemHandler", "Handler initialized", true, false, false, false, false);

	// Register TAG Items
	RegisterTagItemType("TAG / Groundspeed", RG_BREMEN_TAG_ITEM_GROUNDSPEED);
	RegisterTagItemType("TAG / Altitude", RG_BREMEN_TAG_ITEM_ALTITUDE);
	RegisterTagItemType("TAG / Vertical Speed Indicator", RG_BREMEN_TAG_ITEM_VERTICAL_SPEED_INDICATOR);
	RegisterTagItemType("TAG / Vertical Speed", RG_BREMEN_TAG_ITEM_VERTICAL_SPEED);
	RegisterTagItemType("TAG / Indicated Airspeed", RG_BREMEN_TAG_ITEM_INDICATED_AIRSPEED);
	RegisterTagItemType("TAG / Destination", RG_BREMEN_TAG_ITEM_DESTINATION);
	RegisterTagItemType("TAG / Sid Star Check", RG_BREMEN_TAG_ITEM_SIDSTAR);
	RegisterTagItemType("TAG / Cleared Flight Level", RG_BREMEN_TAG_ITEM_CLEARED_FLIGHT_LEVEL);
	RegisterTagItemType("TAG / Assigned Heading", RG_BREMEN_TAG_ITEM_ASSIGNED_HEADING);
	RegisterTagItemType("TAG / COPX", RG_BREMEN_TAG_ITEM_COPX);
	RegisterTagItemType("TAG / Transfer Flight Level", RG_BREMEN_TAG_ITEM_TRANSFER_FLIGHT_LEVEL);
	RegisterTagItemType("TAG / Next Sector", RG_BREMEN_TAG_ITEM_NEXT_SECTOR);
	RegisterTagItemType("TAG / VFR Indicator", RG_BREMEN_TAG_ITEM_VFR_INDICATOR);
	RegisterTagItemType("TAG / VFR Callsign", RG_BREMEN_TAG_ITEM_VFR_CALLSIGN);
	RegisterTagItemType("TAG / VFR Altitude", RG_BREMEN_TAG_ITEM_VFR_ALTITUDE);
	RegisterTagItemType("TAG / VFR Indicated Airspeed", RG_BREMEN_TAG_ITEM_INDICATED_AIRSPEED);
	RegisterTagItemType("TAG / TWR Indicator", RG_BREMEN_TAG_ITEM_TWR_INDICATOR);
	RegisterTagItemType("TAG / TWR Callsign", RG_BREMEN_TAG_ITEM_TWR_CALLSIGN);
	RegisterTagItemType("TAG / TWR Altitude", RG_BREMEN_TAG_ITEM_TWR_ALTITUDE);
	RegisterTagItemType("TAG / TWR Indicated Airspeed", RG_BREMEN_TAG_ITEM_TWR_INDICATED_AIRSPEED);
}

void RGBremenPlugIn::InitializeSidStarHandler(std::string& pluginName)
{
	m_SidStarHandler = new SidStarHandler(m_Config);
	if (m_SidStarHandler->ParseReady()) {
		m_SidStarHandler->ParseDefinitionFile();
	}
	DisplayUserMessage(pluginName.c_str(), "SidStarHandler", m_SidStarHandler->GetStatus().c_str(), true, false, false, false, false);

	// Register List Items for Sid Star Checker
	RegisterTagItemType("LIST / SID Check", RG_BREMEN_LIST_ITEM_SID_CHECK);
	RegisterTagItemType("LIST / STAR Check", RG_BREMEN_LIST_ITEM_STAR_CHECK);
}

void RGBremenPlugIn::LoadConfiguration(std::string& pluginName)
{
	m_Config = new Config();
	DisplayUserMessage(pluginName.c_str(), "Configuration", "Configuration loaded from 'RG Bremen.json'", true, false, false, false, false);
	if (m_Config->GetDebugMode()) {
		DisplayUserMessage(pluginName.c_str(), "Configuration", "Debug mode active", true, false, false, false, false);
	}
}

void RGBremenPlugIn::InitializeAirspeedHandler()
{
	this->RegisterTagItemType("TAG / Calculated IAS", RG_BREMEN_TAG_ITEM_CALCULATED_IAS);
	this->RegisterTagItemType("TAG / Calculated IAS (togglable)", RG_BREMEN_TAG_ITEM_CALCULATED_IAS_TOGGLABLE);
	this->RegisterTagItemType("TAG / Calculated IAS (abbreviated)", RG_BREMEN_TAG_ITEM_CALCULATED_IAS_ABBREVIATED);
	this->RegisterTagItemType("TAG / Calculated IAS (abbreviated, togglable)", RG_BREMEN_TAG_ITEM_CALCULATED_IAS_ABBREVIATED_TOGGLABLE);
	this->RegisterTagItemType("TAG / Calculated Mach", RG_BREMEN_TAG_ITEM_CALCULATED_MACH);
	this->RegisterTagItemType("TAG / Calculated Mach (togglable)", RG_BREMEN_TAG_ITEM_CALCULATED_MACH_TOGGLABLE);
	this->RegisterTagItemType("TAG / Calculated Mach (above threshold)", RG_BREMEN_TAG_ITEM_CALCULATED_MACH_ABOVE_THRESHOLD);
	this->RegisterTagItemType("TAG / Calculated Mach (above threshold, togglable)", RG_BREMEN_TAG_ITEM_CALCULATED_MACH_ABOVE_THRESHOLD_TOGGLABLE);

	this->RegisterTagItemFunction("TAG FUNC / Open reported IAS menu", RG_BREMEN_TAG_ITEM_FUNC_OPEN_REPORTED_IAS_MENU);
	this->RegisterTagItemFunction("TAG FUNC / Clear reported IAS", RG_BREMEN_TAG_ITEM_FUNC_CLEAR_REPORTED_IAS);
	this->RegisterTagItemFunction("TAG FUNC / Toggle calculated IAS", RG_BREMEN_TAG_ITEM_FUNC_TOGGLE_CALCULATED_IAS);
	this->RegisterTagItemFunction("TAG FUNC / Toggle calculated IAS (abbreviated)", RG_BREMEN_TAG_ITEM_FUNC_TOGGLE_CALCULATED_IAS_ABBREVIATED);
	this->RegisterTagItemFunction("TAG FUNC / Open reported Mach menu", RG_BREMEN_TAG_ITEM_FUNC_OPEN_REPORTED_MACH_MENU);
	this->RegisterTagItemFunction("TAG FUNC / Clear reported Mach", RG_BREMEN_TAG_ITEM_FUNC_CLEAR_REPORTED_MACH);
	this->RegisterTagItemFunction("TAG FUNC / Toggle calculated Mach", RG_BREMEN_TAG_ITEM_FUNC_TOGGLE_CALCULATED_MACH);
	this->RegisterTagItemFunction("TAG FUNC / Toggle calculated Mach (above threshold)", RG_BREMEN_TAG_ITEM_FUNC_TOGGLE_CALCULATED_MACH_ABOVE_THRESHOLD);
	this->RegisterTagItemFunction("TAG FUNC / Toggle unreliable speed", RG_BREMEN_TAG_ITEM_FUNC_TOGGLE_UNRELIABLE_SPEED);

	nlohmann::json airspeedConfiguration = nullptr;
	airspeedConfiguration = m_Config->GetAirspeedConfiguration();
	if (airspeedConfiguration == nullptr) {
		LogMessage("Airspeed configuration not loaded. Please check RG Bremen.json file for the corresponding section.", "Configuration");
		return;
	}

	try
	{
		auto& machCfg = airspeedConfiguration.at("mach");
		int machDigits = machCfg.value<int>("digits", this->machDigits);
		if (machDigits < MIN_MACH_DIGITS || machDigits > MAX_MACH_DIGITS) {
			LogMessage("Invalid digit count for mach numbers. Must be between 1 and 13, falling back to default (2)", "Configuration");
		}
		else {
			this->machDigits = machDigits;
		}
		int machThresholdFL = machCfg.value<int>("thresholdFL", this->machThresholdFL);
		if (machThresholdFL < 0) {
			LogMessage("Invalid mach threshold flight level. Must be greater than 0, falling back to default (245)", "Configuration");
		}
		else {
			this->machThresholdFL = machThresholdFL * 100;
		}

		std::string prefixMach = machCfg.value<std::string>("prefix", this->prefixMach);
		if (prefixMach.size() > (size_t)(TAG_ITEM_MAX_CONTENT_LENGTH - this->machDigits)) {
			std::ostringstream msg;
			msg << "Mach number prefix is too long, must be " << (TAG_ITEM_MAX_CONTENT_LENGTH - this->machDigits) << " characters or less. Falling back to default (" << this->prefixMach << ")";
			LogMessage(msg.str(), "Configuration");
		}
		else {
			this->prefixMach = prefixMach;
		}

		std::string unreliableMachIndicator = machCfg.value<std::string>("unreliableIndicator", this->unreliableMachIndicator);
		if (unreliableMachIndicator.size() > (size_t)(TAG_ITEM_MAX_CONTENT_LENGTH)) {
			std::ostringstream msg;
			msg << "Unreliable Mach number indicator is too long, must be " << TAG_ITEM_MAX_CONTENT_LENGTH << " characters or less. Falling back to default (" << this->unreliableMachIndicator << ")";
			LogMessage(msg.str(), "Configuration");
		}
		else {
			this->unreliableMachIndicator = unreliableMachIndicator;
		}

		std::string unreliableMachColor = machCfg.value<std::string>("unreliableColor", "");
		if (unreliableMachColor.size() > 0) {
			this->unreliableMachColor = parseRGBString(unreliableMachColor);
			if (this->unreliableMachColor == nullptr) {
				LogMessage("Unreliable Mach number color is invalid, must be in comma-separated integer RGB format (e.g. \"123,123,123\"). Falling back to no color", "Configuration");
			}
		}
	}
	catch (const std::exception&)
	{
		if (m_Config->GetDebugMode()) {
			LogMessage("Unable to parse 'MACH' section of airspeed configuration.", "Configuration");
		}
	}

	try
	{
		auto& iasCfg = airspeedConfiguration.at("ias");

		std::string prefixIAS = iasCfg.value<std::string>("prefix", this->prefixIAS);
		if (prefixIAS.size() > (size_t)(TAG_ITEM_MAX_CONTENT_LENGTH - this->machDigits)) {
			std::ostringstream msg;
			msg << "Indicated air speed prefix is too long, must be " << (TAG_ITEM_MAX_CONTENT_LENGTH - this->machDigits) << " characters or less. Falling back to default (" << this->prefixIAS << ")";
			LogMessage(msg.str(), "Configuration");
		}
		else {
			this->prefixIAS = prefixIAS;
		}

		std::string unreliableIASIndicator = iasCfg.value<std::string>("unreliableIndicator", this->unreliableIASIndicator);
		if (unreliableIASIndicator.size() > (size_t)(TAG_ITEM_MAX_CONTENT_LENGTH)) {
			std::ostringstream msg;
			msg << "Unreliable IAS indicator is too long, must be " << TAG_ITEM_MAX_CONTENT_LENGTH << " characters or less. Falling back to default (" << this->unreliableIASIndicator << ")";
			this->LogMessage(msg.str(), "Configuration");
		}
		else {
			this->unreliableIASIndicator = unreliableIASIndicator;
		}

		std::string unreliableIASColor = iasCfg.value<std::string>("unreliableColor", "");
		if (unreliableIASColor.size() > 0) {
			this->unreliableIASColor = parseRGBString(unreliableIASColor);
			if (this->unreliableIASColor == nullptr) {
				this->LogMessage("Unreliable IAS color is invalid, must be in comma-separated (integer) RGB format (e.g. \"123,123,123\"). Falling back to no color", "Configuration");
			}
		}
	}
	catch (const std::exception&)
	{
		if (m_Config->GetDebugMode()) {
			LogMessage("Unable to parse 'IAS' section of airspeed configuration.", "Configuration");
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
			LogMessage("Unable to parse 'Weather' section of airspeed configuration.", "Configuration");
		}
	}

	try
	{
		auto& broadcastCfg = airspeedConfiguration.at("broadcast");

		this->broadcastUnreliableSpeed = broadcastCfg.value<bool>("unreliableSpeed", this->broadcastUnreliableSpeed);
	}
	catch (const std::exception&)
	{
		if (m_Config->GetDebugMode()) {
			LogMessage("Unable to parse 'Broadcast' section of airspeed configuration.", "Configuration");
		}
	}
}

RGBremenPlugIn::RGBremenPlugIn() : EuroScopePlugIn::CPlugIn(
		EuroScopePlugIn::COMPATIBILITY_CODE,
		RG_BREMEN_PLUGIN_NAME,
		RG_BREMEN_PLUGIN_VERSION,
		RG_BREMEN_PLUGIN_DEVELOPER,
		RG_BREMEN_PLUGIN_COPYRIGHT
	),
	weatherUpdateInterval(5),
	loginState(0),
	weatherUpdateHandler(nullptr),
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
	std::string pluginName = RG_BREMEN_PLUGIN_NAME;
	std::string version = RG_BREMEN_PLUGIN_VERSION;
	DisplayUserMessage(pluginName.c_str(), "Initialisation", ("Loading Version " + version).c_str(), true, false, false, false, false);

	// Load configuration
	LoadConfiguration(pluginName);

	// Load SidStarHandler
	InitializeSidStarHandler(pluginName);

	// Initialize the TagItemHandler
	InitializeTagItemHandler(pluginName);

	// Initialize Airspeed Calculation
	InitializeAirspeedHandler();

	DisplayUserMessage(pluginName.c_str(), "Initialisation", ("Version " + version + " loaded").c_str(), true, false, false, false, false);
}

void RGBremenPlugIn::LogMessage(std::string msg, std::string channel)
{
	DisplayUserMessage(RG_BREMEN_PLUGIN_NAME, channel.c_str(), msg.c_str(), true, false, false, false, false);
}

void RGBremenPlugIn::LogDebugMessage(std::string msg, std::string channel)
{
	if (m_Config->GetDebugMode()) {
		this->LogMessage(msg, channel);
	}
}

RGBremenPlugIn::~RGBremenPlugIn()
{
	// Stop Weather Update Handler
	this->StopWeatherUpdater();
	// Clear Tag Handler
	delete m_TagHandler;
	// Clear Sid Star Handler
	delete m_SidStarHandler;
	// Clear configuration
	m_Config->SaveConfiguration();
	delete m_Config;
}

void RGBremenPlugIn::OnControllerPositionUpdate(EuroScopePlugIn::CController Controller)
{
}

void RGBremenPlugIn::OnControllerDisconnect(EuroScopePlugIn::CController Controller)
{
}

void RGBremenPlugIn::OnRadarTargetPositionUpdate(EuroScopePlugIn::CRadarTarget RadarTarget)
{
}

void RGBremenPlugIn::OnFlightPlanDisconnect(EuroScopePlugIn::CFlightPlan FlightPlan)
{
}

void RGBremenPlugIn::OnFlightPlanFlightPlanDataUpdate(EuroScopePlugIn::CFlightPlan FlightPlan)
{
}

void RGBremenPlugIn::OnPlaneInformationUpdate(const char* sCallsign, const char* sLivery, const char* sPlaneType)
{
}

void RGBremenPlugIn::OnFlightPlanControllerAssignedDataUpdate(EuroScopePlugIn::CFlightPlan FlightPlan, int DataType)
{
	if (!FlightPlan.IsValid()) {
		return;
	}

	switch (DataType) {
	case EuroScopePlugIn::CTR_DATA_TYPE_SCRATCH_PAD_STRING:
		this->CheckScratchPadBroadcast(FlightPlan);
		break;
	}
}

void RGBremenPlugIn::OnFlightPlanFlightStripPushed(EuroScopePlugIn::CFlightPlan FlightPlan, const char* sSenderController, const char* sTargetController)
{
	if (!FlightPlan.IsValid()) {
		return;
	}

	auto cad = FlightPlan.GetControllerAssignedData();
	if (strcmp(sTargetController, this->ControllerMyself().GetCallsign()) == 0) {
		// Tag is pushed towards us
		this->CheckFlightStripAnnotations(FlightPlan);
	}
}

EuroScopePlugIn::CRadarScreen* RGBremenPlugIn::OnRadarScreenCreated(const char* sDisplayName, bool NeedRadarContent, bool GeoReferenced, bool CanBeSaved, bool CanBeCreated)
{
	return NULL;
}

bool RGBremenPlugIn::OnCompileCommand(const char* sCommandLine)
{
	std::regex regex("[\\s]+");
	std::vector<std::string> sCommandLineSplit = Tokenize(sCommandLine, regex);

	// In debug mode:
	// Display the tokenized substrings
	if (m_Config->GetDebugMode()) {
		for (uint32_t i = 0; i < sCommandLineSplit.size(); i++) {
			DisplayUserMessage(RG_BREMEN_PLUGIN_NAME, RG_BREMEN_PLUGIN_NAME, sCommandLineSplit.at(i).c_str(), true, false, false, false, false);
		}
	}

	if (sCommandLineSplit.at(0) == ".rgbremen" && sCommandLineSplit.size() >= 1) {
		// reload main configuration from file
		if (sCommandLineSplit.at(1) == "config" && sCommandLineSplit.at(2) == "reload") {
			m_Config->ReloadConfigurationFromFile();
			DisplayUserMessage(RG_BREMEN_PLUGIN_NAME, "Configuration", "Configuration reloaded from 'RG Bremen.json'", true, false, false, false, false);
			return true;
		}
		// Enable / Disable custom color flag
		if (sCommandLineSplit.at(1) == "color") {
			m_Config->ToggleCustomColorsUsageFlag();
			DisplayUserMessage(RG_BREMEN_PLUGIN_NAME, "Configuration", "Toggled custom color usage.", true, false, false, false, false);
			return true;
		}
		// Sidstar handler commands
		if (sCommandLineSplit.at(1) == "sidstar") {
			// Force sid star definition file to redownload
			if (sCommandLineSplit.at(2) == "download") {
				m_SidStarHandler->DownloadDefinitionFile();
				DisplayUserMessage(RG_BREMEN_PLUGIN_NAME, "SidStarHandler", m_SidStarHandler->GetStatus().c_str(), true, false, false, false, false);
				return true;
			}
			// Parse the definition file
			if (sCommandLineSplit.at(2) == "parse") {
				if (m_SidStarHandler->ParseReady())
					m_SidStarHandler->ParseDefinitionFile();
				DisplayUserMessage(RG_BREMEN_PLUGIN_NAME, "SidStarHandler", m_SidStarHandler->GetStatus().c_str(), true, false, false, false, false);
				return true;
			}
		}
		return true;
	}

	return false;
}

void RGBremenPlugIn::OnCompileFrequencyChat(const char* sSenderCallsign, double Frequency, const char* sChatMessage)
{
}

void RGBremenPlugIn::OnCompilePrivateChat(const char* sSenderCallsign, const char* sReceiverCallsign, const char* sChatMessage)
{
}

void RGBremenPlugIn::OnGetTagItem(EuroScopePlugIn::CFlightPlan FlightPlan, EuroScopePlugIn::CRadarTarget RadarTarget, int ItemCode, int TagData, char sItemString[16], int* pColorCode, COLORREF* pRGB, double* pFontSize)
{
	if (!FlightPlan.IsValid()) return;

	// Handle LIST items first
	if (ItemCode == RG_BREMEN_LIST_ITEM_SID_CHECK) {
		if (m_SidStarHandler->AnalyzeSid(FlightPlan))
		{
			if(m_Config->GetCustomColorsUsageFlag())
			{
				*pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
				*pRGB = RGB(m_Config->GetListColorSidStarYes()[0], m_Config->GetListColorSidStarYes()[1], m_Config->GetListColorSidStarYes()[2]);
			}
			strcpy_s(sItemString, 16, "YES");
		}
		else {
			if(m_Config->GetCustomColorsUsageFlag())
			{
				*pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
				*pRGB = RGB(m_Config->GetListColorSidStarNo()[0], m_Config->GetListColorSidStarNo()[1], m_Config->GetListColorSidStarNo()[2]);
			}
			strcpy_s(sItemString, 16, "NO");
		}
		return;
	}
	if (ItemCode == RG_BREMEN_LIST_ITEM_STAR_CHECK) {
		if (m_SidStarHandler->AnalyzeStar(FlightPlan))
		{
			if(m_Config->GetCustomColorsUsageFlag())
			{
				*pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
				*pRGB = RGB(m_Config->GetListColorSidStarYes()[0], m_Config->GetListColorSidStarYes()[1], m_Config->GetListColorSidStarYes()[2]);
			}
			strcpy_s(sItemString, 16, "YES");
		}
		else {
			if(m_Config->GetCustomColorsUsageFlag())
			{
				*pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
				*pRGB = RGB(m_Config->GetListColorSidStarNo()[0], m_Config->GetListColorSidStarNo()[1], m_Config->GetListColorSidStarNo()[2]);
			}
			strcpy_s(sItemString, 16, "NO");
		}
		return;
	}
	
	// Handle all other Tag cases
	// within the TagHandler class
	TagItemContainer tic = m_TagHandler->HandleTagItem(FlightPlan, RadarTarget, ItemCode);
	/*if (m_Config->GetDebugMode() && tic.handled) {
		char* tmp = new char[15];
		_itoa_s(ItemCode, tmp, 15, 10);
		DisplayUserMessage(RG_BREMEN_PLUGIN_NAME, "OnGetTagItem", "Received TagItemCode for", true, false, false, false, false);
		DisplayUserMessage(RG_BREMEN_PLUGIN_NAME, "OnGetTagItem", tmp, true, false, false, false, false);
		DisplayUserMessage(RG_BREMEN_PLUGIN_NAME, "OnGetTagItem", tic.sItemString.c_str(), true, false, false, false, false);
	}*/

	if (tic.handled && tic.sItemString.size() > 0) {
		if (tic.customColor && m_Config->GetCustomColorsUsageFlag()) {
			*pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
			*pRGB = tic.color;
		}
		strncpy_s(sItemString, 15, (tic.sItemString + "\0").c_str(), 15);
	}
	// Try to override tag item color if custom colors are used and the aircraft is VFR
	if (SquawkHandler::IsSquawkVFR(atoi(RadarTarget.GetPosition().GetSquawk())) && (!tic.handled || (tic.handled && !tic.customColor))) {
		if (m_Config->GetCustomColorsUsageFlag() && FlightPlan.GetFlightPlanData().GetPlanType()[0] != 'I') {
			*pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
			*pRGB = RGB(m_Config->GetTagColorVFR()[0], m_Config->GetTagColorVFR()[1], m_Config->GetTagColorVFR()[2]);
		}
	}

	// Handle Airspeed Indication Items
	switch (ItemCode) {
	case RG_BREMEN_TAG_ITEM_CALCULATED_IAS:
		this->ShowCalculatedIAS(RadarTarget, sItemString, pColorCode, pRGB);
		break;
	case RG_BREMEN_TAG_ITEM_CALCULATED_IAS_ABBREVIATED:
		this->ShowCalculatedIAS(RadarTarget, sItemString, pColorCode, pRGB, true);
		break;
	case RG_BREMEN_TAG_ITEM_CALCULATED_IAS_TOGGLABLE:
		this->ShowCalculatedIAS(RadarTarget, sItemString, pColorCode, pRGB, false, true);
		break;
	case RG_BREMEN_TAG_ITEM_CALCULATED_IAS_ABBREVIATED_TOGGLABLE:
		this->ShowCalculatedIAS(RadarTarget, sItemString, pColorCode, pRGB, true, true);
		break;
	case RG_BREMEN_TAG_ITEM_CALCULATED_MACH:
		this->ShowCalculatedMach(RadarTarget, sItemString, pColorCode, pRGB);
		break;
	case RG_BREMEN_TAG_ITEM_CALCULATED_MACH_ABOVE_THRESHOLD:
		this->ShowCalculatedMach(RadarTarget, sItemString, pColorCode, pRGB, true);
		break;
	case RG_BREMEN_TAG_ITEM_CALCULATED_MACH_TOGGLABLE:
		this->ShowCalculatedMach(RadarTarget, sItemString, pColorCode, pRGB, false, true);
		break;
	case RG_BREMEN_TAG_ITEM_CALCULATED_MACH_ABOVE_THRESHOLD_TOGGLABLE:
		this->ShowCalculatedMach(RadarTarget, sItemString, pColorCode, pRGB, true, true);
		break;
	}

	// Not our tag item
	return;
}

void RGBremenPlugIn::OnRefreshFpListContent(EuroScopePlugIn::CFlightPlanList AcList)
{
}

void RGBremenPlugIn::OnNewMetarReceived(const char* sStation, const char* sFullMetar)
{
}

void RGBremenPlugIn::OnFunctionCall(int FunctionId, const char* sItemString, POINT Pt, RECT Area)
{
	EuroScopePlugIn::CFlightPlan fp = this->FlightPlanSelectASEL();
	if (!fp.IsValid()) {
		return;
	}

	switch (FunctionId)
	{
	case RG_BREMEN_TAG_ITEM_FUNC_OPEN_REPORTED_IAS_MENU:
	{
		EuroScopePlugIn::CRadarTarget rt = fp.GetCorrelatedRadarTarget();
		if (!rt.IsValid()) {
			return;
		}
		int ias = this->useReportedGS ? rt.GetPosition().GetReportedGS() : rt.GetGS();
		double calcIAS = this->CalculateIAS(rt);
		if (calcIAS >= 0) {
			ias = roundToNearest(calcIAS, INTERVAL_REPORTED_IAS);
		}
		this->OpenPopupList(Area, "RIAS", 1);
		for (int i = MAX_REPORTED_IAS; i >= MIN_REPORTED_IAS; i -= INTERVAL_REPORTED_IAS) {
			this->AddPopupListElement(std::to_string(i).c_str(), nullptr, RG_BREMEN_TAG_ITEM_FUNC_SET_REPORTED_IAS, i >= ias && ias >= i - INTERVAL_REPORTED_IAS, EuroScopePlugIn::POPUP_ELEMENT_NO_CHECKBOX, false, false);
		}
		this->AddPopupListElement("---", nullptr, 0, false, EuroScopePlugIn::POPUP_ELEMENT_NO_CHECKBOX, true, true);
		this->AddPopupListElement("Clear", nullptr, RG_BREMEN_TAG_ITEM_FUNC_CLEAR_REPORTED_IAS, false, EuroScopePlugIn::POPUP_ELEMENT_NO_CHECKBOX, false, true);
		break;
	}
	case RG_BREMEN_TAG_ITEM_FUNC_CLEAR_REPORTED_IAS:
		this->ClearReportedIAS(fp);
		break;
	case RG_BREMEN_TAG_ITEM_FUNC_TOGGLE_CALCULATED_IAS:
		this->ToggleCalculatedIAS(fp, false);
		break;
	case RG_BREMEN_TAG_ITEM_FUNC_TOGGLE_CALCULATED_IAS_ABBREVIATED:
		this->ToggleCalculatedIAS(fp, true);
		break;
	case RG_BREMEN_TAG_ITEM_FUNC_SET_REPORTED_IAS:
		this->SetReportedIAS(fp, sItemString);
		break;
	case RG_BREMEN_TAG_ITEM_FUNC_OPEN_REPORTED_MACH_MENU:
	{
		EuroScopePlugIn::CRadarTarget rt = fp.GetCorrelatedRadarTarget();
		if (!rt.IsValid()) {
			return;
		}

		int mach = 0;
		double calculatedMach = this->CalculateMach(rt);
		if (calculatedMach >= 0) {
			mach = roundToNearest(calculatedMach * 100, INTERVAL_REPORTED_MACH);
		}

		this->OpenPopupList(Area, "RMCH", 1);
		for (int i = MAX_REPORTED_MACH; i >= MIN_REPORTED_MACH; i -= INTERVAL_REPORTED_MACH) {
			this->AddPopupListElement(std::to_string(i).c_str(), nullptr, RG_BREMEN_TAG_ITEM_FUNC_SET_REPORTED_MACH, i >= mach && mach >= i - INTERVAL_REPORTED_MACH, EuroScopePlugIn::POPUP_ELEMENT_NO_CHECKBOX, false, false);
		}
		this->AddPopupListElement("---", nullptr, 0, false, EuroScopePlugIn::POPUP_ELEMENT_NO_CHECKBOX, true, true);
		this->AddPopupListElement("Clear", nullptr, RG_BREMEN_TAG_ITEM_FUNC_CLEAR_REPORTED_MACH, false, EuroScopePlugIn::POPUP_ELEMENT_NO_CHECKBOX, false, true);

		break;
	}
	case RG_BREMEN_TAG_ITEM_FUNC_CLEAR_REPORTED_MACH:
		this->ClearReportedMach(fp);
		break;
	case RG_BREMEN_TAG_ITEM_FUNC_TOGGLE_CALCULATED_MACH:
		this->ToggleCalculatedMach(fp);
		break;
	case RG_BREMEN_TAG_ITEM_FUNC_TOGGLE_CALCULATED_MACH_ABOVE_THRESHOLD:
		this->ToggleCalculatedMach(fp, true);
		break;
	case RG_BREMEN_TAG_ITEM_FUNC_SET_REPORTED_MACH:
		this->SetReportedMach(fp, sItemString);
		break;
	case RG_BREMEN_TAG_ITEM_FUNC_TOGGLE_UNRELIABLE_SPEED:
		this->ToggleUnreliableSpeed(fp);
		break;
	default:
		break;
	}
}

void RGBremenPlugIn::OnTimer(int Counter)
{
	if (Counter % 60 == 0) {
		m_TagHandler->SetTransitionAltitude(GetTransitionAltitude());
	}
	if (Counter % 2 == 0) {
		this->UpdateLoginState();
	}
}

void RGBremenPlugIn::SetReportedIAS(const EuroScopePlugIn::CFlightPlan& fp, std::string selected)
{
	int ias;
	try {
		ias = std::stoi(selected);
	}
	catch (std::exception const& ex) {
		std::ostringstream msg;
		msg << "Failed to parse reported IAS: " << ex.what();

		this->LogMessage(msg.str(), "Airspeed");
		return;
	}

	this->reportedIAS.insert_or_assign(fp.GetCallsign(), ias);
}

void RGBremenPlugIn::ClearReportedIAS(const EuroScopePlugIn::CFlightPlan& fp)
{
	this->reportedIAS.erase(fp.GetCallsign());
}

void RGBremenPlugIn::ToggleCalculatedIAS(const EuroScopePlugIn::CFlightPlan& fp, bool abbreviated)
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

double RGBremenPlugIn::CalculateIAS(const EuroScopePlugIn::CRadarTarget& rt)
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

void RGBremenPlugIn::ShowCalculatedIAS(const EuroScopePlugIn::CRadarTarget& rt, char tagItemContent[16], int* tagItemColorCode, COLORREF* tagItemRGB, bool abbreviated, bool onlyToggled)
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

void RGBremenPlugIn::SetReportedMach(const EuroScopePlugIn::CFlightPlan& fp, std::string selected)
{
	int mach;
	try {
		mach = std::stoi(selected);
	}
	catch (std::exception const& ex) {
		std::ostringstream msg;
		msg << "Failed to parse reported Mach: " << ex.what();

		this->LogMessage(msg.str(), "Airspeed");
		return;
	}

	this->reportedMach.insert_or_assign(fp.GetCallsign(), (double)mach / 100.0);
}

void RGBremenPlugIn::ClearReportedMach(const EuroScopePlugIn::CFlightPlan& fp)
{
	this->reportedMach.erase(fp.GetCallsign());
}

void RGBremenPlugIn::ToggleCalculatedMach(const EuroScopePlugIn::CFlightPlan& fp, bool aboveThreshold)
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

double RGBremenPlugIn::CalculateMach(const EuroScopePlugIn::CRadarTarget& rt)
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

void RGBremenPlugIn::ShowCalculatedMach(const EuroScopePlugIn::CRadarTarget& rt, char tagItemContent[16], int* tagItemColorCode, COLORREF* tagItemRGB, bool aboveThreshold, bool onlyToggled)
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

void RGBremenPlugIn::ToggleUnreliableSpeed(const EuroScopePlugIn::CFlightPlan& fp)
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

void RGBremenPlugIn::BroadcastScratchPad(const EuroScopePlugIn::CFlightPlan& fp, std::string msg)
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
		this->LogMessage("Failed to set broadcast message in scratch pad", fp.GetCallsign());
	}

	if (!cad.SetScratchPadString(scratch.c_str())) {
		this->LogMessage("Failed to reset scratch pad after setting broadcast message", fp.GetCallsign());
	}
}

void RGBremenPlugIn::CheckScratchPadBroadcast(const EuroScopePlugIn::CFlightPlan& fp)
{
	std::vector<std::string> scratch = split(fp.GetControllerAssignedData().GetScratchPadString(), BROADCAST_DELIMITER);

	if (scratch.size() < 3 || scratch[0] != BROADCAST_PREFIX) {
		return;
	}

	if (this->broadcastUnreliableSpeed && scratch[1] == BROADCAST_UNRELIABLE_SPEED) {
		if (scratch[2] == "1") {
			this->LogDebugMessage("Enabling unreliable speed indication for aircraft after broadcast", fp.GetCallsign());
			this->unreliableSpeedToggled.insert(fp.GetCallsign());
		}
		else {
			this->LogDebugMessage("Disabling unreliable speed indication for aircraft after broadcast", fp.GetCallsign());
			this->unreliableSpeedToggled.erase(fp.GetCallsign());
		}
	}
}

void RGBremenPlugIn::SetFlightStripAnnotation(const EuroScopePlugIn::CFlightPlan& fp, std::string msg, int index)
{
	if (!fp.IsValid()) {
		return;
	}

	if (!fp.GetTrackingControllerIsMe() && strcmp(fp.GetTrackingControllerId(), "") != 0) {
		return;
	}

	auto cad = fp.GetControllerAssignedData();

	if (!cad.SetFlightStripAnnotation(index, msg.c_str())) {
		this->LogMessage("Failed to set message in flight strip annotations", fp.GetCallsign());
	}
}

void RGBremenPlugIn::CheckFlightStripAnnotations(const EuroScopePlugIn::CFlightPlan& fp)
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
			this->LogDebugMessage("Enabling unreliable speed indication for aircraft due to flight strip annotation", fp.GetCallsign());
			this->unreliableSpeedToggled.insert(fp.GetCallsign());
		}
		else {
			this->LogDebugMessage("Disabling unreliable speed indication for aircraft due to empty flight strip annotation", fp.GetCallsign());
			this->unreliableSpeedToggled.erase(fp.GetCallsign());
		}
	}
}

void RGBremenPlugIn::CheckFlightStripAnnotationsForAllAircraft()
{
	if (this->broadcastUnreliableSpeed) {
		this->unreliableSpeedToggled.clear();

		for (EuroScopePlugIn::CFlightPlan fp = this->FlightPlanSelectFirst(); fp.IsValid(); fp = this->FlightPlanSelectNext(fp)) {
			this->CheckFlightStripAnnotations(fp);
		}
	}
}

void RGBremenPlugIn::UpdateLoginState()
{
	if (this->loginState == this->GetConnectionType()) {
		return;
	}

	this->loginState = this->GetConnectionType();

	this->CheckLoginState();
}

void RGBremenPlugIn::CheckLoginState()
{
	switch (this->loginState)
	{
	case EuroScopePlugIn::CONNECTION_TYPE_DIRECT:
	case EuroScopePlugIn::CONNECTION_TYPE_VIA_PROXY:
		this->StartWeatherUpdater();
		//this->CheckFlightStripAnnotationsForAllAircraft();
		break;
	default:
		this->StopWeatherUpdater();
		break;
	}
}

void RGBremenPlugIn::UpdateWeather()
{
	this->LogDebugMessage("Retrieving weather data", "Weather");

	std::string weatherJSON;
	try {
		weatherJSON = Http::get(this->weatherUpdateURL);
	}
	catch (std::exception ex) {
		this->LogMessage("Failed to load weather data", "Weather");
		this->LogDebugMessage(ex.what(), "Weather");
		return;
	}

	this->LogDebugMessage("Parsing weather data", "Weather");
	try {
		this->weather.parse(weatherJSON);
	}
	catch (std::exception ex) {
		this->LogMessage("Failed to parse weather data", "Weather");
		this->LogDebugMessage(ex.what(), "Weather");
		return;
	}

	this->LogDebugMessage("Successfully updated weather data", "Weather");
}

void RGBremenPlugIn::StartWeatherUpdater()
{
	if (this->weatherUpdateURL.empty() && this->weatherUpdateInterval.count() > 0) {
		this->LogMessage("Weather update URL is empty, cannot fetch weather data for calculations. Configure via config file (RG Bremen.json in same directory as RG Bremen EuroScopePlugIn.dll).", "Configuration");
		return;
	}

	if (this->weatherUpdateHandler == nullptr && this->weatherUpdateInterval.count() > 0) {
		this->weatherUpdateHandler = new Threading::PeriodicAction(std::chrono::milliseconds(0), std::chrono::milliseconds(this->weatherUpdateInterval), std::bind(&RGBremenPlugIn::UpdateWeather, this));
	}
}

void RGBremenPlugIn::StopWeatherUpdater()
{
	if (this->weatherUpdateHandler != nullptr) {
		this->weatherUpdateHandler->Stop();
		delete this->weatherUpdateHandler;
		this->weatherUpdateHandler = nullptr;
	}
}

void RGBremenPlugIn::ResetWeatherUpdater()
{
	this->StopWeatherUpdater();
	this->CheckLoginState();
}
