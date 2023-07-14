#pragma once

#include <string>
#include <windows.h>
#include <wininet.h>

#include "../Configuration/Defines.h"
#include "../Helpers.h"

namespace Http
{
	std::string get(const char* const url);
	std::string get(const std::string& url);

	std::tuple<std::string, INTERNET_PORT, DWORD, std::string> parseURL(std::string url);
	[[noreturn]] void throwLastError(const std::string& functionName);
}
