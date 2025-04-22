#include "SnowRendererAsync.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "Common.h"
#include "ScreenQuad.h"
#include "SingleTexturePlayer.h"
#include "AsyncSequenceFramePlayer.h"

using namespace hiveVG;

CSnowRendererAsync::CSnowRendererAsync(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CSnowRendererAsync::~CSnowRendererAsync()
{
    if (m_pScreenQuad)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
    __deleteSafely(m_pSmallSnowForePlayer);
    __deleteSafely(m_pSmallSnowBackPlayer);
    __deleteSafely(m_pBigSnowForePlayer);
    __deleteSafely(m_pBigSnowBackPlayer);
    __deleteSafely(m_pSingleFramePlayer);
}

void CSnowRendererAsync::__initAlgorithm()
{
//    m_pSmallSnowForePlayer = new CAsyncSequenceFramePlayer("textures/SmallSnow_fore", m_TextureCount, m_PictureType);
//    m_pSmallSnowForePlayer->initTextureAndShaderProgram();
//    m_pSmallSnowBackPlayer = new CAsyncSequenceFramePlayer("textures/SmallSnow_back", m_TextureCount, m_PictureType);
//    m_pSmallSnowBackPlayer->initTextureAndShaderProgram();
//    m_pBigSnowForePlayer   = new CAsyncSequenceFramePlayer("textures/BigSnow_fore", m_TextureCount, m_PictureType);
//    m_pBigSnowForePlayer->initTextureAndShaderProgram();
//    m_pBigSnowBackPlayer   = new CAsyncSequenceFramePlayer("textures/BigSnow_back", m_TextureCount, m_PictureType);
//    m_pBigSnowBackPlayer->initTextureAndShaderProgram();
    m_pSingleFramePlayer   = new CSingleTexturePlayer("textures/snowScene.png");
    m_pSingleFramePlayer->initTextureAndShaderProgram();
    m_pScreenQuad = CScreenQuad::getOrCreate();
}

void CSnowRendererAsync::renderScene()
{
    glClearColor(0.345f,0.345f,0.345f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (m_EnableSmallSnowBack)
    {
        if (m_pSmallSnowBackPlayer == nullptr)
        {
            m_pSmallSnowBackPlayer = new CAsyncSequenceFramePlayer("textures/SmallSnow_back", m_TextureCount, m_PictureType);
            m_pSmallSnowBackPlayer->initTextureAndShaderProgram();
            sleep(1);
        }
        m_pSmallSnowBackPlayer->updateFrames();
        m_pScreenQuad->bindAndDraw();
    }
    if (m_EnableBigSnowBack)
    {
        if (m_pBigSnowBackPlayer == nullptr)
        {
            m_pBigSnowBackPlayer = new CAsyncSequenceFramePlayer("textures/BigSnow_back", m_TextureCount, m_PictureType);
            m_pBigSnowBackPlayer->initTextureAndShaderProgram();
            sleep(1);
        }
        m_pBigSnowBackPlayer->updateFrames();
        m_pScreenQuad->bindAndDraw();
    }

    m_pSingleFramePlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();

    if (m_EnableBigSnowFore)
    {
        if (m_pBigSnowForePlayer == nullptr)
        {
            m_pBigSnowForePlayer = new CAsyncSequenceFramePlayer("textures/BigSnow_fore", m_TextureCount, m_PictureType);
            m_pBigSnowForePlayer->initTextureAndShaderProgram();
            sleep(1);
        }
        m_pBigSnowForePlayer->updateFrames();
        m_pScreenQuad->bindAndDraw();
    }
    if (m_EnableSmallSnowFore)
    {
        if (m_pSmallSnowForePlayer == nullptr)
        {
            m_pSmallSnowForePlayer = new CAsyncSequenceFramePlayer("textures/SmallSnow_fore", m_TextureCount, m_PictureType);
            m_pSmallSnowForePlayer->initTextureAndShaderProgram();
            sleep(1);
        }
        m_pSmallSnowForePlayer->updateFrames();
        m_pScreenQuad->bindAndDraw();
    }
}

void CSnowRendererAsync::handleInput(ERenderType vInputType, bool vIsPointerDown)
{
    if (vInputType == ERenderType::SMALL_SNOW_FORE)
    {
        // 在持续摁下时只改变一次 m_EnableSmallSnowFore 的变量值
        if (vIsPointerDown && !m_PreviousPointerState) m_EnableSmallSnowFore = !m_EnableSmallSnowFore;
    }
    else if (vInputType == ERenderType::SMALL_SNOW_BACK)
    {
        if (vIsPointerDown && !m_PreviousPointerState) m_EnableSmallSnowBack = !m_EnableSmallSnowBack;
    }
    else if (vInputType == ERenderType::BIG_SNOW_FORE)
    {
        if (vIsPointerDown && !m_PreviousPointerState) m_EnableBigSnowFore   = !m_EnableBigSnowFore;
    }
    else if (vInputType == ERenderType::BIG_SNOW_BACK)
    {
        if (vIsPointerDown && !m_PreviousPointerState) m_EnableBigSnowBack   = !m_EnableBigSnowBack;
    }
    m_PreviousPointerState = vIsPointerDown;
}