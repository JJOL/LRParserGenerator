#ifndef _STRING_UTILS_H
#define _STRING_UTILS_H

#include <string>
#include <regex>
#include <vector>

std::vector<std::string> split(std::string s, std::string delimiter);
std::vector<std::string> split(std::string s, std::regex r);

#endif