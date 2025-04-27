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
    class CSceneRenderer
    {
    public:
        CSceneRenderer(android_app *vApp);
        ~CSceneRenderer();

        void render(int vWindowWidth, int vWindowHeight);
        const std::string& getOutputFileName() {return m_OutputFile;}

    private:
        void   __initAlgorithm();

        double                     m_LastFrameTime        = 0.0f;
        double                     m_CurrentTime          = 0.0f;
        std::string                m_OutputFile           = "";

        android_app*               m_pApp                 = nullptr;
        CScreenQuad*               m_pScreenQuad          = nullptr;
        CSequenceFramePlayer*      m_pSequencePlayer      = nullptr;
        CSingleTexturePlayer*      m_pSingleTexture       = nullptr;
    };
}