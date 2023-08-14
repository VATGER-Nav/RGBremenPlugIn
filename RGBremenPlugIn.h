#pragma once

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <shared_mutex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <wtypes.h>
#include <windows.h>

#include <EuroScopePlugIn.h>

#include "Configuration/Defines.h"
#include "Configuration/Config.h"
#include "lib/Helpers.h"
#include "Calculation/Calculations.h"
#include "Http/Http.h"
#include "Threading/PeriodicAction.h"
#include "SidStar/SidStarHandler.h"
#include "Tag/TagHandler.h"
#include "Weather/Weather.h"
#include "Airspeed/TrueAirspeed.h"
#include "LoA/LoaDefinition.h"
#include "LoA/NextSectorStructure.h"

class RGBremenPlugIn : public EuroScopePlugIn::CPlugIn
{
private:
	Config* m_Config;
	SidStarHandler* m_SidStarHandler;
	TagHandler* m_TagHandler;
	LetterOfAgreement::LoaDefinition* m_LoaDefinition;

	TrueAirspeed* m_AirspeedHandler;

	std::unordered_map<std::string, NextSectorStructure> calculatedNextSectors;
	Threading::PeriodicAction* nextSectorUpdateHandler;
	std::chrono::minutes nextSectorUpdateInterval;

	int loginState;

	void InitializeTagItemHandler(std::string& pluginName);
	void InitializeSidStarHandler(std::string& pluginName);
	void LoadConfiguration(std::string& pluginName);
	void InitializeAirspeedHandler();
	void LogMessage(std::string msg, std::string channel);
	void LogDebugMessage(std::string msg, std::string channel);

	void UpdateLoginState();
	void CheckLoginState();

	void UpdateNextSectorPredictionForAllAircraft();
	NextSectorStructure CalculateNextSector(const EuroScopePlugIn::CFlightPlan& fp, const EuroScopePlugIn::CRadarTarget& rt);
	const NextSectorStructure CalculateNextSectorById(const char* sectorId, const EuroScopePlugIn::CFlightPlan& fp, const EuroScopePlugIn::CRadarTarget& rt);
public:
	RGBremenPlugIn();
	virtual ~RGBremenPlugIn();
	virtual void	OnAirportRunwayActivityChanged();
	virtual void    OnControllerPositionUpdate(EuroScopePlugIn::CController Controller);
	virtual void    OnControllerDisconnect(EuroScopePlugIn::CController Controller);
	virtual void    OnRadarTargetPositionUpdate(EuroScopePlugIn::CRadarTarget RadarTarget);
	virtual void    OnFlightPlanDisconnect(EuroScopePlugIn::CFlightPlan FlightPlan);
	virtual void    OnFlightPlanFlightPlanDataUpdate(EuroScopePlugIn::CFlightPlan FlightPlan);
	virtual void    OnPlaneInformationUpdate(const char* sCallsign, const char* sLivery, const char* sPlaneType);
	virtual void    OnFlightPlanControllerAssignedDataUpdate(EuroScopePlugIn::CFlightPlan FlightPlan, int DataType);
	virtual void    OnFlightPlanFlightStripPushed(EuroScopePlugIn::CFlightPlan FlightPlan, const char* sSenderController, const char* sTargetController);
	virtual EuroScopePlugIn::CRadarScreen* OnRadarScreenCreated(const char* sDisplayName, bool NeedRadarContent, bool GeoReferenced, bool CanBeSaved, bool CanBeCreated);
	virtual bool    OnCompileCommand(const char* sCommandLine);
	virtual void    OnCompileFrequencyChat(const char* sSenderCallsign, double Frequency, const char* sChatMessage);
	virtual void    OnCompilePrivateChat(const char* sSenderCallsign, const char* sReceiverCallsign, const char* sChatMessage);
    virtual void    OnGetTagItem(EuroScopePlugIn::CFlightPlan FlightPlan, EuroScopePlugIn::CRadarTarget RadarTarget, int ItemCode, int TagData, char sItemString[16], int* pColorCode, COLORREF* pRGB, double* pFontSize);
	virtual void    OnRefreshFpListContent(EuroScopePlugIn::CFlightPlanList AcList);
	virtual void    OnNewMetarReceived(const char* sStation, const char* sFullMetar);
	virtual void    OnFunctionCall(int FunctionId, const char* sItemString, POINT Pt, RECT Area);
	virtual void	OnTimer(int Counter);
};