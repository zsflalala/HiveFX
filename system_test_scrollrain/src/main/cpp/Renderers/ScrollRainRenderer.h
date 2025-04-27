#pragma once

#include <EGL/egl.h>
#include <memory>

struct android_app;

namespace hiveVG
{
    class CScreenQuad;
    class CSlideWindow;
    class CSingleTexturePlayer;
    class CSequenceFramePlayer;
    class CSplashManager;

    class CScrollRainRenderer
    {
    public:
        explicit CScrollRainRenderer(android_app *vApp);
        ~CScrollRainRenderer();

        void renderScene();

    private:
        void __initAlgorithm();

        template<typename T>
        void __deleteSafely(T*& vPointer);

        double         m_LastFrameTime  = 0.0f;
        double         m_CurrentTime    = 0.0f;
        int            m_WindowWidth    = 0;
        int            m_WindowHeight   = 0;
        android_app*   m_pApp           = nullptr;
        CScreenQuad*   m_pScreenQuad    = nullptr;
        CSlideWindow*  m_pSlideWindow   = nullptr;
        CSingleTexturePlayer* m_pBackFramePlayer = nullptr;
        CSequenceFramePlayer* m_pSplashPlayer    = nullptr;
        std::unique_ptr<CSplashManager> m_pSplashManager = nullptr;
    };

    template<typename T>
    void CScrollRainRenderer::__deleteSafely(T*& vPointer)
    {
        if (vPointer != nullptr)
        {
            delete vPointer;
            vPointer = nullptr;
        }
    }
}