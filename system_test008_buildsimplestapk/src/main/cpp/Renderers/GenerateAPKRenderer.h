#pragma once

#include <EGL/egl.h>
#include "Common.h"

struct android_app;

namespace hiveVG
{
    class CScreenQuad;
    class CSingleTexturePlayer;
    class CSequenceFramePlayer;

    class CGenerateAPKRenderer
    {
    public:
        explicit CGenerateAPKRenderer(android_app *vApp);
        ~CGenerateAPKRenderer();

        void renderScene(int vWindowWidth, int vWindowHeight);

    private:
        void __initAlgorithm();

        double                m_LastFrameTime           = 0.0f;
        double                m_CurrentTime             = 0.0f;
        android_app*          m_pApp                    = nullptr;
        CScreenQuad*          m_pScreenQuad             = nullptr;
        CSingleTexturePlayer* m_pBackFramePlayer        = nullptr;
        EPictureType::EPictureType m_PictureType = EPictureType::PNG;
    };
}