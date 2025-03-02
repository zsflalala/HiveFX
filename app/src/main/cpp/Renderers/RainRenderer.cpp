#include "RainRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "Common.h"
#include "ScreenQuad.h"
#include "TimeUtils.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"

using namespace hiveVG;

CRainRenderer::CRainRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CRainRenderer::~CRainRenderer()
{
    if (m_pScreenQuad)          delete m_pScreenQuad;
    if (m_pSmallRainForePlayer) delete m_pSmallRainForePlayer;
    if (m_pSmallRainBackPlayer) delete m_pSmallRainBackPlayer;
    if (m_pBigRainForePlayer)   delete m_pSmallRainForePlayer;
    if (m_pBigRainBackPlayer)   delete m_pSmallRainBackPlayer;
    if (m_pBackFramePlayer)     delete m_pBackFramePlayer;
}

void CRainRenderer::__initAlgorithm()
{
    int SequenceRows = 1, SequenceCols = 1, TextureCount = 64;
    EPictureType::EPictureType PictureType = EPictureType::PNG;
    m_pScreenQuad = CScreenQuad::getOrCreate();

    m_pSmallRainForePlayer = new CSequenceFramePlayer("Textures/SmallRain_fore", SequenceRows, SequenceCols, TextureCount, PictureType);
    m_pSmallRainForePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
    m_pSmallRainForePlayer->setFrameRate(60.0);

    m_pSmallRainBackPlayer = new CSequenceFramePlayer("Textures/SmallRain_back", SequenceRows, SequenceCols, TextureCount, PictureType);
    m_pSmallRainBackPlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
    m_pSmallRainBackPlayer->setFrameRate(60.0);

    m_pBigRainForePlayer   = new CSequenceFramePlayer("Textures/BigRain_fore", SequenceRows, SequenceCols, TextureCount, PictureType);
    m_pBigRainForePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
    m_pBigRainForePlayer->setFrameRate(60.0);

    m_pBigRainBackPlayer   = new CSequenceFramePlayer("Textures/BigRain_back", SequenceRows, SequenceCols, TextureCount, PictureType);
    m_pBigRainBackPlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
    m_pBigRainBackPlayer->setFrameRate(60.0);

    m_pBackFramePlayer     = new CSingleTexturePlayer("Textures/snowScene.png");
    m_pBackFramePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
}

void CRainRenderer::renderScene(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.1f,0.1f,0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (m_EnableSmallRainBack)
    {
        m_pSmallRainBackPlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
        m_pSmallRainBackPlayer->draw(m_pScreenQuad);
    }
    if (m_EnableBigRainBack)
    {
        m_pBigRainBackPlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
        m_pBigRainBackPlayer->draw(m_pScreenQuad);
    }
    m_pBackFramePlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();
    if (m_EnableSmallRainFore)
    {
        m_pSmallRainForePlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
        m_pSmallRainForePlayer->draw(m_pScreenQuad);
    }
    if (m_EnableBigRainFore)
    {
        m_pBigRainForePlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
        m_pBigRainForePlayer->draw(m_pScreenQuad);
    }
}

void CRainRenderer::handleInput(ERenderType vInputType, bool vIsPointerDown)
{
    if (vInputType == ERenderType::SMALL_RAIN_FORE)
    {
        // 在持续摁下时只改变一次 m_EnableSmallRainFore 的变量值
        if (vIsPointerDown && !m_PreviousPointerState) m_EnableSmallRainFore = !m_EnableSmallRainFore;
    }
    else if (vInputType == ERenderType::SMALL_RAIN_BACK)
    {
        if (vIsPointerDown && !m_PreviousPointerState) m_EnableSmallRainBack = !m_EnableSmallRainBack;
    }
    else if (vInputType == ERenderType::BIG_RAIN_FORE)
    {
        if (vIsPointerDown && !m_PreviousPointerState) m_EnableBigRainFore   = !m_EnableBigRainFore;
    }
    else if (vInputType == ERenderType::BIG_RAIN_BACK)
    {
        if (vIsPointerDown && !m_PreviousPointerState) m_EnableBigRainBack   = !m_EnableBigRainBack;
    }
    m_PreviousPointerState = vIsPointerDown;
}