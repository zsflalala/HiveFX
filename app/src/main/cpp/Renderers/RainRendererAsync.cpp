#include "RainRendererAsync.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "Common.h"
#include "ScreenQuad.h"
#include "SingleTexturePlayer.h"
#include "AsyncSequenceFramePlayer.h"

using namespace hiveVG;

CRainRendererAsync::CRainRendererAsync(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CRainRendererAsync::~CRainRendererAsync()
{
    if (m_pScreenQuad)          delete m_pScreenQuad;
    if (m_pSmallRainForePlayer) delete m_pSmallRainForePlayer;
    if (m_pSmallRainBackPlayer) delete m_pSmallRainBackPlayer;
    if (m_pBigRainForePlayer)   delete m_pSmallRainForePlayer;
    if (m_pBigRainBackPlayer)   delete m_pSmallRainBackPlayer;
    if (m_pSingleFramePlayer)   delete m_pSingleFramePlayer;
}

void CRainRendererAsync::__initAlgorithm()
{
    int TextureCount = 64;
    EPictureType::EPictureType PictureType = EPictureType::PNG;
    m_pScreenQuad = CScreenQuad::getOrCreate();

    m_pSmallRainForePlayer = new CAsyncSequenceFramePlayer("textures/SmallRain_fore", TextureCount, PictureType);
    m_pSmallRainForePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
    m_pSmallRainBackPlayer = new CAsyncSequenceFramePlayer("textures/SmallRain_back", TextureCount, PictureType);
    m_pSmallRainBackPlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
    m_pBigRainForePlayer   = new CAsyncSequenceFramePlayer("textures/BigRain_fore", TextureCount, PictureType);
    m_pBigRainForePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
    m_pBigRainBackPlayer   = new CAsyncSequenceFramePlayer("textures/BigRain_back", TextureCount, PictureType);
    m_pBigRainBackPlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
    m_pSingleFramePlayer   = new CSingleTexturePlayer("textures/snowScene.png");
    m_pSingleFramePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
}

void CRainRendererAsync::renderScene()
{
    glClearColor(0.345f,0.345f,0.345f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);

    if (m_EnableSmallRainBack)
    {
        m_pSmallRainBackPlayer->updateFrames();
        m_pScreenQuad->bindAndDraw();
    }
    if (m_EnableBigRainBack)
    {
        m_pBigRainBackPlayer->updateFrames();
        m_pScreenQuad->bindAndDraw();
    }

    m_pSingleFramePlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (m_EnableBigRainFore)
    {
        m_pBigRainForePlayer->updateFrames();
        m_pScreenQuad->bindAndDraw();
    }
    if (m_EnableSmallRainFore)
    {
        m_pSmallRainForePlayer->updateFrames();
        m_pScreenQuad->bindAndDraw();
    }
}

void CRainRendererAsync::handleInput(ERenderType vInputType, bool vIsPointerDown)
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