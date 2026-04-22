#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

std::vector<std::string> split(const std::string& str, char delimiter);
std::string trim(const std::string& str);
bool isNumber(const std::string& str);
bool isValidPrice(const std::string& str);

#endif