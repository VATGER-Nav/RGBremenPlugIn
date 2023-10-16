#include "stdafx.h"
#include "RGBremenPlugIn.h"

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

	m_AirspeedHandler = new TrueAirspeed(m_Config);
	if (m_AirspeedHandler->messageMap.size() > 0) {
		for (int i = 0; i < m_AirspeedHandler->messageMap.size(); i++) {
			LogMessage(m_AirspeedHandler->messageMap.at(i), "AirSpeedHandler");
		}
		m_AirspeedHandler->messageMap.clear();
	}
	LogMessage("AirSpeedHandler initialized", "AirSpeedHandler");
}

RGBremenPlugIn::RGBremenPlugIn() : EuroScopePlugIn::CPlugIn(
		EuroScopePlugIn::COMPATIBILITY_CODE,
		RG_BREMEN_PLUGIN_NAME,
		RG_BREMEN_PLUGIN_VERSION,
		RG_BREMEN_PLUGIN_DEVELOPER,
		RG_BREMEN_PLUGIN_COPYRIGHT
	),
	nextSectorUpdateInterval(3),
	nextSectorUpdateHandler(nullptr)
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

	// Load LoA Definition
	m_LoaDefinition = new LetterOfAgreement::LoaDefinition();
	LogMessage("Loaded LoA", "Next Sector Prediction");

	// Init DeliveryHelper
	this->RegisterTagItemType("LIST / Flightplan Validation", RG_BREMEN_LIST_ITEM_FLIGHTPLAN_VALIDATION);
	this->RegisterTagItemFunction("LIST FUNC / Validation menu", RG_BREMEN_LIST_ITEM_FUNC_FLIGHTPLAN_VALIDATION_MENU);
	this->RegisterTagItemFunction("LIST FUNC / Process FPL", RG_BREMEN_LIST_ITEM_FUNC_FLIGHTPLAN_PROCESS_FP);
	this->RegisterTagItemFunction("LIST FUNC / Process FPL (non-NAP)", RG_BREMEN_LIST_ITEM_FUNC_FLIGHTPLAN_PROCESS_FP_NON_NAP);
	this->RegisterTagItemFunction("LIST FUNC / Process FPL (NAP)", RG_BREMEN_LIST_ITEM_FUNC_FLIGHTPLAN_PROCESS_FP_NAP);
	LogDebugMessage("Registered LIST FUNCs for DelHel", "DelHel");
	m_DelHel = new DeliveryHelper(m_Config);

	while (m_DelHel->HasMessage()) {
		LogDebugMessage(m_DelHel->GetNextMessage(), "DelHel");
	}

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
	// Remove DelHel
	delete m_DelHel;
	// Stop Weather Update Handler
	m_AirspeedHandler->StopWeatherUpdater();
	delete m_AirspeedHandler;
	// Stop next sector calculation
	if(nextSectorUpdateHandler != nullptr)
		nextSectorUpdateHandler->Stop();
	// Clear Tag Handler
	delete m_TagHandler;
	// Clear Sid Star Handler
	delete m_SidStarHandler;
	// Clear configuration
	m_Config->SaveConfiguration();
	delete m_Config;
}

