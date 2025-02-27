#pragma once

#include <EGL/egl.h>

struct android_app;

namespace hiveVG
{
    class  CScreenQuad;
    class  CSequenceFramePlayer;
    struct STextureData;

    class CSnowSceneRenderer
    {
    public:
        CSnowSceneRenderer(android_app *vApp);
        ~CSnowSceneRenderer();

        void renderScene(int vWindowWidth, int vWindowHeight);

    private:
        void   __initAlgorithm();
        double __getCurrentTime();

        double                m_LastFrameTime            = 0.0f;
        double                m_CurrentTime              = 0.0f;
        android_app*          m_pApp                     = nullptr;
        CScreenQuad*          m_pScreenQuad              = nullptr;
        CSequenceFramePlayer* m_pSnowSceneSeqFramePlayer = nullptr;
    };
}
