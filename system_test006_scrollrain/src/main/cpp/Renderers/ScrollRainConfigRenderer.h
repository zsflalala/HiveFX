#pragma once

#include <EGL/egl.h>
#include <memory>

struct android_app;

namespace hiveVG
{
    class CBlendManager;

    class CScrollRainConfigRenderer
    {
    public:
        CScrollRainConfigRenderer(android_app *vApp);
        ~CScrollRainConfigRenderer();

        void render();

    private:
        void __initAlgorithm();

        android_app*   m_pApp     = nullptr;
        CBlendManager* m_pManager = nullptr;
    };
}