#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <windows.h>
#include <urlmon.h>
#include <regex>
#include <list>
#include "SID.h"
#include "STAR.h"
#include <EuroScopePlugIn.h>
#include "../Configuration/Config.h"

class SidStarHandler
{
private:
	Config *m_Config;
	std::string m_localFilePath;
	std::list<SIDCheckerData> m_sids;
	std::list<STARCheckerData> m_stars;
	enum SidStarHandlerStati
	{
		DOWNLOAD_OK,
		DOWNLOAD_SKIPPED,
		DOWNLOAD_FAILED,
		PARSED_OK,
		PARSED_FAILED
	};
	int m_status = -1;
public:
	SidStarHandler(Config *config);
	virtual ~SidStarHandler();
	void DownloadDefinitionFile();
	void ParseDefinitionFile();
	std::string GetStatus();
	bool ParseReady();
	bool AnalyzeSid(EuroScopePlugIn::CFlightPlan fp);
	bool AnalyzeStar(EuroScopePlugIn::CFlightPlan fp);
};

