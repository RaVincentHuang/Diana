#include "sysy/Support/StrUtils.h"
#include "sysy/Support/Debug.h"

int strHex(std::string str)
{
    std::stringstream ss;
    ss << str;
    int ans;
    ss >> std::hex >> ans;
    return ans;
}

int strOct(std::string str)
{
    std::stringstream ss;
    ss << str;
    int ans;
    ss >> std::oct >> ans;
    return ans;
}

int strDec(std::string str)
{
    std::stringstream ss;
    ss << str;
    int ans;
    ss >> ans;
    return ans;
}

float strFloat(std::string str)
{
    float ans = std::atof(str.c_str());
    return ans;
}

float strFloatHex(std::string str)
{
    float ans = std::atof(str.c_str());
    return ans;
}

std::string addIndent(const std::string& str)
{ 
    return std::regex_replace(str, std::regex("\n"), "\n        ");
}

std::string subLine(std::string str)
{
    return "\n" + std::string("        ") + addIndent(str);
}
