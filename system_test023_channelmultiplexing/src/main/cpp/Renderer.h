#pragma once

#include <EGL/egl.h>
#include "Common.h"

struct android_app;

namespace hiveVG
{
    class CRainMultiChannelSeqRenderer;

    class CRenderer
    {
    public:
        CRenderer(android_app *vApp);
        ~CRenderer();

        void renderScene();
        void handleInput();

    private:
        void __initRenderer();
        void __updateRenderArea();

        template<typename T>
        void __deleteSafely(T*& vPointer);

        bool           m_IsPointerDown = false;
        ERenderChannel m_RenderChannel = ERenderChannel::R;
        int            m_WindowWidth   = -1;
        int            m_WindowHeight  = -1;
        android_app*   m_pApp          = nullptr;
        EGLDisplay     m_Display       = EGL_NO_DISPLAY;
        EGLSurface     m_Surface       = EGL_NO_SURFACE;
        EGLContext     m_Context       = EGL_NO_CONTEXT;
        CRainMultiChannelSeqRenderer* m_pRainMultiChannelSeqRenderer = nullptr;
    };

    template<typename T>
    void CRenderer::__deleteSafely(T*& vPointer)
    {
        if (vPointer != nullptr)
        {
            delete vPointer;
            vPointer = nullptr;
        }
    }
}