#pragma once

#include <EGL/egl.h>
#include "Common.h"

struct android_app;

namespace hiveVG
{
    class CSequenceFramePlayer;
    class CScreenQuad;

    class CRenderer
    {
    public:
        CRenderer(android_app *vApp);
        ~CRenderer();

        void renderScene();

    private:
        void __initRenderer();
        void __initAlgorithm();
        void __updateRenderArea();
        void __generateSnowScene();

        std::string  m_TexturePath;
        std::string  m_P60GeneratePath      = P60SaveToPhotoPath;
        int          m_WindowWidth          = -1;
        int          m_WindowHeight         = -1;
        double       m_LastFrameTime        = 0.0f;
        double       m_CurrentTime          = 0.0f;
        android_app* m_pApp                 = nullptr;
        EGLDisplay   m_Display              = EGL_NO_DISPLAY;
        EGLSurface   m_Surface              = EGL_NO_SURFACE;
        EGLContext   m_Context              = EGL_NO_CONTEXT;
        CSequenceFramePlayer* m_pTestPlayer = nullptr;
        CScreenQuad*          m_pScreenQuad = nullptr;
    };
}