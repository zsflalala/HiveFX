#pragma once

#include <EGL/egl.h>

struct android_app;

namespace hiveVG
{
    class  CScreenQuad;
    class  CSingleTexturePlayer;
    class  CAsyncSequenceFramePlayer;
    struct STextureData;

    class CRainRendererAsync
    {
    public:
        CRainRendererAsync(android_app *vApp);
        ~CRainRendererAsync();

        void renderScene();

    private:
        void   __initAlgorithm();

        android_app*                m_pApp                 = nullptr;
        CScreenQuad*                m_pScreenQuad          = nullptr;
        CAsyncSequenceFramePlayer*  m_pAsyncSeqFramePlayer = nullptr;
        CSingleTexturePlayer*       m_pSingleFramePlayer   = nullptr;
    };
}