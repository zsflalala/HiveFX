#pragma once

#include <EGL/egl.h>
#include <glm/glm.hpp>
#include "Common.h"

struct android_app;

namespace hiveVG
{
    class CScreenQuad;
    class CSingleTexturePlayer;
    class CSequenceFramePlayer;

    class CTestSequencePlayerRenderer
    {
    public:
        CTestSequencePlayerRenderer(android_app *vApp);
        ~CTestSequencePlayerRenderer();

        void renderScene(int vWindowWidth, int vWindowHeight);

    private:
        void   __initAlgorithm();

        double                     m_LastFrameTime        = 0.0f;
        double                     m_CurrentTime          = 0.0f;
        glm::vec2                  m_UVOffset             = glm::vec2(0.0f, 0.0f);
        EPlayType::EPlayType       m_PlayMode             = EPlayType::FULLSCREEN;
        EPictureType::EPictureType m_PictureType          = EPictureType::PNG;
        float                      m_PlayScale            = 1.0f;
        android_app*               m_pApp                 = nullptr;
        CScreenQuad*               m_pScreenQuad          = nullptr;
        CSequenceFramePlayer*      m_pSmallSnowForePlayer = nullptr;
    };
}