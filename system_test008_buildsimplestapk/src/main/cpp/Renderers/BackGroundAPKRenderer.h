#pragma once

#include <EGL/egl.h>
#include "Common.h"

namespace hiveVG
{
    class CScreenQuad;
    class CSingleTexturePlayer;

    class CBackgroundAPKRenderer
    {
    public:
        explicit CBackgroundAPKRenderer();
        ~CBackgroundAPKRenderer();

        void renderScene();

    private:
        void __initAlgorithm();

        double                m_LastFrameTime           = 0.0f;
        double                m_CurrentTime             = 0.0f;
        CScreenQuad*          m_pScreenQuad             = nullptr;
        CSingleTexturePlayer* m_pBackgroundPlayer        = nullptr;
        EPictureType::EPictureType m_PictureType        = EPictureType::PNG;
    };
}
