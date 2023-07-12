#pragma once
#include "../Configuration/Defines.h"
#include "../Configuration/Config.h"
#include "../lib/Helpers.h"
#include <wtypes.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <EuroScopePlugIn.h>
#include "../Squawk/SquawkHandler.h"
#include "../SidStar/SidStarHandler.h"
#include "TagItemContainer.h"

class TagHandler
{
private:
	Config *m_Configuration;
	SidStarHandler* m_SidStarHandler;
	int m_TransitionAltitude;
public:
	TagHandler(Config* config, SidStarHandler* sidstarHandler, int transitionAltitude);
	virtual ~TagHandler();
	void SetTransitionAltitude(int transitionAltitude);
	TagItemContainer HandleTagItem(EuroScopePlugIn::CFlightPlan f, EuroScopePlugIn::CRadarTarget r, int tagItemCode);
};

