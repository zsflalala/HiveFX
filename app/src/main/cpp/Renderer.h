#pragma once

#include <EGL/egl.h>
#include "Common.h"

struct android_app;

namespace hiveVG
{
    class CTestSequencePlayerRenderer;
    class CRainRendererAsync;
    class CSnowRendererAsync;
    class CSnowSceneRendererAsync;
    class CCloudRendererBillBoard;
    class CSnowSceneRenderer;
    class CRainRenderer;

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

        ERenderType  m_RenderType        = ERenderType::SNOW;
        ERenderType  m_EnableRenderType  = ERenderType::SMALL_SNOW_FORE;
        bool         m_IsPointerDown = false;
        int          m_WindowWidth   = -1;
        int          m_WindowHeight  = -1;
        android_app* m_pApp          = nullptr;
        EGLDisplay   m_Display       = EGL_NO_DISPLAY;
        EGLSurface   m_Surface       = EGL_NO_SURFACE;
        EGLContext   m_Context       = EGL_NO_CONTEXT;
        CTestSequencePlayerRenderer* m_pSnow       = nullptr;
        CRainRenderer*               m_pRain       = nullptr;
        CRainRendererAsync*          m_pRainAsync  = nullptr;
        CSnowRendererAsync*          m_pSnowAsync  = nullptr;
        CCloudRendererBillBoard*     m_pCloudScene = nullptr;
        CSnowSceneRenderer*          m_pSnowScene  = nullptr;
        CSnowSceneRendererAsync*     m_pSnowSceneASync  = nullptr;
    };
}