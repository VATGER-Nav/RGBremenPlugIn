#pragma once

#include "../Configuration/Config.h"
#include "../Helpers.h"
#include "../Configuration/Defines.h"
#include "FlightPlan.h"
#include "Airport.h"
#include "Validation.h"

class DeliveryHelper
{
private:
	Config* m_Config;
	std::map<std::string, Airport> m_Airports;
	std::vector<std::string> m_Processed;

	bool assignNap = false;
	bool autoProcess = false;
	bool warnRFLBelowCFL = false;
	bool logMinMaxRFL = false;
	bool checkMinMaxRFL = false;

	std::vector<std::string> m_MessageMap;
public:
	DeliveryHelper(Config* config);
	~DeliveryHelper();

	std::string GetNextMessage();
	bool HasMessage();

	void ProcessAirportConfig();
	void ProcessRoutingConfig();
	void UpdateActiveAirports(EuroScopePlugIn::CSectorElement& sfe);
	void FlightPlanClosed(EuroScopePlugIn::CFlightPlan& fp);

	Validation ProcessFlightPlan(EuroScopePlugIn::CFlightPlan& fp, bool nap, bool validateOnly = false);
	bool CheckFlightPlanProcessed(EuroScopePlugIn::CFlightPlan& fp);
	bool IsFlightPlanProcessed(EuroScopePlugIn::CFlightPlan& fp);
	
};

