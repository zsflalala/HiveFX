#pragma once

#include <EGL/egl.h>
#include <memory>

namespace hiveVG
{
    class CScreenQuad;
    class CSingleTexturePlayer;
    class CLightningSequencePlayer;

    class CLightningRenderer
    {
    public:
        CLightningRenderer();
        ~CLightningRenderer();

        void render();

    private:
        void __initAlgorithm();

        double       m_LastFrameTime = 0.0f;
        double       m_CurrentTime   = 0.0f;

        CScreenQuad*          m_pScreenQuad = nullptr;
        //CSingleTexturePlayer* m_pStaticCloud = nullptr;
        CLightningSequencePlayer* m_pLightningPlayer = nullptr;
    };
}