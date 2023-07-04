#pragma once

#include "Configuration/Defines.h"
#include "Configuration/Config.h"
#include "lib/Helpers.h"
#include <EuroScopePlugIn.h>
#include <string>
#include "SidStar/SidStarHandler.h"


using namespace std;
using namespace EuroScopePlugIn;


class RGBremenPlugIn : public EuroScopePlugIn::CPlugIn
{
private:
	Config *m_Config;
	SidStarHandler* m_SidStarHandler;
public:
	RGBremenPlugIn();
	virtual ~RGBremenPlugIn();
	virtual void    OnControllerPositionUpdate(CController Controller);
	virtual void    OnControllerDisconnect(CController Controller);
	virtual void    OnRadarTargetPositionUpdate(CRadarTarget RadarTarget);
	virtual void    OnFlightPlanDisconnect(CFlightPlan FlightPlan);
	virtual void    OnFlightPlanFlightPlanDataUpdate(CFlightPlan FlightPlan);
	virtual void    OnPlaneInformationUpdate(const char* sCallsign, const char* sLivery, const char* sPlaneType);
	virtual void    OnFlightPlanControllerAssignedDataUpdate(CFlightPlan FlightPlan, int DataType);
	virtual void    OnFlightPlanFlightStripPushed(CFlightPlan FlightPlan, const char* sSenderController, const char* sTargetController);
	virtual CRadarScreen* OnRadarScreenCreated(const char* sDisplayName, bool NeedRadarContent, bool GeoReferenced, bool CanBeSaved, bool CanBeCreated);
	virtual bool    OnCompileCommand(const char* sCommandLine);
	virtual void    OnCompileFrequencyChat(const char* sSenderCallsign, double Frequency, const char* sChatMessage);
	virtual void    OnCompilePrivateChat(const char* sSenderCallsign, const char* sReceiverCallsign, const char* sChatMessage);
    virtual void    OnGetTagItem(CFlightPlan FlightPlan, CRadarTarget RadarTarget, int ItemCode, int TagData, char sItemString[16], int* pColorCode, COLORREF* pRGB, double* pFontSize);
	virtual void    OnRefreshFpListContent(CFlightPlanList AcList);
	virtual void    OnNewMetarReceived(const char* sStation, const char* sFullMetar);
	virtual void    OnFunctionCall(int FunctionId, const char* sItemString, POINT Pt, RECT Area);
};