#pragma once

#ifdef __ANDROID__
#define HIVE_ANDROID 1
#elif defined(_WIN32)
#define HIVE_UNIT_TEST 1
#endif
