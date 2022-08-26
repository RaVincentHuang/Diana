#ifndef SYSY_STR_UTILS_H
#define SYSY_STR_UTILS_H

#include <iomanip>
#include <sstream>
#include <string>
#include <regex>

int strHex(std::string str);

int strOct(std::string str);

int strDec(std::string str);

float strFloat(std::string str);

float strFloatHex(std::string str);

std::string addIndent(const std::string& str);

std::string subLine(std::string str);

#endif