#pragma once

#include <android/log.h>

#define LOG_DEBUG(...) __android_log_print(ANDROID_LOG_DEBUG, __VA_ARGS__)
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, __VA_ARGS__)
#define LOG_WARN(...) __android_log_print(ANDROID_LOG_WARN, __VA_ARGS__)
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, __VA_ARGS__)

namespace hiveVG::TAG_KEYWORD
{
    const char *const MAIN_TAG = "Main";
    const char *const RENDERER_TAG = "CRenderer";
    const char *const SEQFRAME_RENDERER_TAG = "CSequenceFrameRenderer";
    const char *const TEXTURE2D_TAG = "CTexture2D";
    const char *const SHADER_PROGRAM_TAG = "CShaderProgram";
    const char *const SINGLE_PALYER_TAG = "CSequenceFramePlayer";
    const char *const SEQFRAME_PALYER_TAG = "CSequenceFramePlayer";
    const char *const ASYNC_SEQFRAME_PALYER_TAG = "CAsyncSequenceFramePlayer";
}

namespace hiveVG
{
    enum class EPictureType : std::uint8_t
    {
        PNG = 0, JPG, WEBP, ASTC, ETC1, ETC2
    };
}