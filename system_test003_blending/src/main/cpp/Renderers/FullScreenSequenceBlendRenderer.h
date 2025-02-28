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
        void   __initAlgorithm();
        void   __initBillBoardManager();
        double __getCurrentTime();
        void   __SequenceFrameDrawCallFunc(CSequenceFramePlayer* vSequFraPlayer, int vWindowWidth, int vWindowHeight, float vDeltaTime);
        void   __BillBoardDrawCallFunc(int vWindowWidth, int vWindowHeight, float vDeltaTime);

        bool         m_showLayer1 = true;
        bool         m_showLayer2 = true;
        bool         m_showLayer3 = true;
        bool         m_showLayer4 = true;
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
