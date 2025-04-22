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
    if (m_pScreenQuad)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
    __deleteSafely(m_pSmallRainForePlayer);
    __deleteSafely(m_pSmallRainBackPlayer);
    __deleteSafely(m_pBigRainForePlayer);
    __deleteSafely(m_pBigRainBackPlayer);
    __deleteSafely(m_pSingleFramePlayer);
}

void CRainRendererAsync::__initAlgorithm()
{
    m_pScreenQuad = CScreenQuad::getOrCreate();

//    m_pSmallRainForePlayer = new CAsyncSequenceFramePlayer("textures/SmallRain_fore", m_TextureCount, m_PictureType);
//    m_pSmallRainForePlayer->initTextureAndShaderProgram();
//    m_pSmallRainBackPlayer = new CAsyncSequenceFramePlayer("textures/SmallRain_back", m_TextureCount, m_PictureType);
//    m_pSmallRainBackPlayer->initTextureAndShaderProgram();
//    m_pBigRainForePlayer   = new CAsyncSequenceFramePlayer("textures/BigRain_fore", m_TextureCount, m_PictureType);
//    m_pBigRainForePlayer->initTextureAndShaderProgram();
//    m_pBigRainBackPlayer   = new CAsyncSequenceFramePlayer("textures/BigRain_back", m_TextureCount, m_PictureType);
//    m_pBigRainBackPlayer->initTextureAndShaderProgram();
    m_pSingleFramePlayer   = new CSingleTexturePlayer("textures/snowScene.png");
    m_pSingleFramePlayer->initTextureAndShaderProgram();
}

void CRainRendererAsync::renderScene()
{
    glClearColor(0.345f,0.345f,0.345f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (m_EnableSmallRainBack)
    {
        if (m_pSmallRainBackPlayer == nullptr)
        {
            m_pSmallRainBackPlayer = new CAsyncSequenceFramePlayer("textures/SmallRain_fore", m_TextureCount, m_PictureType);
            m_pSmallRainBackPlayer->initTextureAndShaderProgram();
            sleep(0.5);
        }
        m_pSmallRainBackPlayer->updateFrames();
        m_pScreenQuad->bindAndDraw();
    }
    if (m_EnableBigRainBack)
    {
        if (m_pBigRainBackPlayer == nullptr)
        {
            m_pBigRainBackPlayer = new CAsyncSequenceFramePlayer("textures/BigRain_back", m_TextureCount, m_PictureType);
            m_pBigRainBackPlayer->initTextureAndShaderProgram();
            sleep(0.5);
        }
        m_pBigRainBackPlayer->updateFrames();
        m_pScreenQuad->bindAndDraw();
    }

    m_pSingleFramePlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();

    if (m_EnableBigRainFore)
    {
        if (m_pBigRainForePlayer == nullptr)
        {
            m_pBigRainForePlayer = new CAsyncSequenceFramePlayer("textures/BigRain_fore", m_TextureCount, m_PictureType);
            m_pBigRainForePlayer->initTextureAndShaderProgram();
            sleep(0.5);
        }
        m_pBigRainForePlayer->updateFrames();
        m_pScreenQuad->bindAndDraw();
    }
    if (m_EnableSmallRainFore)
    {
        if (m_pSmallRainForePlayer == nullptr)
        {
            m_pSmallRainForePlayer = new CAsyncSequenceFramePlayer("textures/SmallRain_fore", m_TextureCount, m_PictureType);
            m_pSmallRainForePlayer->initTextureAndShaderProgram();
            sleep(0.5);
        }
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