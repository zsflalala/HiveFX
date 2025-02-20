#pragma once

#include <EGL/egl.h>
#include "Common.h"

struct android_app;

namespace hiveVG
{
    class CScreenQuad;
    class CSingleTexturePlayer;
    class CSequenceFramePlayer;

    class CSnowRenderer
    {
    public:
        CSnowRenderer(android_app *vApp);
        ~CSnowRenderer();

        void renderScene(int vWindowWidth, int vWindowHeight);
        void handleInput(ERenderType vInputType, bool vIsPointerDown);

    private:
        void   __initAlgorithm();
        double __getCurrentTime();

        bool         m_PreviousPointerState = false;
        bool         m_EnableBigSnowFore    = false;
        bool         m_EnableBigSnowBack    = false;
        bool         m_EnableSmallSnowFore  = false;
        bool         m_EnableSmallSnowBack  = false;
        double       m_LastFrameTime        = 0.0f;
        double       m_CurrentTime          = 0.0f;
        android_app* m_pApp                 = nullptr;
        CScreenQuad* m_pScreenQuad          = nullptr;
        CSequenceFramePlayer* m_pSmallSnowForePlayer     = nullptr;
        CSequenceFramePlayer* m_pSmallSnowBackPlayer     = nullptr;
        CSequenceFramePlayer* m_pBigSnowForePlayer       = nullptr;
        CSequenceFramePlayer* m_pBigSnowBackPlayer       = nullptr;
        CSequenceFramePlayer* m_pSnowSceneSeqFramePlayer = nullptr;
    };
}