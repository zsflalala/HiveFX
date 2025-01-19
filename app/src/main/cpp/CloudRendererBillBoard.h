#pragma once

#include <EGL/egl.h>
#include <memory>

struct android_app;

namespace hiveVG
{
    class CScreenQuad;
    class CSequenceFramePlayer;
    class CBillBoardManager;

    class CCloudRendererBillBoard
    {
    public:
        CCloudRendererBillBoard(android_app *vApp);
        ~CCloudRendererBillBoard();

        void renderScene(int vWindowWidth, int vWindowHeight);

    private:
        void   __initAlgorithm();
        double __getCurrentTime();

        double       m_LastFrameTime = 0.0f;
        double       m_CurrentTime   = 0.0f;
        android_app* m_pApp          = nullptr;
        CScreenQuad* m_pScreenQuad   = nullptr;
        std::unique_ptr<CBillBoardManager>  m_pCloudManager = nullptr;
    };
}


