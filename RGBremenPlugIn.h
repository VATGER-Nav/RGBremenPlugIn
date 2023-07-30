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
#include "LoA/LoaDefinition.h"
#include "LoA/NextSectorStructure.h"

class RGBremenPlugIn : public EuroScopePlugIn::CPlugIn
{
private:
	Config* m_Config;
	SidStarHandler* m_SidStarHandler;
	TagHandler* m_TagHandler;
	LetterOfAgreement::LoaDefinition* m_LoaDefinition;

	std::chrono::minutes weatherUpdateInterval;
	std::string weatherUpdateURL;
	bool useReportedGS;
	bool useTrueNorthHeading;
	std::string prefixIAS;
	std::string prefixMach;
	int machDigits;
	int machThresholdFL;
	std::string unreliableIASIndicator;
	COLORREF* unreliableIASColor;
	std::string unreliableMachIndicator;
	COLORREF* unreliableMachColor;
	bool broadcastUnreliableSpeed;

	std::unordered_map<std::string, int> reportedIAS;
	std::unordered_set<std::string> calculatedIASToggled;
	std::unordered_set<std::string> calculatedIASAbbreviatedToggled;
	std::unordered_map<std::string, double> reportedMach;
	std::unordered_set<std::string> calculatedMachToggled;
	std::unordered_set<std::string> calculatedMachAboveThresholdToggled;
	std::unordered_set<std::string> unreliableSpeedToggled;

	Weather::Weather weather;
	Threading::PeriodicAction* weatherUpdateHandler;

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
	void SetReportedIAS(const EuroScopePlugIn::CFlightPlan& fp, std::string selected);
	void ClearReportedIAS(const EuroScopePlugIn::CFlightPlan& fp);
	void ToggleCalculatedIAS(const EuroScopePlugIn::CFlightPlan& fp, bool abbreviated = false);
	double CalculateIAS(const EuroScopePlugIn::CRadarTarget& rt);
	void ShowCalculatedIAS(const EuroScopePlugIn::CRadarTarget& rt, char tagItemContent[16], int* tagItemColorCode, COLORREF* tagItemRGB, bool abbreviated = false, bool onlyToggled = false);

	void SetReportedMach(const EuroScopePlugIn::CFlightPlan& fp, std::string selected);
	void ClearReportedMach(const EuroScopePlugIn::CFlightPlan& fp);
	void ToggleCalculatedMach(const EuroScopePlugIn::CFlightPlan& fp, bool aboveThreshold = false);
	double CalculateMach(const EuroScopePlugIn::CRadarTarget& rt);
	void ShowCalculatedMach(const EuroScopePlugIn::CRadarTarget& rt, char tagItemContent[16], int* tagItemColorCode, COLORREF* tagItemRGB, bool aboveThreshold = false, bool onlyToggled = false);

	void ToggleUnreliableSpeed(const EuroScopePlugIn::CFlightPlan& fp);

	void BroadcastScratchPad(const EuroScopePlugIn::CFlightPlan& fp, std::string msg);
	void CheckScratchPadBroadcast(const EuroScopePlugIn::CFlightPlan& fp);

	void SetFlightStripAnnotation(const EuroScopePlugIn::CFlightPlan& fp, std::string msg, int index = BROADCAST_FLIGHT_STRIP_INDEX);
	void CheckFlightStripAnnotations(const EuroScopePlugIn::CFlightPlan& fp);
	void CheckFlightStripAnnotationsForAllAircraft();

	void UpdateLoginState();
	void CheckLoginState();
	void UpdateWeather();
	void StartWeatherUpdater();
	void StopWeatherUpdater();
	void ResetWeatherUpdater();

	void UpdateNextSectorPredictionForAllAircraft();
	NextSectorStructure CalculateNextSector(const EuroScopePlugIn::CFlightPlan& fp, const EuroScopePlugIn::CRadarTarget& rt);
public:
	RGBremenPlugIn();
	virtual ~RGBremenPlugIn();
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