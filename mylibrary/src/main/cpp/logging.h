#pragma once
#include "platform.h"

#ifdef HIVE_ANDROID
    #include <android/log.h>

    #define LOG_DEBUG(...) __android_log_print(ANDROID_LOG_DEBUG, __VA_ARGS__)
    #define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, __VA_ARGS__)
    #define LOG_WARN(...) __android_log_print(ANDROID_LOG_WARN, __VA_ARGS__)
    #define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, __VA_ARGS__)

#elif defined(HIVE_UNIT_TEST)
    #include <iostream>
    #include <format>

    #define LOG_DEBUG(tag, fmt, ...) std::cout << std::format("[DEBUG][{}] {}", tag, std::format(fmt, ##__VA_ARGS__)) << std::endl
    #define LOG_INFO(tag, fmt, ...)  std::cout << std::format("[INFO][{}] {}",  tag, std::format(fmt, ##__VA_ARGS__)) << std::endl
    #define LOG_WARN(tag, fmt, ...)  std::cout << std::format("[WARN][{}] {}",  tag, std::format(fmt, ##__VA_ARGS__)) << std::endl
    #define LOG_ERROR(tag, fmt, ...) std::cout << std::format("[ERROR][{}] {}", tag, std::format(fmt, ##__VA_ARGS__)) << std::endl
#endif