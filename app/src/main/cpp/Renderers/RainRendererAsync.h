#pragma once

#include <EGL/egl.h>
#include "Common.h"

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
        void handleInput(ERenderType vInputType, bool vIsPointerDown);

    private:
        void __initAlgorithm();

        bool                        m_PreviousPointerState = false;
        bool                        m_EnableBigRainFore    = false;
        bool                        m_EnableBigRainBack    = false;
        bool                        m_EnableSmallRainFore  = false;
        bool                        m_EnableSmallRainBack  = false;
        android_app*                m_pApp                 = nullptr;
        CScreenQuad*                m_pScreenQuad          = nullptr;
        CAsyncSequenceFramePlayer*  m_pSmallRainForePlayer = nullptr;
        CAsyncSequenceFramePlayer*  m_pSmallRainBackPlayer = nullptr;
        CAsyncSequenceFramePlayer*  m_pBigRainForePlayer   = nullptr;
        CAsyncSequenceFramePlayer*  m_pBigRainBackPlayer   = nullptr;
        CSingleTexturePlayer*       m_pSingleFramePlayer   = nullptr;
    };
}