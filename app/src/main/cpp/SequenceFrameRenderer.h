#pragma once

#include <EGL/egl.h>

struct android_app;

namespace hiveVG
{
    class CScreenQuad;
    class CSingleTexturePlayer;
    class CSequenceFramePlayer;

    class CSequenceFrameRenderer
    {
    public:
        CSequenceFrameRenderer(android_app *vApp);
        virtual ~CSequenceFrameRenderer();

        void renderBlendingSnow(const int vRow, const int vColumn);

    private:
        void            __initRenderer();
        void            __initAlgorithm();

        android_app*                    m_pApp               = nullptr;
        EGLDisplay                      m_Display            = EGL_NO_DISPLAY;
        EGLSurface                      m_Surface            = EGL_NO_SURFACE;
        EGLContext                      m_Context            = EGL_NO_CONTEXT;
        CScreenQuad*                    m_pScreenQuad        = nullptr;
        CSingleTexturePlayer*           m_pBackGroundTexture = nullptr;
        CSingleTexturePlayer*           m_pCartoonTexture    = nullptr;
        CSequenceFramePlayer*           m_pSnowSceneSeq      = nullptr;
        CSequenceFramePlayer*           m_pNearSnowSeq       = nullptr;
    };

}

