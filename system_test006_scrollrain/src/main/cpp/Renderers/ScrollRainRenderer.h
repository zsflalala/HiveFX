#include <EGL/egl.h>
#include <glm/glm.hpp>
#include "Common.h"

struct android_app;

namespace hiveVG
{
    class CScreenQuad;
    class CSlideWindow;
    class CSingleTexturePlayer;
    class CTextureBlender;

    typedef void (DrawCallFunc)();
    class CScrollRainRenderer
    {
    public:
        CScrollRainRenderer(android_app *vApp);
        ~CScrollRainRenderer();

        void renderScene(int vWindowWidth, int vWindowHeight);

    private:
        void   __initAlgorithm();

        double                     m_LastFrameTime        = 0.0f;
        double                     m_CurrentTime          = 0.0f;
        android_app*               m_pApp                 = nullptr;
        CScreenQuad*               m_pScreenQuad          = nullptr;
        CSlideWindow*              m_pSlideWindow         = nullptr;
        CSingleTexturePlayer*      m_pBackground          = nullptr;
        CTextureBlender*           m_pTexBlender          = nullptr;
    };

}
