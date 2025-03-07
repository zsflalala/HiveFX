#include "ConfigBlendRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <GLES3/gl3.h>
#include <assert.h>
#include "BlendManager.h"

using namespace hiveVG;

CConfigBlendRenderer::CConfigBlendRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CConfigBlendRenderer::~CConfigBlendRenderer()
{
    if(m_pManager) delete m_pManager;
}

void CConfigBlendRenderer::render()
{
    glClearColor(0.1f,0.2f,0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_pManager->render();
}

void CConfigBlendRenderer::__initAlgorithm()
{
    int Width = 0, Height = 0;
    assert(m_pApp->window != nullptr);
    if (m_pApp->window)
    {
        Width = ANativeWindow_getWidth(m_pApp->window);
        Height = ANativeWindow_getHeight(m_pApp->window);
    }

    m_pManager = new CBlendManager(m_pApp->activity->assetManager);
    //m_pManager->init("configs/BlendingConfig.json", Width, Height);
    m_pManager->init("configs/ConfigWithoutBillboard.json", Width, Height);
}

void CConfigBlendRenderer::switchRenderStatus(int vIndex)
{
    m_pManager->switchRenderStatus(vIndex);
}

void CConfigBlendRenderer::setLayerBlendMode(int vBlendMode)
{
    assert(vBlendMode >= 0 && vBlendMode < static_cast<int>(EBlendingMode::EBlendingMode::COUNT));
    auto Mode = static_cast<EBlendingMode::EBlendingMode>(vBlendMode);
    m_pManager->setBlendModeForAllLayer(Mode);
}