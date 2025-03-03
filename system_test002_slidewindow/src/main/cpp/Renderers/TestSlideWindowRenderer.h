#include <EGL/egl.h>
#include <glm/glm.hpp>
#include "Common.h"

struct android_app;

namespace hiveVG
{
    class CScreenQuad;
    class CSlideWindow;

    class CTestSlideWindowRenderer
    {
    public:
        CTestSlideWindowRenderer(android_app *vApp);
        ~CTestSlideWindowRenderer();

        void renderScene(int vWindowWidth, int vWindowHeight);

    private:
        void   __initAlgorithm();
        static double __getCurrentTime();

        double                     m_LastFrameTime        = 0.0f;
        double                     m_CurrentTime          = 0.0f;
        android_app*               m_pApp                 = nullptr;
        CScreenQuad*               m_pScreenQuad          = nullptr;
        CSlideWindow*              m_pSlideWindow         = nullptr;
    };

}


