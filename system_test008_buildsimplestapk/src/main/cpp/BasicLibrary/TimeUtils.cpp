#include "TimeUtils.h"
#include <ctime>
#include "platform.h"
#ifdef HIVE_UNIT_TEST
#include <chrono>
#endif

using namespace hiveVG;

double CTimeUtils::getCurrentTime()
{
#ifdef HIVE_ANDROID

    struct timeval tv{};
    gettimeofday(&tv, nullptr);
    return static_cast<double>(tv.tv_sec) + static_cast<double>(tv.tv_usec) / 1'000'000.0;

#elif defined(HIVE_UNIT_TEST)

    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();

    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    auto micros = std::chrono::duration_cast<std::chrono::microseconds>(duration - seconds);

    return static_cast<double>(seconds.count()) + static_cast<double>(micros.count()) / 1'000'000.0;
#endif
}