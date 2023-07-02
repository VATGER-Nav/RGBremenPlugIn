#pragma once

#include "Defines.h"
#include <EuroScopePlugIn.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <math.h>
#include <iomanip>
#include <list>
#include "SID_CLB_DATA.h"
#include "STAR_DESC_DATA.h"

#define CURL_STATICLIB
#include <curl/curl.h>
#include <regex>

using namespace std;
using namespace EuroScopePlugIn;

class CRGBremenPlugin : public EuroScopePlugIn::CPlugIn
{
private:
	bool isVFR(int squawk);
	std::string url_sidstar_file_location;
	COLORREF colorVfr;
	COLORREF colorHandoff;
	COLORREF colorClimbViaSidYes;
	COLORREF colorClimbViaSidNo;
	COLORREF colorTwr;
	/*COLORREF colorConcerned;
	COLORREF colorUnconcerned;
	COLORREF colorAssumed;*/
	list<SID_CLB_DATA> climbViaSids;
	list<STAR_DESC_DATA> descendViaStar;
	void loadConfiguration();
	void readSidStarRestrictionsList();
	std::string getCurrentDirectory();
	bool analzyeSid(CFlightPlan fp);
	bool analzyeStarTrans(CFlightPlan fp);
	CArray<CFlightPlan, CFlightPlan &> m_NordoData;
	CFlightPlanList m_NordoList;
	vector<string> Tokenize(const string str, const std::regex regex);
public:
	CRGBremenPlugin();
	virtual ~CRGBremenPlugin();
	virtual void OnGetTagItem(EuroScopePlugIn::CFlightPlan FlightPlan, EuroScopePlugIn::CRadarTarget RadarTarget, int ItemCode, int TagData, char sItemString[16], int* pColorCode, COLORREF* pRGB, double* pFontSize);
	virtual bool OnCompileCommand(const char* sCommandLine);
	void WriteNordoReport(std::string& nordoFilePath);
	virtual void OnFunctionCall(int FunctionId, const char* sItemString, POINT Pt, RECT Area);
protected:
	const char* GetCopxPointName(EuroScopePlugIn::CFlightPlan FlightPlan);
	string padWithZeros(int padding, int s);
	int getNordoFlightPlanIndex(const char* sCallsign);
	string time_to_string();
};

