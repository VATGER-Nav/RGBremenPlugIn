#include "Sha256.h"
#include "../lib/Helpers.h"

std::string Sha256::CalculateFileSHA256(const std::string& filePath)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return "";
    }

    std::vector<unsigned char> fileData(
        (std::istreambuf_iterator<char>(file)),
        (std::istreambuf_iterator<char>())
    );

    std::function<std::string(const std::vector<unsigned char>&)> sha256 = [](const std::vector<unsigned char>& data) {
        unsigned char hash[SHA256_DIGEST_LENGTH] = { 0 };
        SHA256_CTX sha256Context;
        #pragma warning(suppress : 4996)
        SHA256_Init(&sha256Context);
#pragma warning(suppress : 4996)
        SHA256_Update(&sha256Context, data.data(), data.size());
#pragma warning(suppress : 4996)
        SHA256_Final(hash, &sha256Context);

        std::stringstream ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
        }

        return ss.str();
    };

    return sha256(fileData);
}
