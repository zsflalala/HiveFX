#pragma once

#include <android/log.h>
#include <cstdint>
#include "EnumReflect.h"

DECLARE_ENUM(EPlayType, FULLSCREEN = 0, PARTIAL)
DECLARE_ENUM(EPictureType, PNG = 0, JPG, WEBP, ASTC, ETC1, ETC2)

#define LOG_DEBUG(...) __android_log_print(ANDROID_LOG_DEBUG, __VA_ARGS__)
#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, __VA_ARGS__)
#define LOG_WARN(...) __android_log_print(ANDROID_LOG_WARN, __VA_ARGS__)
#define LOG_ERROR(...) __android_log_print(ANDROID_LOG_ERROR, __VA_ARGS__)

namespace hiveVG::TAG_KEYWORD
{
    const char *const MAIN_TAG                  = "Main";
    const char *const RENDERER_TAG              = "CRenderer";
    const char *const SEQFRAME_RENDERER_TAG     = "CSequenceFrameRenderer";
    const char *const TEXTURE2D_TAG             = "CTexture2D";
    const char *const SHADER_PROGRAM_TAG        = "CShaderProgram";
    const char *const SINGLE_PALYER_TAG         = "CSequenceFramePlayer";
    const char *const SEQFRAME_PALYER_TAG       = "CSequenceFramePlayer";
    const char *const ASYNC_SEQFRAME_PALYER_TAG = "CAsyncSequenceFramePlayer";
    const char *const TEXTURE_BLENDER_TAG      = "CTextureBlender";
    const char *const SYSTEMTEST_TAG            = "SystemTest";
    const char *const SNOW_STYLIZE_TAG          = "SnowStylize";
}

namespace hiveVG::CONFIG_KEYWORD
{
    const char *const TexturePath               = "texture_path";
}

namespace hiveVG
{
    enum class ERenderType : std::uint8_t
    {
        NONE = 0, SNOW, RAIN, CLOUD, SNOW_SCENE, SMALL_SNOW_FORE, SMALL_SNOW_BACK, BIG_SNOW_FORE, BIG_SNOW_BACK, SMALL_RAIN_FORE, SMALL_RAIN_BACK, BIG_RAIN_FORE, BIG_RAIN_BACK
    };

    enum class EPlayType : std::uint8_t
    {
        FULLSCREEN = 0, PARTICAL
    };

    enum class EBlendingMode : std::uint8_t
    {
        ALPHA_BLEND, MULTIPLY, LIGHTEN, LINEAR_DODGE, LIGHTER_COLOR, OVERLAY, SOFT_LIGHT
    };
    const static std::string P60SaveToPhotoPath = "/storage/emulated/0/Pictures/Screenshots/";
}