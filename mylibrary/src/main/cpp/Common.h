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
    const char *const TEXTURE_BLENDER_TAG       = "CTextureBlender";
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

    enum class EBlendingMode : std::uint8_t
    {
        NORMAL = 0, MULTIPLY, LIGHTEN, LINEAR_DODGE, LIGHTER_COLOR, OVERLAY, SOFT_LIGHT, COUNT
    };

    const static std::string P60SaveToPhotoPath = "/storage/emulated/0/Pictures/Screenshots/";

    // Shaders config path
    const static std::string SeqTexPlayVert        = "shaders/sequenceTexturePlayer.vert";
    const static std::string SeqTexPlayFrag        = "shaders/sequenceTexturePlayer.frag";
    const static std::string SingleTexPlayVert     = "shaders/singleTexturePlayer.vert";
    const static std::string SingleTexPlayFrag     = "shaders/singleTexturePlayer.frag";
    const static std::string SlideWindowVert       = "shaders/slideWindow.vert";
    const static std::string SlideWindowHFrag      = "shaders/slideWindowH.frag";
    const static std::string SlideWindowVFrag      = "shaders/slideWindowV.frag";

    const static std::string BlitTex2ScreenVert    = "shaders/blitTex2Screen.vert";
    const static std::string BlitTex2ScreenFrag    = "shaders/blitTex2Screen.frag";
    const static std::string BlendAlphaFrag        = "shaders/blendAlpha.frag";
    const static std::string BlendMultiplyFrag     = "shaders/blendMultiply.frag";
    const static std::string BlendLightenFrag      = "shaders/blendLighten.frag";
    const static std::string BlendLinearDodgeFrag  = "shaders/blendLinearDodge.frag";
    const static std::string BlendLighterColorFrag = "shaders/blendLighterColor.frag";
    const static std::string BlendOverlayFrag      = "shaders/blendOverlay.frag";
}