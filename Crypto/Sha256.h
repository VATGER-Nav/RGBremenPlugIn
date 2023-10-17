#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <functional>
#include <openssl/sha.h>

class Sha256
{

public:
	static std::string CalculateFileSHA256(const std::string& filePath);
};

