#pragma once

#include <EGL/egl.h>

struct android_app;

namespace hiveVG
{
    class  CScreenQuad;
    class  CAsyncSequenceFramePlayer;
    struct STextureData;

    class CSnowSceneRendererAsync
    {
    public:
        CSnowSceneRendererAsync(android_app *vApp);
        ~CSnowSceneRendererAsync();

        void renderScene();

    private:
        void   __initAlgorithm();

        android_app*                m_pApp                 = nullptr;
        CScreenQuad*                m_pScreenQuad          = nullptr;
        CAsyncSequenceFramePlayer*  m_pAsyncSeqFramePlayer = nullptr;
    };
}
