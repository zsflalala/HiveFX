#pragma once

#include <EGL/egl.h>

struct android_app;

namespace hiveVG
{
    class CSnowRenderer;
    class CRainRendererAsync;
    class CCloudRendererBillBoard;
    class CSnowSceneRendererAsync;

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
        CSnowRenderer*           m_pSnow           = nullptr;
        CRainRendererAsync*      m_pRainScene      = nullptr;
        CCloudRendererBillBoard* m_pCloudScene     = nullptr;
        CSnowSceneRendererAsync* m_pSnowScene      = nullptr;
    };

}
