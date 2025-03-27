#pragma once
struct android_app;

namespace hiveVG
{
    class CScreenQuad;
    class CSingleTexturePlayer;
    class CSequenceFramePlayer;

    class CRendererWithExternalStorage
    {
    public:
        CRendererWithExternalStorage(android_app *vApp);
        ~CRendererWithExternalStorage();

        void renderScene(int vWindowWidth, int vWindowHeight);

    private:
        void   __initAlgorithm();

        double                     m_LastFrameTime        = 0.0f;
        double                     m_CurrentTime          = 0.0f;
        android_app*               m_pApp                 = nullptr;
        CScreenQuad*               m_pScreenQuad          = nullptr;
        CSingleTexturePlayer*      m_pBackground          = nullptr;
        CSequenceFramePlayer*      m_pSmallRain           = nullptr;
    };
}