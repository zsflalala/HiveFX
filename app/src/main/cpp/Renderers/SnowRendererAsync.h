#pragma once

#include <EGL/egl.h>
#include "Common.h"

struct android_app;

namespace hiveVG
{
    class  CScreenQuad;
    class  CSingleTexturePlayer;
    class  CAsyncSequenceFramePlayer;

    class CSnowRendererAsync
    {
    public:
        CSnowRendererAsync(android_app *vApp);
        ~CSnowRendererAsync();

        void renderScene();
        void handleInput(ERenderType vInputType, bool vIsPointerDown);

    private:
        void __initAlgorithm();

        bool                        m_PreviousPointerState = false;
        bool                        m_EnableBigSnowFore    = false;
        bool                        m_EnableBigSnowBack    = false;
        bool                        m_EnableSmallSnowFore  = false;
        bool                        m_EnableSmallSnowBack  = false;
        android_app*                m_pApp                 = nullptr;
        CScreenQuad*                m_pScreenQuad          = nullptr;
        CAsyncSequenceFramePlayer*  m_pSmallSnowForePlayer = nullptr;
        CAsyncSequenceFramePlayer*  m_pSmallSnowBackPlayer = nullptr;
        CAsyncSequenceFramePlayer*  m_pBigSnowForePlayer   = nullptr;
        CAsyncSequenceFramePlayer*  m_pBigSnowBackPlayer   = nullptr;
        CSingleTexturePlayer*       m_pSingleFramePlayer   = nullptr;
    };
}