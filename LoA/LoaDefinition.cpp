#include "LoaDefinition.h"

LetterOfAgreement::LoaDefinition::LoaDefinition()
{
	// Get the working directory
	char path[MAX_PATH];
	HMODULE hm = NULL;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)"RG Bremen PlugIn", &hm);
	GetModuleFileName(hm, path, sizeof(path));
	std::string dir(path);
	m_workingDirectory = dir.substr(0, dir.find_last_of("\\/"));

	// Then we can read the configuration file
	std::ifstream f(m_workingDirectory + "/RG Bremen.json");
	m_loaDefinition = nlohmann::json::parse(f);
}

LetterOfAgreement::LoaDefinition::~LoaDefinition()
{
}

nlohmann::json LetterOfAgreement::LoaDefinition::GetSectorDefinition(std::string sectorId)
{
	try
	{
		auto& sectorDefinition = m_loaDefinition.at(sectorId);
		return sectorDefinition;
	}
	catch (const std::exception&)
	{
		return nullptr;
	}
}
