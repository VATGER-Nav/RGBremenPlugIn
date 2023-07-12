#pragma once
#include <string>
#include <wtypes.h>

struct TagItemContainer
{
	bool handled = true;
	std::string sItemString;
	bool customColor = false;
	COLORREF color;
};