#pragma once

#include <vector>
#include <string>
#include <regex>

inline std::vector<std::string> Tokenize(const std::string str, const std::regex regex)
{
	std::vector<std::string> result;
	std::sregex_token_iterator it(str.begin(), str.end(), regex, -1);
	std::sregex_token_iterator reg_end;
	for (; it != reg_end; ++it) {
		if (!it->str().empty())
			result.emplace_back(it->str());
	}
	return result;
}