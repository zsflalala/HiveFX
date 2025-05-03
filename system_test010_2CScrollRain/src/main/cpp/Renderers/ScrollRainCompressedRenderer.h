#pragma once

#include <EGL/egl.h>
#include <memory>

struct android_app;

namespace hiveVG
{
    class CScreenQuad;
    class CSlideWindow;

    class CScrollRainCompressedRenderer
    {
    public:
        explicit CScrollRainCompressedRenderer(android_app *vApp);
        ~CScrollRainCompressedRenderer();

        void renderScene();

    private:
        void __initAlgorithm();

        double         m_LastFrameTime  = 0.0f;
        double         m_CurrentTime    = 0.0f;
        int            m_WindowWidth    = 0;
        int            m_WindowHeight   = 0;
        android_app*   m_pApp           = nullptr;
        CScreenQuad*   m_pScreenQuad    = nullptr;
        CSlideWindow*  m_pSlideWindow   = nullptr;
    };
}