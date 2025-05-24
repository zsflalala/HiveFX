#pragma once

#include <EGL/egl.h>
#include <memory>

namespace hiveVG
{
    class CScreenQuad;
    class CSequenceFramePlayer;

    class CCloudInterpolationRenderer
    {
    public:
        CCloudInterpolationRenderer();
        ~CCloudInterpolationRenderer();

        void render();

    private:
        void __initAlgorithm();

        double       m_LastFrameTime = 0.0f;
        double       m_CurrentTime   = 0.0f;

        CScreenQuad*          m_pScreenQuad = nullptr;
        CSequenceFramePlayer* m_pCloudPlayer = nullptr;
    };
}