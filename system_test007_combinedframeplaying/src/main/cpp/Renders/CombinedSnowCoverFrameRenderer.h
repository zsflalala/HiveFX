//
// Created by asus on 2025/3/3.
//
#include <EGL/egl.h>
#include "Common.h"

struct android_app;
namespace hiveVG
{
    class  CScreenQuad;
    class  CSingleTexturePlayer;
    class  CSequenceFramePlayer;
    class CCombinedSnowCoverFrameRenderer
    {
    public:
        CCombinedSnowCoverFrameRenderer(android_app *vApp);
        ~CCombinedSnowCoverFrameRenderer();

        void renderScene(int vWindowWidth, int vWindowHeight);

    private:
        void __initAlgorithm();

        double                m_LastFrameTime        = 0.0f;
        double                m_CurrentTime          = 0.0f;
        android_app*          m_pApp                 = nullptr;
        CScreenQuad*          m_pScreenQuad          = nullptr;
        CSequenceFramePlayer* m_pCombineFramePlayer = nullptr;
        EPictureType::EPictureType  m_PictureType=  EPictureType::PNG;
    };
}
