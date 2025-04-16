#pragma once

#include <EGL/egl.h>
#include <glm/glm.hpp>
#include "Common.h"

struct android_app;

namespace hiveVG
{
    class CScreenQuad;
    class CSingleTexturePlayer;
    class CAsyncSequenceFramePlayer;

    class CTestInterpolationPlaying
    {
    public:
        CTestInterpolationPlaying(android_app *vApp);
        ~CTestInterpolationPlaying();

        void renderScene(int vWindowWidth, int vWindowHeight);

    private:
        void   __initAlgorithm();

        int                        m_TextureCount         = 64;
        EPictureType::EPictureType m_PictureType          = EPictureType::PNG;
        android_app*               m_pApp                 = nullptr;
        CScreenQuad*               m_pScreenQuad          = nullptr;
        CAsyncSequenceFramePlayer*      m_pTestPlayer          = nullptr;
        CSingleTexturePlayer*       m_pSingleFramePlayer   = nullptr;
    };
}