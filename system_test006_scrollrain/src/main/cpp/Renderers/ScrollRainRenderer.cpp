#include "ScrollRainRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <android/asset_manager.h>
#include <functional>
#include "Common.h"
#include "TimeUtils.h"
#include "ScreenQuad.h"
#include "SingleTexturePlayer.h"
#include "TextureBlender.h"
#include "SlideWindow.h"

using namespace hiveVG;

CScrollRainRenderer::CScrollRainRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CScrollRainRenderer::~CScrollRainRenderer()
{
    if (m_pScreenQuad)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
    if (m_pSlideWindow)
    {
        delete m_pSlideWindow;
        m_pSlideWindow = nullptr;
    }
    if (m_pBackground)
    {
        delete m_pBackground;
        m_pBackground = nullptr;
    }
    if (m_pTexBlender)
    {
        delete m_pTexBlender;
        m_pTexBlender = nullptr;
    }
}

void CScrollRainRenderer::renderScene(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.35f,0.35f,0.35f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

//    std::function<DrawCallFunc> BackGroundDrawCallFunc = [this]()
//    {
//        m_pBackground->updateFrame();
//        m_pScreenQuad->bindAndDraw();
//    };
//    m_pTexBlender->drawAndBlend(BackGroundDrawCallFunc);
//
//    std::function<DrawCallFunc> SlideWindowDrawCallFunc = [this, vWindowWidth, vWindowHeight, DeltaTime]()
//    {
//        m_pSlideWindow->updateFrameAndDraw(vWindowWidth, vWindowHeight, DeltaTime * 100.0f, m_pScreenQuad);
//    };
//    m_pTexBlender->drawAndBlend(SlideWindowDrawCallFunc);
    m_pSlideWindow->updateFrameAndDraw(vWindowWidth, vWindowHeight, DeltaTime * 100.0f, m_pScreenQuad);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    m_pTexBlender->blit();
}

void CScrollRainRenderer::__initAlgorithm()
{
    m_pScreenQuad = CScreenQuad::getOrCreate();
    m_pSlideWindow = new CSlideWindow("Textures/LongTexRain1920_10800.webp", -90, "vertical", EPictureType::WEBP);
    m_pSlideWindow->initTextureAndShaderProgram();

//    m_pBackground = new CSingleTexturePlayer("textures/Background.png");
//    m_pBackground->initTextureAndShaderProgram();
//
//    m_pTexBlender = new CTextureBlender();
    int Width = 0, Height = 0;
    assert(m_pApp->window != nullptr);
    if (m_pApp->window != nullptr)
    {
        Width = ANativeWindow_getWidth(m_pApp->window);
        Height = ANativeWindow_getHeight(m_pApp->window);
    }
//    m_pTexBlender->init(Width, Height);

    m_LastFrameTime = CTimeUtils::getCurrentTime();
}
