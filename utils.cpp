#include "utils.h"
#include <algorithm>
#include <cctype>
#include <sstream>

std::vector<std::string> split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;

    while (std::getline(ss, token, delimiter)) {
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }

    return tokens;
}

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";

    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

bool isNumber(const std::string& str) {
    if (str.empty()) return false;

    for (char c : str) {
        if (!std::isdigit(c)) return false;
    }

    return true;
}

bool isValidPrice(const std::string& str) {
    if (str.empty()) return false;

    bool hasDot = false;
    int dotCount = 0;

    for (char c : str) {
        if (c == '.') {
            if (hasDot) return false;
            hasDot = true;
            dotCount++;
        } else if (!std::isdigit(c)) {
            return false;
        }
    }

    return true;
}