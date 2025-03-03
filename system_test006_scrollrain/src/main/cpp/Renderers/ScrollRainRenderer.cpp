#include "ScrollRainRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <android/asset_manager.h>
#include "Common.h"
#include "TimeUtils.h"
#include "ScreenQuad.h"
#include "SlideWindow.h"

using namespace hiveVG;

CScrollRainRenderer::CScrollRainRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CScrollRainRenderer::~CScrollRainRenderer()
{
    if (m_pScreenQuad)  delete m_pScreenQuad;
    if (m_pSlideWindow) delete m_pSlideWindow;
}

void CScrollRainRenderer::renderScene(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.1f,0.1f,0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_pSlideWindow->updateFrame(vWindowWidth, vWindowHeight, DeltaTime * 100.0f, m_pScreenQuad);
}

void CScrollRainRenderer::__initAlgorithm()
{
    // TODO: 使用 json 文件初始化
    m_pScreenQuad = CScreenQuad::getOrCreate();
    m_pSlideWindow = new CSlideWindow("Textures/LongTexRain1920_10800.png", 10, "vertical");
    m_pSlideWindow->createProgram(m_pApp->activity->assetManager);
    m_pSlideWindow->loadTextures(m_pApp->activity->assetManager);

    m_LastFrameTime = CTimeUtils::getCurrentTime();
}
