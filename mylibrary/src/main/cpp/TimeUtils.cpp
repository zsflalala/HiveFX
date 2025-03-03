#include "TimeUtils.h"
#include <ctime>

using namespace hiveVG;

double CTimeUtils::getCurrentTime()
{
    struct timeval tv{};
    gettimeofday(&tv, nullptr);
    return static_cast<double>(tv.tv_sec) + static_cast<double>(tv.tv_usec) / 1'000'000.0;
}