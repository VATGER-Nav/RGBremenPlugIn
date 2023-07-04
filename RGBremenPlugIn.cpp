#include "stdafx.h"
#include "RGBremenPlugIn.h"
#include <string>

RGBremenPlugIn::RGBremenPlugIn() : EuroScopePlugIn::CPlugIn(EuroScopePlugIn::COMPATIBILITY_CODE, PLUGIN_NAME, PLUGIN_VERSION, PLUGIN_DEVELOPER, PLUGIN_COPYRIGHT)
{
	std::string pluginName = PLUGIN_NAME;
	std::string version = PLUGIN_VERSION;
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
	RegisterTagItemType("LIST / SID Check", LIST_ITEM_SID_CHECK);
	RegisterTagItemType("LIST / STAR Check", LIST_ITEM_STAR_CHECK);

	DisplayUserMessage(pluginName.c_str(), "Initialisation", ("Version " + version + " loaded").c_str(), true, false, false, false, false);
}

RGBremenPlugIn::~RGBremenPlugIn()
{

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

	if (m_Config->GetDebugMode()) {
		for (uint32_t i = 0; i < sCommandLineSplit.size(); i++) {
			DisplayUserMessage(PLUGIN_NAME, PLUGIN_NAME, sCommandLineSplit.at(i).c_str(), true, false, false, false, false);
		}
	}

	if (sCommandLineSplit.at(0) == ".rgbremen" && sCommandLineSplit.size() >= 1) {
		if (sCommandLineSplit.at(1) == "config" && sCommandLineSplit.at(2) == "reload") {
			m_Config->ReloadConfigurationFromFile();
			DisplayUserMessage(PLUGIN_NAME, "Configuration", "Configuration reloaded from 'RG Bremen.json'", true, false, false, false, false);
			return true;
		}
		if (sCommandLineSplit.at(1) == "sidstar") {
			if (sCommandLineSplit.at(2) == "download") {
				m_SidStarHandler->DownloadDefinitionFile();
				DisplayUserMessage(PLUGIN_NAME, "SidStarHandler", m_SidStarHandler->GetStatus().c_str(), true, false, false, false, false);
				return true;
			}
			if (sCommandLineSplit.at(2) == "parse") {
				if (m_SidStarHandler->ParseReady())
					m_SidStarHandler->ParseDefinitionFile();
				DisplayUserMessage(PLUGIN_NAME, "SidStarHandler", m_SidStarHandler->GetStatus().c_str(), true, false, false, false, false);
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
	if (ItemCode == LIST_ITEM_SID_CHECK) {
		if (m_SidStarHandler->AnalyzeSid(FlightPlan))
		{
			*pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
			*pRGB = RGB(m_Config->GetListColorSidStarYes()[0], m_Config->GetListColorSidStarYes()[1], m_Config->GetListColorSidStarYes()[2]);
			strcpy_s(sItemString, 16, "YES");
		}
		else {
			*pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
			*pRGB = RGB(m_Config->GetListColorSidStarNo()[0], m_Config->GetListColorSidStarNo()[1], m_Config->GetListColorSidStarNo()[2]);
			strcpy_s(sItemString, 16, "NO");
		}
	}
	if (ItemCode == LIST_ITEM_STAR_CHECK) {
		if (m_SidStarHandler->AnalyzeStar(FlightPlan))
		{
			*pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
			*pRGB = RGB(m_Config->GetListColorSidStarYes()[0], m_Config->GetListColorSidStarYes()[1], m_Config->GetListColorSidStarYes()[2]);
			strcpy_s(sItemString, 16, "YES");
		}
		else {
			*pColorCode = EuroScopePlugIn::TAG_COLOR_RGB_DEFINED;
			*pRGB = RGB(m_Config->GetListColorSidStarNo()[0], m_Config->GetListColorSidStarNo()[1], m_Config->GetListColorSidStarNo()[2]);
			strcpy_s(sItemString, 16, "NO");
		}
	}
	
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
