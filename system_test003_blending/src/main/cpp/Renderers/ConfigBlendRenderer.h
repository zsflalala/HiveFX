#pragma once

#include <EGL/egl.h>
#include <memory>

struct android_app;

namespace hiveVG
{
    class CBlendManager;

    class CConfigBlendRenderer
    {
    public:
        CConfigBlendRenderer(android_app *vApp);
        ~CConfigBlendRenderer();

        void render();
        void switchRenderStatus(int vIndex);
        void setLayerBlendMode(int vIndex, int vBlendMode);

    private:
        void __initAlgorithm();

        android_app* m_pApp = nullptr;
        CBlendManager* m_pManager = nullptr;
    };

}