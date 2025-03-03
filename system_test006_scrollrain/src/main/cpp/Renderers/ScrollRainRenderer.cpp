#include "ScrollRainRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <android/asset_manager.h>
#include <functional>
#include "Common.h"
#include "TimeUtils.h"
#include "ScreenQuad.h"
#include "SlideWindow.h"
#include "SingleTexturePlayer.h"
#include "TextureBlender.h"

using namespace hiveVG;

CScrollRainRenderer::CScrollRainRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CScrollRainRenderer::~CScrollRainRenderer()
{
    if (m_pScreenQuad)  delete m_pScreenQuad;
    if (m_pSlideWindow) delete m_pSlideWindow;
    if (m_pBackground)  delete m_pBackground;
    if (m_pTexBlender)  delete m_pTexBlender;
}

void CScrollRainRenderer::renderScene(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.1f,0.1f,0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    std::function<DrawCallFunc> BackGroundDrawCallFunc = [this]()
    {
        m_pBackground->updateFrame();
        m_pScreenQuad->bindAndDraw();
    };
    m_pTexBlender->drawAndBlend(BackGroundDrawCallFunc);

    std::function<DrawCallFunc> SlideWindowDrawCallFunc = [this, vWindowWidth, vWindowHeight, DeltaTime]()
    {
        m_pSlideWindow->updateFrame(vWindowWidth, vWindowHeight, DeltaTime * 100.0f, m_pScreenQuad);
    };
    m_pTexBlender->drawAndBlend(SlideWindowDrawCallFunc);
    //m_pSlideWindow->updateFrame(vWindowWidth, vWindowHeight, DeltaTime * 100.0f, m_pScreenQuad);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_pTexBlender->blitToScreen();
}

void CScrollRainRenderer::__initAlgorithm()
{
    // TODO: 使用 json 文件初始化
    m_pScreenQuad = CScreenQuad::getOrCreate();
    m_pSlideWindow = new CSlideWindow("Textures/LongTexRain1920_10800.png", -70, "vertical");
    m_pSlideWindow->createProgram(m_pApp->activity->assetManager);
    m_pSlideWindow->loadTextures(m_pApp->activity->assetManager);

    m_pBackground = new CSingleTexturePlayer("Textures/Background.png");
    m_pBackground->initTextureAndShaderProgram(m_pApp->activity->assetManager);

    m_pTexBlender = new CTextureBlender();
    int Width = 0, Height = 0;
    assert(m_pApp->window != nullptr);
    if (m_pApp->window != nullptr)
    {
        Width = ANativeWindow_getWidth(m_pApp->window);
        Height = ANativeWindow_getHeight(m_pApp->window);
    }
    m_pTexBlender->init(m_pApp->activity->assetManager,Width, Height);

    m_LastFrameTime = CTimeUtils::getCurrentTime();
}
