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
    if (m_pScreenQuad)          delete m_pScreenQuad;
    if (m_pSmallSnowForePlayer) delete m_pSmallSnowForePlayer;
    if (m_pSmallSnowBackPlayer) delete m_pSmallSnowBackPlayer;
    if (m_pBigSnowForePlayer)   delete m_pSmallSnowForePlayer;
    if (m_pBigSnowBackPlayer)   delete m_pSmallSnowBackPlayer;
    if (m_pSingleFramePlayer)   delete m_pSingleFramePlayer;
}

void CSnowRendererAsync::__initAlgorithm()
{
    int TextureCount = 128;
    EPictureType::EPictureType PictureType = EPictureType::PNG;
    m_pScreenQuad = CScreenQuad::getOrCreate();

    m_pSmallSnowForePlayer = new CAsyncSequenceFramePlayer("textures/SmallSnow_fore", TextureCount, PictureType);
    m_pSmallSnowForePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
    m_pSmallSnowBackPlayer = new CAsyncSequenceFramePlayer("textures/SmallSnow_back", TextureCount, PictureType);
    m_pSmallSnowBackPlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
    m_pBigSnowForePlayer   = new CAsyncSequenceFramePlayer("textures/BigSnow_fore", TextureCount, PictureType);
    m_pBigSnowForePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
    m_pBigSnowBackPlayer   = new CAsyncSequenceFramePlayer("textures/BigSnow_back", TextureCount, PictureType);
    m_pBigSnowBackPlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
    m_pSingleFramePlayer   = new CSingleTexturePlayer("textures/snowScene.png");
    m_pSingleFramePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
}

void CSnowRendererAsync::renderScene()
{
    glClearColor(0.345f,0.345f,0.345f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);

    if (m_EnableSmallSnowBack)
    {
        m_pSmallSnowBackPlayer->updateFrames();
        m_pScreenQuad->bindAndDraw();
    }
    if (m_EnableBigSnowBack)
    {
        m_pBigSnowBackPlayer->updateFrames();
        m_pScreenQuad->bindAndDraw();
    }

    m_pSingleFramePlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (m_EnableBigSnowFore)
    {
        m_pBigSnowForePlayer->updateFrames();
        m_pScreenQuad->bindAndDraw();
    }
    if (m_EnableSmallSnowFore)
    {
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