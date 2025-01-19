#pragma once

#include <EGL/egl.h>

struct android_app;

namespace hiveVG
{
    class CScreenQuad;
    class CSingleTexturePlayer;
    class CSequenceFramePlayer;

    class CSnowFrameRenderer
    {
    public:
        CSnowFrameRenderer(android_app *vApp);
        ~CSnowFrameRenderer();

        void renderScene(int vWindowWidth, int vWindowHeight);

    private:
        void   __initAlgorithm();
        double __getCurrentTime();

        double       m_LastFrameTime = 0.0f;
        double       m_CurrentTime   = 0.0f;
        android_app* m_pApp          = nullptr;
        CScreenQuad* m_pScreenQuad   = nullptr;
        CSequenceFramePlayer*  m_pNearSnowPlayer     = nullptr;
        CSequenceFramePlayer*  m_pFarSnowPlayer      = nullptr;
        CSingleTexturePlayer*  m_pBackgroundPlayer   = nullptr;
    };
}