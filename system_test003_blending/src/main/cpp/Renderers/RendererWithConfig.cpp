#include "RendererWithConfig.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <GLES3/gl3.h>
#include <assert.h>
#include "BlendManager.h"

using namespace hiveVG;

CRendererWithConfig::CRendererWithConfig(android_app *vApp) : m_pApp(vApp)
{ }

CRendererWithConfig::~CRendererWithConfig()
{
    if (m_pManager != nullptr)
    {
        delete m_pManager;
        m_pManager = nullptr;
    }
}

bool CRendererWithConfig::init(const std::string& vConfigFile)
{
    int Width = 0, Height = 0;
    assert(m_pApp->window != nullptr);
    if (m_pApp->window)
    {
        Width = ANativeWindow_getWidth(m_pApp->window);
        Height = ANativeWindow_getHeight(m_pApp->window);
    }

    m_pManager = new CBlendManager();
    m_pManager->init(vConfigFile, Width, Height);
    return true;
}

void CRendererWithConfig::render()
{
    m_pManager->render();
}

void CRendererWithConfig::switchRenderStatus(int vIndex)
{
    m_pManager->switchRenderStatus(vIndex);
}

void CRendererWithConfig::setLayerBlendMode(int vBlendMode)
{
    assert(vBlendMode >= 0 && vBlendMode < static_cast<int>(EBlendingMode::EBlendingMode::COUNT));
    auto Mode = static_cast<EBlendingMode::EBlendingMode>(vBlendMode);
    m_pManager->setBlendModeForAllLayer(Mode);
    LOG_INFO(TAG_KEYWORD::RENDERER_TAG, "Set blending mode: %s", EBlendingMode::ToString(Mode));
}