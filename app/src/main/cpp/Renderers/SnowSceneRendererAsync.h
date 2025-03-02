#pragma once

#include <EGL/egl.h>
#include "Common.h"

struct android_app;

namespace hiveVG
{
    class  CScreenQuad;
    class  CSingleTexturePlayer;
    class  CAsyncSequenceFramePlayer;

    class CSnowSceneRendererAsync
    {
    public:
        CSnowSceneRendererAsync(android_app *vApp);
        ~CSnowSceneRendererAsync();

        void renderScene();
        void handleInput(ERenderType vInputType, bool vIsPointerDown);

    private:
        void __initAlgorithm();

        bool                        m_PreviousPointerState = false;
        bool                        m_EnableSnowScene   = false;
        android_app*                m_pApp                 = nullptr;
        CScreenQuad*                m_pScreenQuad          = nullptr;
        CAsyncSequenceFramePlayer*  m_pSnowSceneSeqFramePlayer = nullptr;

    };
}