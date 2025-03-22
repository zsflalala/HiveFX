#pragma once

#include <EGL/egl.h>
#include "Common.h"

struct android_app;

namespace hiveVG
{
    class CWeatherAPKRenderer;
    class CBackgroundAPKRenderer;
    class CWeatherSeqRenderer;

    class CRenderer
    {
    public:
        CRenderer(android_app *vApp);
        ~CRenderer();

        void renderScene();

    private:
        void __initRenderer();
        void __updateRenderArea();

        int          m_WindowWidth   = -1;
        int          m_WindowHeight  = -1;
        android_app* m_pApp          = nullptr;
        EGLDisplay   m_Display       = EGL_NO_DISPLAY;
        EGLSurface   m_Surface       = EGL_NO_SURFACE;
        EGLContext   m_Context       = EGL_NO_CONTEXT;
        CWeatherAPKRenderer*    m_pWeatherRenderer    = nullptr;
        CBackgroundAPKRenderer* m_pBackgroundRenderer = nullptr;
        CWeatherSeqRenderer*    m_pWeatherSeqRenderer = nullptr;
    };
}