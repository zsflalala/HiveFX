#pragma once

#include <EGL/egl.h>
#include "Common.h"

struct android_app;

namespace hiveVG
{

    class CFullScreenSequenceBlendRenderer;
    class CRenderer
    {
    public:
        CRenderer(android_app *vApp);
        ~CRenderer();

        void render();
        void handleInput();

    private:
        void __initRenderer();
        void __updateRenderArea();
        void __blendInit();
        void __blendTest();

        bool         m_IsPointerDown = false;
        int          m_WindowWidth   = -1;
        int          m_WindowHeight  = -1;
        android_app* m_pApp          = nullptr;
        EGLDisplay   m_Display       = EGL_NO_DISPLAY;
        EGLSurface   m_Surface       = EGL_NO_SURFACE;
        EGLContext   m_Context       = EGL_NO_CONTEXT;

        CFullScreenSequenceBlendRenderer* m_pFullScreenSequenceBlendRenderer = nullptr;
    };

} // hiveVG

