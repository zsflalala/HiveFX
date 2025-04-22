
#include "StringUtils.h"
using namespace hiveVG;

int CStringUtils::checkWeatherCondition(const std::string &vTexPath) {
    std::string lowerStr = vTexPath;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    if (lowerStr.find("snow") != std::string::npos)
        return 1;
    else if (lowerStr.find("rain") != std::string::npos)
        return 0;
    return -1;
}
