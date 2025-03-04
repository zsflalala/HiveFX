#pragma once

#include <EGL/egl.h>
#include "Common.h"

struct android_app;

namespace hiveVG
{
    class CCombinedBigSnowFrameRenderer;
    class CCombinedSmallSnowFrameRenderer;
    class CCombinedBigRainFrameRenderer;
    class CCombinedSmallRainFrameRenderer;
    class CCombinedSnowCoverFrameRenderer;
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


        int                                 m_WindowWidth   = -1;
        int                                 m_WindowHeight  = -1;
        bool                                m_IsPointerDown = false;
        android_app*                        m_pApp          = nullptr;
        EGLDisplay                          m_Display       = EGL_NO_DISPLAY;
        EGLSurface                          m_Surface       = EGL_NO_SURFACE;
        EGLContext                          m_Context       = EGL_NO_CONTEXT;
        ERenderType                         m_RenderType        = ERenderType::SNOW;
        ERenderType                         m_EnableRenderType  = ERenderType::SMALL_SNOW_FORE;
        CCombinedBigSnowFrameRenderer*      m_pCombinedBigSnowRender;
        CCombinedSmallSnowFrameRenderer*    m_pCombinedSmallSnowRender;
        CCombinedBigRainFrameRenderer*      m_pCombinedBigRainRender;
        CCombinedSmallRainFrameRenderer*    m_pCombinedSmallRainRender;
        CCombinedSnowCoverFrameRenderer*    m_pCombinedSnowCoverRender;
        CCloudRendererBillBoard*            m_pCombinedCloudRender;

    };
}