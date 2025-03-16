#pragma once

#include <EGL/egl.h>
#include <memory>

struct android_app;

namespace hiveVG
{
    class CBlendManager;

    class CRendererWithConfig
    {
    public:
        CRendererWithConfig(android_app *vApp);
        ~CRendererWithConfig();

        bool init(const std::string& vConfigFile);
        void render();
        void switchRenderStatus(int vIndex);
        void setLayerBlendMode(int vBlendMode);

    private:
        android_app* m_pApp = nullptr;
        CBlendManager* m_pManager = nullptr;
    };
}