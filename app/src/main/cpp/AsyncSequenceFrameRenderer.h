#pragma once

#include <EGL/egl.h>
#include <vector>
#include <queue>
#include <android/asset_manager.h>

struct android_app;

namespace hiveVG
{
    class CScreenQuad;
    class CSingleTexturePlayer;
    class CSequenceFramePlayer;
    class CAsyncSequenceFramePlayer;
    class CShaderProgram;
    struct STextureData;

    class CAsyncSequenceFrameRenderer
    {
    public:
        CAsyncSequenceFrameRenderer(android_app *vApp);
        virtual ~CAsyncSequenceFrameRenderer();

        void renderScene();

    private:
        void   __initRenderer();
        void   __initAlgorithm();
        void   __updateRenderArea();

        int                         m_WindowWidth          = -1;
        int                         m_WindowHeight         = -1;
        android_app*                m_pApp                 = nullptr;
        EGLDisplay                  m_Display              = EGL_NO_DISPLAY;
        EGLSurface                  m_Surface              = EGL_NO_SURFACE;
        EGLContext                  m_Context              = EGL_NO_CONTEXT;
        CScreenQuad*                m_pScreenQuad          = nullptr;
        CAsyncSequenceFramePlayer*  m_pAsyncSeqFramePlayer = nullptr;
        CSingleTexturePlayer*       m_pSingleFramePlayer   = nullptr;
    };
}