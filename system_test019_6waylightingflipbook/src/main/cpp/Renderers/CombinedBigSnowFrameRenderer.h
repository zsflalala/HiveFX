#pragma once

#include <EGL/egl.h>
#include "Common.h"

struct android_app;

namespace hiveVG
{
    class CScreenQuad;
    class CSingleTexturePlayer;
    class CSequenceFramePlayer;

    class CCombinedBigSnowFrameRenderer
    {
    public:
        explicit CCombinedBigSnowFrameRenderer(android_app *vApp);
        ~CCombinedBigSnowFrameRenderer();

        void renderScene(int vWindowWidth, int vWindowHeight);

    private:
        void __initAlgorithm();

        double                m_LastFrameTime           = 0.0f;
        double                m_CurrentTime             = 0.0f;
        android_app*          m_pApp                    = nullptr;
        CScreenQuad*          m_pScreenQuad             = nullptr;
        CSequenceFramePlayer* m_pCombineForeFramePlayer = nullptr;
        CSequenceFramePlayer* m_pCombineBackFramePlayer = nullptr;
        CSingleTexturePlayer* m_pBackFramePlayer        = nullptr;
        EPictureType::EPictureType m_PictureType = EPictureType::PNG;
    };
}