void RGBremenPlugIn::OnAirportRunwayActivityChanged()
{
	// Make sure current sector file is selected
	if (m_DelHel != nullptr) {
		this->SelectActiveSectorfile();
		for (auto sfe = this->SectorFileElementSelectFirst(EuroScopePlugIn::SECTOR_ELEMENT_RUNWAY); sfe.IsValid(); sfe = this->SectorFileElementSelectNext(sfe, EuroScopePlugIn::SECTOR_ELEMENT_RUNWAY)) {
			m_DelHel->UpdateActiveAirports(sfe);
		}
	}
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
	if(m_DelHel != nullptr)
		m_DelHel->FlightPlanClosed(FlightPlan);
	calculatedNextSectors.erase(FlightPlan.GetCallsign());
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
		m_AirspeedHandler->CheckScratchPadBroadcast(FlightPlan);
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
		m_AirspeedHandler->CheckFlightStripAnnotations(FlightPlan);
		// Update next sector prediction once for this flight
		calculatedNextSectors.emplace(FlightPlan.GetCallsign(), CalculateNextSector(FlightPlan, FlightPlan.GetCorrelatedRadarTarget()));
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
	if (ItemCode == RG_BREMEN_LIST_ITEM_FLIGHTPLAN_VALIDATION && m_DelHel != nullptr) {
		Validation v = m_DelHel->ProcessFlightPlan(FlightPlan, false, true);
		if (v.valid && m_DelHel->IsFlightPlanProcessed(FlightPlan)) {
			if (v.tag.empty()) {
				strcpy_s(sItemString, 16, "OK");
			}
			else
			{
				strcpy_s(sItemString, 16, v.tag.c_str());
			}

			*pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;

			if (v.color == RGB(200, 200, 200)) {
				*pRGB = RGB(0, 200, 0);
			}
			else {
				*pRGB = v.color;
			}
		}
		else
		{
			strcpy_s(sItemString, 16, v.tag.c_str());

			if (v.color != RGB(200, 200, 200)) {
				*pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
				*pRGB = v.color;
			}
		}
		return;
	}
	
	// Handle all other Tag cases
	// within the TagHandler class
	TagItemContainer tic = m_TagHandler->HandleTagItem(FlightPlan, RadarTarget, ItemCode);

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

	// Handle Next Sector Preview
	//NextSectorStructure nss = CalculateNextSector(FlightPlan, RadarTarget);
	NextSectorStructure nss;
	for (auto const& [cs, ns] : calculatedNextSectors) {
		if (cs == FlightPlan.GetCallsign() && cs == RadarTarget.GetCallsign()) {
			nss = ns;
		}
	}

	switch (ItemCode)
	{
	case RG_BREMEN_TAG_ITEM_NEXT_SECTOR:
		if(nss.isValid) {
			std::stringstream ss;
			ss << nss.nextSectorId;
			strncpy_s(sItemString, 15, ss.str().c_str(), 15);
		}
		break;
	case RG_BREMEN_TAG_ITEM_TRANSFER_FLIGHT_LEVEL:
		if (nss.isValid) {
			std::stringstream ss;
			//ss << "@FL" << nss.copAltitude / 1000 << (nss.clbDesc > 0) ? "^" : (nss.clbDesc < 0) ? "|" : "-";
			ss << nss.copAltitude / 1000 << (nss.clbDesc > 0) ? "^" : (nss.clbDesc < 0) ? "|" : "-"; // Disable @FL for Chris
			strncpy_s(sItemString, 15, ss.str().c_str(), 15);
		}
		break;
	case RG_BREMEN_TAG_ITEM_COPX:
		if (nss.isValid) {
			std::stringstream ss;
			ss << nss.copn;
			strncpy_s(sItemString, 15, ss.str().c_str(), 15);
		}
		break;
	default:
		break;
	}

	// Handle Airspeed Indication Items
	switch (ItemCode) {
	case RG_BREMEN_TAG_ITEM_CALCULATED_IAS:
		m_AirspeedHandler->ShowCalculatedIAS(RadarTarget, sItemString, pColorCode, pRGB);
		break;
	case RG_BREMEN_TAG_ITEM_CALCULATED_IAS_ABBREVIATED:
		m_AirspeedHandler->ShowCalculatedIAS(RadarTarget, sItemString, pColorCode, pRGB, true);
		break;
	case RG_BREMEN_TAG_ITEM_CALCULATED_IAS_TOGGLABLE:
		m_AirspeedHandler->ShowCalculatedIAS(RadarTarget, sItemString, pColorCode, pRGB, false, true);
		break;
	case RG_BREMEN_TAG_ITEM_CALCULATED_IAS_ABBREVIATED_TOGGLABLE:
		m_AirspeedHandler->ShowCalculatedIAS(RadarTarget, sItemString, pColorCode, pRGB, true, true);
		break;
	case RG_BREMEN_TAG_ITEM_CALCULATED_MACH:
		m_AirspeedHandler->ShowCalculatedMach(RadarTarget, sItemString, pColorCode, pRGB);
		break;
	case RG_BREMEN_TAG_ITEM_CALCULATED_MACH_ABOVE_THRESHOLD:
		m_AirspeedHandler->ShowCalculatedMach(RadarTarget, sItemString, pColorCode, pRGB, true);
		break;
	case RG_BREMEN_TAG_ITEM_CALCULATED_MACH_TOGGLABLE:
		m_AirspeedHandler->ShowCalculatedMach(RadarTarget, sItemString, pColorCode, pRGB, false, true);
		break;
	case RG_BREMEN_TAG_ITEM_CALCULATED_MACH_ABOVE_THRESHOLD_TOGGLABLE:
		m_AirspeedHandler->ShowCalculatedMach(RadarTarget, sItemString, pColorCode, pRGB, true, true);
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
		int ias = m_AirspeedHandler->useReportedGS ? rt.GetPosition().GetReportedGS() : rt.GetGS();
		double calcIAS = m_AirspeedHandler->CalculateIAS(rt);
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
		m_AirspeedHandler->ClearReportedIAS(fp);
		break;
	case RG_BREMEN_TAG_ITEM_FUNC_TOGGLE_CALCULATED_IAS:
		m_AirspeedHandler->ToggleCalculatedIAS(fp, false);
		break;
	case RG_BREMEN_TAG_ITEM_FUNC_TOGGLE_CALCULATED_IAS_ABBREVIATED:
		m_AirspeedHandler->ToggleCalculatedIAS(fp, true);
		break;
	case RG_BREMEN_TAG_ITEM_FUNC_SET_REPORTED_IAS:
		m_AirspeedHandler->SetReportedIAS(fp, sItemString);
		break;
	case RG_BREMEN_TAG_ITEM_FUNC_OPEN_REPORTED_MACH_MENU:
	{
		EuroScopePlugIn::CRadarTarget rt = fp.GetCorrelatedRadarTarget();
		if (!rt.IsValid()) {
			return;
		}

		int mach = 0;
		double calculatedMach = m_AirspeedHandler->CalculateMach(rt);
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
		m_AirspeedHandler->ClearReportedMach(fp);
		break;
	case RG_BREMEN_TAG_ITEM_FUNC_TOGGLE_CALCULATED_MACH:
		m_AirspeedHandler->ToggleCalculatedMach(fp);
		break;
	case RG_BREMEN_TAG_ITEM_FUNC_TOGGLE_CALCULATED_MACH_ABOVE_THRESHOLD:
		m_AirspeedHandler->ToggleCalculatedMach(fp, true);
		break;
	case RG_BREMEN_TAG_ITEM_FUNC_SET_REPORTED_MACH:
		m_AirspeedHandler->SetReportedMach(fp, sItemString);
		break;
	case RG_BREMEN_TAG_ITEM_FUNC_TOGGLE_UNRELIABLE_SPEED:
		m_AirspeedHandler->ToggleUnreliableSpeed(fp);
		break;
	case RG_BREMEN_LIST_ITEM_FUNC_FLIGHTPLAN_PROCESS_FP:
		m_DelHel->ProcessFlightPlan(fp, false);
		break;
	case RG_BREMEN_LIST_ITEM_FUNC_FLIGHTPLAN_PROCESS_FP_NAP:
		m_DelHel->ProcessFlightPlan(fp, true);
		break;
	case RG_BREMEN_LIST_ITEM_FUNC_FLIGHTPLAN_PROCESS_FP_NON_NAP:
		m_DelHel->ProcessFlightPlan(fp, false);
		break;
	case RG_BREMEN_LIST_ITEM_FUNC_FLIGHTPLAN_VALIDATION_MENU:
		this->OpenPopupList(Area, "Validation", 1);
		this->AddPopupListElement("Process FPL (NAP)", NULL, RG_BREMEN_LIST_ITEM_FUNC_FLIGHTPLAN_PROCESS_FP_NAP, false, EuroScopePlugIn::POPUP_ELEMENT_NO_CHECKBOX, false, false);
		this->AddPopupListElement("Process FPL (non-NAP)", NULL, RG_BREMEN_LIST_ITEM_FUNC_FLIGHTPLAN_PROCESS_FP_NON_NAP, false, EuroScopePlugIn::POPUP_ELEMENT_NO_CHECKBOX, false, false);
		break;
	default:
		break;
	}
}

void RGBremenPlugIn::OnTimer(int Counter)
{
	if (Counter >= INT32_MAX - 1) return;

	if (Counter % 60 == 0) {
		m_TagHandler->SetTransitionAltitude(GetTransitionAltitude());
	}
	if (Counter % 2 == 0) {
		this->UpdateLoginState();
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
	case EuroScopePlugIn::CONNECTION_TYPE_SWEATBOX:
	case EuroScopePlugIn::CONNECTION_TYPE_PLAYBACK:
	{
		m_AirspeedHandler->StartWeatherUpdater();
		std::vector<EuroScopePlugIn::CFlightPlan> flightPlans;
		for (EuroScopePlugIn::CFlightPlan fp = this->FlightPlanSelectFirst(); fp.IsValid(); fp = this->FlightPlanSelectNext(fp)) {
			flightPlans.push_back(fp);
		}
		m_AirspeedHandler->CheckFlightStripAnnotationsForAllAircraft(flightPlans);

		// LoA Next Sector Prediction
		if (m_LoaDefinition != nullptr) {
			if (nextSectorUpdateHandler == nullptr && nextSectorUpdateInterval.count() > 0) {
				nextSectorUpdateHandler = new Threading::PeriodicAction(std::chrono::milliseconds(100), std::chrono::milliseconds(this->nextSectorUpdateInterval), std::bind(&RGBremenPlugIn::UpdateNextSectorPredictionForAllAircraft, this));
				LogDebugMessage("Started Next Sector prediction", "LoA");
			}
		}
		if (m_Config->GetDebugMode()) {
			std::stringstream ss;
			ss << "Loginstate check: DIRECT|PROXY @ position: " << ControllerMyself().GetPositionId();
			LogDebugMessage(ss.str(), "State");
		}
		break;
	}
	default:
		m_AirspeedHandler->StopWeatherUpdater();
		if (nextSectorUpdateHandler != nullptr) {
			nextSectorUpdateHandler->Stop();
			calculatedNextSectors.clear();
		}
		break;
	}
}

void RGBremenPlugIn::UpdateNextSectorPredictionForAllAircraft()
{
	LogDebugMessage("Calculating next sectors", "LoA");
	//calculatedNextSectors.clear();
	for (EuroScopePlugIn::CFlightPlan fp = this->FlightPlanSelectFirst(); fp.IsValid(); fp = this->FlightPlanSelectNext(fp)) {
		NextSectorStructure nss = this->CalculateNextSector(fp, fp.GetCorrelatedRadarTarget());
		if (nss.nextSectorId.size() > 0) {
			nss.isValid = true;

			calculatedNextSectors.emplace(fp.GetCallsign(), nss);

			/*if (m_Config->GetDebugMode() && nss.isValid) {
				std::stringstream ss;
				ss << fp.GetCallsign() << " " << nss.nextSectorId << " " << nss.copn << " " << nss.copAltitude << " " << nss.clbDesc;
				LogDebugMessage(ss.str(), "LoA");
			}*/
		}
	}
	std::stringstream ss;
	ss << "Calculated next sectors for " << calculatedNextSectors.size() << " flightplans";
	LogDebugMessage(ss.str(), "LoA");

}

NextSectorStructure RGBremenPlugIn::CalculateNextSector(const EuroScopePlugIn::CFlightPlan& fp, const EuroScopePlugIn::CRadarTarget& rt)
{
	if (!fp.IsValid() || !rt.IsValid()) return NextSectorStructure{};

	const char* sectorId = ControllerMyself().GetPositionId();
	//if(m_Config->GetDebugMode())
	//	sectorId = "ALR"; // FOR DEBUGING

	std::vector<std::string> childSectors {};

	if (sectorOwnership.contains(sectorId)) {
		childSectors = sectorOwnership.at(sectorId);
	}

	if (childSectors.size() > 0) {
		NextSectorStructure nss;
		for (std::string si : childSectors) {
			nss = CalculateNextSectorById(si.c_str(), fp, rt);
			if (nss.isValid) {
				return nss;
			}
		}
	}

	return CalculateNextSectorById(sectorId, fp, rt);
}

const NextSectorStructure RGBremenPlugIn::CalculateNextSectorById(const char* sectorId, const EuroScopePlugIn::CFlightPlan& fp, const EuroScopePlugIn::CRadarTarget& rt)
{
	nlohmann::json sectorDefinition;
	try
	{
		sectorDefinition = this->m_LoaDefinition->GetSectorDefinition(sectorId);
	}
	catch (const std::exception& e)
	{
		LogDebugMessage(e.what(), "Next Sector");
		return NextSectorStructure{};
	}

	if (sectorDefinition.size() < 1) {
		return NextSectorStructure{};
	}

	std::string icaoDep, icaoArr = "";
	icaoDep = fp.GetFlightPlanData().GetOrigin();
	icaoArr = fp.GetFlightPlanData().GetDestination();

	// One sector item is defined like this
	// Structure is the same for Departures and Arrivals
	/*{
	"icaos": ["ETNH", "ETNS"] ,
	"cops" : ["HAM"] ,
	"level" : 11000,
	"vs" : "",
	"directs" : ["HN", "SWG"] ,
	"toSector" : "EIDE"
	},*/

	const char* filedRoute = fp.GetFlightPlanData().GetRoute();
	const char* directTo = fp.GetControllerAssignedData().GetDirectToPointName();
	int altitude = rt.GetPosition().GetPressureAltitude();
	int vs = rt.GetVerticalSpeed() / 200;
	int cfl = fp.GetControllerAssignedData().GetClearedAltitude();

	NextSectorStructure nextSector = NextSectorStructure{};

	try {
		// If a direct is set check for it first
		if (directTo != NULL && directTo != "") {
			for (auto& sector : sectorDefinition.at("Arrivals")) {
				for (const auto& i : sector.at("icaos")) {
					if (i == icaoArr) {
						for (const auto& d : sector.at("directs")) {
							if (d == directTo) {
								nextSector.clbDesc = (sector.at("vs") == "clb") ? 1 : (sector.at("vs") == "desc") ? -1 : 0;
								nextSector.nextSectorId = sector.at("toSector");
								nextSector.copAltitude = sector.at("level");
								nextSector.copn = d;
								return nextSector;
							}
						}
					}
				}
			}
			for (auto& sector : sectorDefinition.at("Departures")) {
				for (const auto& i : sector.at("icaos")) {
					if (i == icaoDep) {
						for (const auto& d : sector.at("directs")) {
							if (d == directTo) {
								nextSector.clbDesc = (sector.at("vs") == "clb") ? 1 : (sector.at("vs") == "desc") ? -1 : 0;
								nextSector.nextSectorId = sector.at("toSector");
								nextSector.copAltitude = sector.at("level");
								nextSector.copn = d;
								return nextSector;
							}
						}
					}
				}
			}
		}

		// If it is not a direct
		// We have to go through the route and find the COP
		std::regex regex("[\\s]+");
		std::vector<std::string> routeParts = Tokenize(filedRoute, regex);
		for (auto& sector : sectorDefinition.at("Arrivals")) {
			for (const auto& i : sector.at("icaos")) {
				if (i == icaoArr) {
					for (const std::string& c : sector.at("cops")) {
						for (const auto& r : routeParts) {
							if (r == c) {
								nextSector.clbDesc = (sector.at("vs") == "clb") ? 1 : (sector.at("vs") == "desc") ? -1 : 0;
								nextSector.nextSectorId = sector.at("toSector");
								nextSector.copAltitude = sector.at("level");
								nextSector.copn = c;
								return nextSector;
							}
						}
					}
				}
			}
		}
		for (auto& sector : sectorDefinition.at("Departures")) {
			for (const auto& i : sector.at("icaos")) {
				if (i == icaoDep) {
					for (const std::string& c : sector.at("cops")) {
						for (const auto& r : routeParts) {
							if (r == c) {
								nextSector.clbDesc = (sector.at("vs") == "clb") ? 1 : (sector.at("vs") == "desc") ? -1 : 0;
								nextSector.nextSectorId = sector.at("toSector");
								nextSector.copAltitude = sector.at("level");
								nextSector.copn = c;
								return nextSector;
							}
						}
					}
				}
			}
		}
	}
	catch (std::exception& e) {
		LogDebugMessage(e.what(), "LoA");
	}
	return nextSector;
}
