#include "Config.h"

void Config::loadConfigurationFromFile()
{
	// Then we can read the configuration file
	std::ifstream f(m_workingDirectory + "/RG Bremen.json");
	m_configData = json::parse(f);
}

void Config::saveConfigurationToFile()
{
	std::ofstream o(m_workingDirectory + "/RG Bremen.json");
	o << std::setw(4) << m_configData << std::endl;
}

Config::Config()
{
	// Get the working directory
	char path[MAX_PATH];
	HMODULE hm = NULL;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)"RG Bremen PlugIn", &hm);
	GetModuleFileName(hm, path, sizeof(path));
	std::string dir(path);
	m_workingDirectory = dir.substr(0, dir.find_last_of("\\/"));

	loadConfigurationFromFile();
}

Config::~Config()
{
}

void Config::ReloadConfigurationFromFile()
{
	m_configData.clear();
	loadConfigurationFromFile();
}

void Config::SaveConfiguration()
{
	this->saveConfigurationToFile();
}

bool Config::GetDebugMode()
{
	return m_configData["debug"];
}

std::string Config::GetWorkingDirectory()
{
	return m_workingDirectory;
}

bool Config::GetSidStarForceDownload()
{
	return m_configData["data"]["sidstarForceDownload"];
}

std::string Config::GetSidStarDownloadUrl()
{
	return m_configData["data"]["sidstarurl"];
}

bool Config::GetNotamForceDownload()
{
	return m_configData["data"]["grpluginForceNotamUpdate"];
}

std::string Config::GetGRPNotamDownloadUrl()
{
	return m_configData["data"]["grpluginNotamUrl"];
}

bool Config::GetCustomColorsUsageFlag()
{
	return m_configData["colors"]["useCustomColors"];
}

void Config::ToggleCustomColorsUsageFlag()
{
	m_configData["colors"]["useCustomColors"] = !this->GetCustomColorsUsageFlag();
}

std::array<int, 4> Config::GetListColorSidStarYes()
{
	return m_configData["colors"]["lists"]["sidstar"]["yes"];
}

std::array<int, 4> Config::GetListColorSidStarNo()
{
	return m_configData["colors"]["lists"]["sidstar"]["no"];
}

std::array<int, 4> Config::GetTagColorVFR()
{
	return m_configData["colors"]["tag"]["vfr"];
}

std::array<int, 4> Config::GetTagColorTWR()
{
	return m_configData["colors"]["tag"]["twr"];
}

nlohmann::json Config::GetAirspeedConfiguration()
{
	try
	{
		return m_configData.at("airspeeds");
	}
	catch (const std::exception&)
	{
		return nlohmann::json::parse(R"(
			{
				"mach":{
					"prefix": "M",
					"digits":2,
					"thresholdFL":245,
					"unreliableIndicator": "--",
					"unreliableColor": "123,123,123"
				},
				"ias":{
					"prefix": "N",
					"unreliableIndicator": "--",
					"unreliableColor": "123,123,123"
				},
				"weather":{
					"url":"https://wx.vatsim-germany.org/api/regions/EDXX/wx",
					"update":30
				}
			}
		)");
	}
	
}

nlohmann::json Config::GetFlightPlanConfiguration()
{
	try
	{
		return m_configData.at("flightplan");
	}
	catch (const std::exception&)
	{
		return nlohmann::json::parse(R"(
			{
				"settings": {
					"assignNAP": false,
					"autoProcess" :  false,
					"warnRFLBelowCFL": false,
					"logMinMaxRFL": false,
					"checkMinMaxRFL" :  false
				},
				"airports": {},
				"routing": {}
			})");
	}
}
