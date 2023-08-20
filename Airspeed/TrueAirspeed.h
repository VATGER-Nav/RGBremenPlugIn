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

#include "../Configuration/Defines.h"
#include "../Configuration/Config.h"
#include "../lib/Helpers.h"
#include "../Calculation/Calculations.h"
#include "../Http/Http.h"
#include "../Threading/PeriodicAction.h"
#include "../Weather/Weather.h"

class TrueAirspeed
{
public:
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
	std::vector<std::string> messageMap = {};

	std::unordered_map<std::string, int> reportedIAS;
	std::unordered_set<std::string> calculatedIASToggled;
	std::unordered_set<std::string> calculatedIASAbbreviatedToggled;
	std::unordered_map<std::string, double> reportedMach;
	std::unordered_set<std::string> calculatedMachToggled;
	std::unordered_set<std::string> calculatedMachAboveThresholdToggled;
	std::unordered_set<std::string> unreliableSpeedToggled;

	Weather::Weather weather;
	Threading::PeriodicAction* weatherUpdateHandler;

	TrueAirspeed(Config* m_Config);
	~TrueAirspeed();

	void UpdateWeather();
	void StartWeatherUpdater();
	void StopWeatherUpdater();
	void ResetWeatherUpdater();

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
	void CheckFlightStripAnnotationsForAllAircraft(std::vector<EuroScopePlugIn::CFlightPlan> flightPlans);
	
};

