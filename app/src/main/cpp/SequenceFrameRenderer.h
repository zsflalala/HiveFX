#pragma once

#include <EGL/egl.h>
#include <memory>

struct android_app;

namespace hiveVG
{
    class CScreenQuad;
    class CSingleTexturePlayer;
    class CSequenceFramePlayer;
    class CSequencePlayerManager;

    class CSequenceFrameRenderer
    {
    public:
        CSequenceFrameRenderer(android_app *vApp);
        virtual ~CSequenceFrameRenderer();

        void renderBlendingSnow();

    private:
        void   __initRenderer();
        void   __initAlgorithm();
        double __getCurrentTime();
        void   __updateRenderArea();

        int          m_WindowWidth   = -1;
        int          m_WindowHeight  = -1;
        double       m_LastFrameTime = 0.0f;
        double       m_CurrentTime   = 0.0f;
        android_app* m_pApp          = nullptr;
        EGLDisplay   m_Display       = EGL_NO_DISPLAY;
        EGLSurface   m_Surface       = EGL_NO_SURFACE;
        EGLContext   m_Context       = EGL_NO_CONTEXT;
        CScreenQuad* m_pScreenQuad   = nullptr;
        std::unique_ptr<CSequencePlayerManager> m_pSequencePlayerManager = nullptr;
        CSequenceFramePlayer* m_pSequenceFramePlayer = nullptr;
    };
}