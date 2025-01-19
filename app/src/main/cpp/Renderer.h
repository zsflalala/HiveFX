#pragma once

#include <EGL/egl.h>
#include <memory>

struct android_app;

namespace hiveVG
{
    class CSnowFrameRenderer;
    class CRainFrameRendererAsync;
    class CCloudRendererBillBoard;

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

        int          m_SceneID       = 0;
        int          m_WindowWidth   = -1;
        int          m_WindowHeight  = -1;
        android_app* m_pApp          = nullptr;
        EGLDisplay   m_Display       = EGL_NO_DISPLAY;
        EGLSurface   m_Surface       = EGL_NO_SURFACE;
        EGLContext   m_Context       = EGL_NO_CONTEXT;
        CSnowFrameRenderer*      m_pSnowScene      = nullptr;
        CRainFrameRendererAsync* m_pRainScene      = nullptr;
        CCloudRendererBillBoard* m_pCloudScene     = nullptr;
    };

}
