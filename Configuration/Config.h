#pragma once

#include <windows.h>
#include <libloaderapi.h>
#include <string>
#include <fstream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class Config
{
private:
	std::string m_workingDirectory;
	json m_configData;
	void loadConfigurationFromFile();
public:
	Config();
	virtual ~Config();
	// Reload configuration from File
	void ReloadConfigurationFromFile();
	// Get debug mode setting
	bool GetDebugMode();
	// Working Directory path
	std::string GetWorkingDirectory();
	// SID STAR VIA Checker Download URL
	bool GetSidStarForceDownload();
	std::string GetSidStarDownloadUrl();
	// List Colors
	std::array<int, 4> GetListColorSidStarYes();
	std::array<int, 4> GetListColorSidStarNo();
	// TAG Colors
	std::array<int, 4> GetTagColorVFR();
	std::array<int, 4> GetTagColorTWR();
};

