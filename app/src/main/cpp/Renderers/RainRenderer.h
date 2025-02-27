#pragma once

#include <EGL/egl.h>
#include "Common.h"

struct android_app;

namespace hiveVG
{
    class  CScreenQuad;
    class  CSingleTexturePlayer;
    class  CSequenceFramePlayer;
    struct STextureData;

    class CRainRenderer
    {
    public:
        CRainRenderer(android_app *vApp);
        ~CRainRenderer();

        void renderScene(int vWindowWidth, int vWindowHeight);
        void handleInput(ERenderType vInputType, bool vIsPointerDown);

    private:
        void __initAlgorithm();
        double __getCurrentTime();

        double                m_LastFrameTime        = 0.0f;
        double                m_CurrentTime          = 0.0f;
        bool                  m_PreviousPointerState = false;
        bool                  m_EnableBigRainFore    = false;
        bool                  m_EnableBigRainBack    = false;
        bool                  m_EnableSmallRainFore  = false;
        bool                  m_EnableSmallRainBack  = false;
        android_app*          m_pApp                 = nullptr;
        CScreenQuad*          m_pScreenQuad          = nullptr;
        CSequenceFramePlayer* m_pSmallRainForePlayer = nullptr;
        CSequenceFramePlayer* m_pSmallRainBackPlayer = nullptr;
        CSequenceFramePlayer* m_pBigRainForePlayer   = nullptr;
        CSequenceFramePlayer* m_pBigRainBackPlayer   = nullptr;
        CSingleTexturePlayer* m_pBackFramePlayer     = nullptr;
    };
}

