#include "SidStarHandler.h"

SidStarHandler::SidStarHandler(Config *config)
{
	m_Config = config;
	m_localFilePath = m_Config->GetWorkingDirectory() + "\\/clbdesc_sidstar.txt";

	FILE* f;
	if (m_Config->GetSidStarForceDownload() || fopen_s(&f, m_localFilePath.c_str(), "r") != 0) {
		DownloadDefinitionFile();
	}
	else {
		m_status = DOWNLOAD_SKIPPED;
		if (f) fclose(f);
	}
}

SidStarHandler::~SidStarHandler()
{
	delete m_Config;
}

void SidStarHandler::DownloadDefinitionFile()
{
	HRESULT hr;
	hr = URLDownloadToFile(NULL, m_Config->GetSidStarDownloadUrl().c_str(), m_localFilePath.c_str(), 0, NULL);
	if (hr == S_OK)
	{
		m_status = DOWNLOAD_OK;
	}
	else {
		m_status = DOWNLOAD_FAILED;
	}
}

void SidStarHandler::ParseDefinitionFile()
{
	std::ifstream definitionFile(m_localFilePath);

	if (!definitionFile.is_open())
	{
		m_status = PARSED_FAILED;
		return;
	}

	std::string line;
	while (std::getline(definitionFile, line)) {
		if (line.substr(0, 1) == ";") continue; // Skip comments
		std::stringstream lineStream(line);
		// Read the file
		std::string icao;
		std::string type;
		std::string point;
		std::string id;
		std::getline(lineStream, icao, ' ');
		std::getline(lineStream, type, ' ');
		std::getline(lineStream, point, ' ');
		std::getline(lineStream, id);

		if (type == "SID") {
			SIDCheckerData s;
			s.airport = icao;
			s.sid = point;
			s.id = id;
			m_sids.push_back(s);
		}
		if (type == "STAR") {
			STARCheckerData s;
			s.airport = icao;
			s.star = point;
			s.id = id;
			m_stars.push_back(s);
		}
	}
	m_status = PARSED_OK;
}

std::string SidStarHandler::GetStatus()
{
	switch (m_status)
	{
	case DOWNLOAD_OK:
		return "Download of definition file succeeded";
		break;
	case DOWNLOAD_SKIPPED:
		return "Download skipped. File exists already. To force download set 'sidstarForceDownload' setting to 'true'";
		break;
	case DOWNLOAD_FAILED:
		return "Download of definition file failed";
		break;
	case PARSED_OK:
		return "Definition file parsed";
		break;
	case PARSED_FAILED:
		return "Definition file parsing failed";
		break;
	default:
		return "Unknown status";
		break;
	}
}

bool SidStarHandler::ParseReady()
{
	return m_status == SidStarHandlerStati::DOWNLOAD_OK || m_status == SidStarHandlerStati::DOWNLOAD_SKIPPED;
}

bool SidStarHandler::AnalyzeSid(EuroScopePlugIn::CFlightPlan fp)
{
	if (!fp.IsValid()) return false;

	if (m_sids.empty()) return false;

	for (auto sid : m_sids) {
		if (fp.GetFlightPlanData().GetSidName() == sid.sid + sid.id) {
			return true;
		}
	}

	return false;
}

bool SidStarHandler::AnalyzeStar(EuroScopePlugIn::CFlightPlan fp)
{
	if (!fp.IsValid()) return false;

	if (m_stars.empty()) return false;

	for (auto star : m_stars) {
		if (fp.GetFlightPlanData().GetStarName() == star.star + star.id) {
			return true;
		}
	}

	return false;
}
