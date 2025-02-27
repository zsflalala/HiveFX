#pragma once

#include <EGL/egl.h>
#include "Common.h"

struct android_app;

namespace hiveVG
{
    class CSingleTexturePlayer;
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
        static double __getCurrentTime();

        std::string  m_TexturePath;
        int          m_WindowWidth          = -1;
        int          m_WindowHeight         = -1;
        android_app* m_pApp                 = nullptr;
        EGLDisplay   m_Display              = EGL_NO_DISPLAY;
        EGLSurface   m_Surface              = EGL_NO_SURFACE;
        EGLContext   m_Context              = EGL_NO_CONTEXT;
        CSingleTexturePlayer* m_pTestPlayer = nullptr;
        CScreenQuad*          m_pScreenQuad = nullptr;
    };
}