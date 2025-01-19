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
    class CAsyncSequenceFramePlayer;
    struct STextureData;

    class CRainFrameRendererAsync
    {
    public:
        CRainFrameRendererAsync(android_app *vApp);
        ~CRainFrameRendererAsync();

        void renderScene();

    private:
        void   __initAlgorithm();

        android_app*                m_pApp                 = nullptr;
        CScreenQuad*                m_pScreenQuad          = nullptr;
        CAsyncSequenceFramePlayer*  m_pAsyncSeqFramePlayer = nullptr;
        CSingleTexturePlayer*       m_pSingleFramePlayer   = nullptr;
    };
}