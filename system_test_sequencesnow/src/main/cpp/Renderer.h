#pragma once

#include <EGL/egl.h>
#include "Common.h"

struct android_app;

namespace hiveVG
{
    class CSnowQuantizationSeqRenderer;
    class CSnowQuantizationBicPicRenderer;

    class CRenderer
    {
    public:
        CRenderer(android_app *vApp);
        ~CRenderer();

        void renderScene();

    private:
        void __initRenderer();
        void __updateRenderArea();

        template<typename T>
        void __deleteSafely(T*& vPointer);

        int           m_WindowWidth   = -1;
        int           m_WindowHeight  = -1;
        android_app*  m_pApp          = nullptr;
        EGLDisplay    m_Display       = EGL_NO_DISPLAY;
        EGLSurface    m_Surface       = EGL_NO_SURFACE;
        EGLContext    m_Context       = EGL_NO_CONTEXT;
        CSnowQuantizationBicPicRenderer*   m_pSnowQuantizationBigPicRenderer = nullptr;
        CSnowQuantizationSeqRenderer*      m_pSnowQuantizationSeqRenderer    = nullptr;
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