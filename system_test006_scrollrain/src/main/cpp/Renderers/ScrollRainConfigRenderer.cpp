#include "ScrollRainConfigRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <GLES3/gl3.h>
#include <cassert>
#include "BlendManager.h"

using namespace hiveVG;

CScrollRainConfigRenderer::CScrollRainConfigRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CScrollRainConfigRenderer::~CScrollRainConfigRenderer()
{
    if(m_pManager)
    {
        delete m_pManager;
        m_pManager = nullptr;
    }
}

void CScrollRainConfigRenderer::render()
{
    glClearColor(0.1f,0.2f,0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_pManager->render();
}

void CScrollRainConfigRenderer::__initAlgorithm()
{
    int Width = 0, Height = 0;
    assert(m_pApp->window != nullptr);
    if (m_pApp->window)
    {
        Width = ANativeWindow_getWidth(m_pApp->window);
        Height = ANativeWindow_getHeight(m_pApp->window);
    }

    m_pManager = new CBlendManager();
    m_pManager->init("configs/ScrollRainConfig.json", Width, Height);
}
