#pragma once

#include <EGL/egl.h>
#include <memory>

struct android_app;

namespace hiveVG
{
    class CScreenQuad;
    class CSingleTexturePlayer;
    class CLightningSequencePlayer;

    class CLightningRenderer
    {
    public:
        CLightningRenderer(android_app *vApp);
        ~CLightningRenderer();

        void render(int vWindowWidth, int vWindowHeight);

    private:
        void __initAlgorithm();

        android_app* m_pApp = nullptr;
        double       m_LastFrameTime = 0.0f;
        double       m_CurrentTime   = 0.0f;

        CScreenQuad*          m_pScreenQuad = nullptr;
        //CSingleTexturePlayer* m_pStaticCloud = nullptr;
        CLightningSequencePlayer* m_pLightningPlayer = nullptr;
    };
}
