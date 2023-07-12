#include "stdafx.h"
#include "RGBremenPlugIn.h"
#include <string>

RGBremenPlugIn::RGBremenPlugIn() : EuroScopePlugIn::CPlugIn(EuroScopePlugIn::COMPATIBILITY_CODE, RG_BREMEN_PLUGIN_NAME, RG_BREMEN_PLUGIN_VERSION, RG_BREMEN_PLUGIN_DEVELOPER, RG_BREMEN_PLUGIN_COPYRIGHT)
{
	std::string pluginName = RG_BREMEN_PLUGIN_NAME;
	std::string version = RG_BREMEN_PLUGIN_VERSION;
	DisplayUserMessage(pluginName.c_str(), "Initialisation", ("Loading Version " + version).c_str(), true, false, false, false, false);

	// Load configuration
	m_Config = new Config();
	DisplayUserMessage(pluginName.c_str(), "Configuration", "Configuration loaded from 'RG Bremen.json'", true, false, false, false, false);
	if (m_Config->GetDebugMode()){
		DisplayUserMessage(pluginName.c_str(), "Configuration", "Debug mode active", true, false, false, false, false);
	}

	// Load SidStarHandler
	m_SidStarHandler = new SidStarHandler(m_Config);
	if (m_SidStarHandler->ParseReady()) {
		m_SidStarHandler->ParseDefinitionFile();
	}
	DisplayUserMessage(pluginName.c_str(), "SidStarHandler", m_SidStarHandler->GetStatus().c_str(), true, false, false, false, false);

	// Register List Items for Sid Star Checker
	RegisterTagItemType("LIST / SID Check", RG_BREMEN_LIST_ITEM_SID_CHECK);
	RegisterTagItemType("LIST / STAR Check", RG_BREMEN_LIST_ITEM_STAR_CHECK);

	// Initialize the TagItemHandler
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

	DisplayUserMessage(pluginName.c_str(), "Initialisation", ("Version " + version + " loaded").c_str(), true, false, false, false, false);
}

RGBremenPlugIn::~RGBremenPlugIn()
{
	// Clear Tag Handler
	delete m_TagHandler;
	// Clear Sid Star Handler
	delete m_SidStarHandler;
	// Clear configuration
	delete m_Config;

}

void RGBremenPlugIn::OnControllerPositionUpdate(CController Controller)
{
}

void RGBremenPlugIn::OnControllerDisconnect(CController Controller)
{
}

void RGBremenPlugIn::OnRadarTargetPositionUpdate(CRadarTarget RadarTarget)
{
}

void RGBremenPlugIn::OnFlightPlanDisconnect(CFlightPlan FlightPlan)
{
}

void RGBremenPlugIn::OnFlightPlanFlightPlanDataUpdate(CFlightPlan FlightPlan)
{
}

void RGBremenPlugIn::OnPlaneInformationUpdate(const char* sCallsign, const char* sLivery, const char* sPlaneType)
{
}

void RGBremenPlugIn::OnFlightPlanControllerAssignedDataUpdate(CFlightPlan FlightPlan, int DataType)
{
}

void RGBremenPlugIn::OnFlightPlanFlightStripPushed(CFlightPlan FlightPlan, const char* sSenderController, const char* sTargetController)
{
}

CRadarScreen* RGBremenPlugIn::OnRadarScreenCreated(const char* sDisplayName, bool NeedRadarContent, bool GeoReferenced, bool CanBeSaved, bool CanBeCreated)
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

void RGBremenPlugIn::OnGetTagItem(CFlightPlan FlightPlan, CRadarTarget RadarTarget, int ItemCode, int TagData, char sItemString[16], int* pColorCode, COLORREF* pRGB, double* pFontSize)
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
	if (m_Config->GetDebugMode() && tic.handled) {
		char* tmp = new char[15];
		_itoa_s(ItemCode, tmp, 15, 10);
		DisplayUserMessage(RG_BREMEN_PLUGIN_NAME, "OnGetTagItem", "Received TagItemCode for", true, false, false, false, false);
		DisplayUserMessage(RG_BREMEN_PLUGIN_NAME, "OnGetTagItem", tmp, true, false, false, false, false);
		DisplayUserMessage(RG_BREMEN_PLUGIN_NAME, "OnGetTagItem", tic.sItemString.c_str(), true, false, false, false, false);
	}

	if (tic.handled) {
		if (tic.customColor && m_Config->GetCustomColorsUsageFlag()) {
			*pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
			*pRGB = tic.color;
		}
		strncpy_s(sItemString, 15, (tic.sItemString + "\0").c_str(), 15);
	}

	// Not our tag item
	return;
}

void RGBremenPlugIn::OnRefreshFpListContent(CFlightPlanList AcList)
{
}

void RGBremenPlugIn::OnNewMetarReceived(const char* sStation, const char* sFullMetar)
{
}

void RGBremenPlugIn::OnFunctionCall(int FunctionId, const char* sItemString, POINT Pt, RECT Area)
{
}

void RGBremenPlugIn::OnTimer(int Counter)
{
	if (Counter % 60 == 0) {
		m_TagHandler->SetTransitionAltitude(GetTransitionAltitude());
	}
}
