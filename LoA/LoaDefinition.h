#pragma once

#include <windows.h>
#include <libloaderapi.h>
#include <string>
#include <fstream>
#include "nlohmann/json.hpp"

namespace LetterOfAgreement
{
	class LoaDefinition
	{
	public:
		LoaDefinition();
		~LoaDefinition();
		nlohmann::json GetSectorDefinition(std::string sectorId);
	private:
		std::string m_workingDirectory;
		nlohmann::json m_loaDefinition;
	};
};