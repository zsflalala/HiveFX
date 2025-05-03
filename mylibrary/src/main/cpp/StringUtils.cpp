#include "StringUtils.h"

using namespace hiveVG;

int CStringUtils::checkWeatherCondition(const std::string &vTexPath)
{
    std::string LowerStr = vTexPath;
    std::transform(LowerStr.begin(), LowerStr.end(), LowerStr.begin(), ::tolower);
    if (LowerStr.find("snow") != std::string::npos)
        return 1;
    else if (LowerStr.find("rain") != std::string::npos)
        return 0;
    return -1;
}