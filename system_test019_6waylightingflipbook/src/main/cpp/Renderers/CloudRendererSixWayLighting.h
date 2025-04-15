#pragma once

#include <EGL/egl.h>
#include <memory>
#include "Common.h"

struct android_app;

namespace hiveVG
{
    class CScreenQuad;
    class CSixWayLightingFlipbookPlayer;
    class CBillBoardManager;
    class CBoard;

    class CCloudRendererSixWayLighting
    {
    public:
        CCloudRendererSixWayLighting(android_app *vApp);
        ~CCloudRendererSixWayLighting();

        void renderScene(int vWindowWidth, int vWindowHeight);

    private:
        void   __initAlgorithm();

        double       m_LastFrameTime = 0.0f;
        double       m_CurrentTime   = 0.0f;
        android_app* m_pApp          = nullptr;

        CBoard*      m_pBoard         = nullptr;
        EPictureType::EPictureType m_PictureType = EPictureType::PNG;
        std::unique_ptr<CBillBoardManager> m_pCloudManager = nullptr;
    };
}