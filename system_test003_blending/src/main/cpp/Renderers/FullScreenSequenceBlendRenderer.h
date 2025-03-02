#pragma once

#include <EGL/egl.h>
#include <memory>

struct android_app;

namespace hiveVG
{
    class CScreenQuad;
    class CTextureBlender;
    class CTexture2D;
    class CSingleTexturePlayer;
    class CSequenceFramePlayer;
    class CBillBoardManager;

    class CFullScreenSequenceBlendRenderer
    {
    public:
        CFullScreenSequenceBlendRenderer(android_app *vApp);
        ~CFullScreenSequenceBlendRenderer();

        void render(int vWindowWidth, int vWindowHeight);
        void changeLayerStatus(int vIndex);
        void changeBlendMode(int vMode);

    private:
        void __initAlgorithm();
        void __initBillBoardManager();
        void __SequenceFrameDrawCallFunc(CSequenceFramePlayer* vSequFraPlayer, int vWindowWidth, int vWindowHeight, double vDeltaTime);
        void __BillBoardDrawCallFunc(int vWindowWidth, int vWindowHeight, double vDeltaTime);

        bool         m_ShowLayer1    = true;
        bool         m_ShowLayer2    = true;
        bool         m_ShowLayer3    = true;
        bool         m_ShowLayer4    = true;
        double       m_LastFrameTime = 0.0f;
        double       m_CurrentTime   = 0.0f;
        android_app* m_pApp          = nullptr;

        CScreenQuad*          m_pScreenQuad = nullptr;
        CTextureBlender*      m_pTexBlender = nullptr;
        CTexture2D*           m_pBackground = nullptr;
        CSingleTexturePlayer* m_pMediumShot = nullptr;
        CSequenceFramePlayer* m_pForeSequFraPlayer = nullptr;
        CSequenceFramePlayer* m_pBackSequFraPlayer = nullptr;
        std::unique_ptr<CBillBoardManager>  m_pCloudManager = nullptr;
    };
}
