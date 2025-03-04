#include "SnowRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <android/asset_manager.h>
#include <json/json.h>
#include "Common.h"
#include "TimeUtils.h"
#include "ScreenQuad.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"
#include "BillBoardManager.h"

using namespace hiveVG;

CTestSequencePlayerRenderer::CTestSequencePlayerRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CTestSequencePlayerRenderer::~CTestSequencePlayerRenderer()
{
    if (m_pScreenQuad)
        delete m_pScreenQuad;
    if (m_pSmallSnowForePlayer)
        delete m_pSmallSnowForePlayer;
    if (m_pSmallSnowBackPlayer)
        delete m_pSmallSnowBackPlayer;
    if (m_pBigSnowForePlayer)
        delete m_pBigSnowForePlayer;
    if (m_pBigSnowBackPlayer)
        delete m_pBigSnowBackPlayer;
    if (m_pSnowSceneSeqFramePlayer)
        delete m_pSnowSceneSeqFramePlayer;
}

void CTestSequencePlayerRenderer::__initAlgorithm()
{
    m_pScreenQuad = CScreenQuad::getOrCreate();

    EPictureType::EPictureType PictureType = EPictureType::PNG;
    int SequenceRows = 1, SequenceCols = 1, TextureCount = 128;

    std::string TexRootPath = "textures/SmallSnow_fore";
    m_pSmallSnowForePlayer = new CSequenceFramePlayer(TexRootPath, SequenceRows, SequenceCols, TextureCount, PictureType);
    if (!m_pSmallSnowForePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return;
    }
    m_pSmallSnowForePlayer->setFrameRate(60.0f);

    TexRootPath = "textures/SmallSnow_back";
    m_pSmallSnowBackPlayer = new CSequenceFramePlayer(TexRootPath, SequenceRows, SequenceCols, TextureCount, PictureType);
    if (!m_pSmallSnowBackPlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return;
    }
    m_pSmallSnowBackPlayer->setFrameRate(60.0f);

    TexRootPath = "textures/BigSnow_fore";
    m_pBigSnowForePlayer = new CSequenceFramePlayer(TexRootPath, SequenceRows, SequenceCols, TextureCount, PictureType);
    if (!m_pBigSnowForePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return;
    }
    m_pBigSnowForePlayer->setFrameRate(60.0f);

    TexRootPath = "textures/BigSnow_back";
    m_pBigSnowBackPlayer = new CSequenceFramePlayer(TexRootPath, SequenceRows, SequenceCols, TextureCount, PictureType);
    if (!m_pBigSnowBackPlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return;
    }
    m_pBigSnowBackPlayer->setFrameRate(60.0f);

    m_pSnowSceneSeqFramePlayer = new CSequenceFramePlayer("textures/SnowCover", SequenceRows, SequenceCols, TextureCount, PictureType);
    if (!m_pSnowSceneSeqFramePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SnowScene initialization falied.");
        return;
    }
    m_pSnowSceneSeqFramePlayer->setFrameRate(5);

    m_LastFrameTime = CTimeUtils::getCurrentTime();
}

void CTestSequencePlayerRenderer::renderScene(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime = m_CurrentTime;

    glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (m_EnableSmallSnowBack)
    {
        m_pSmallSnowBackPlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
        m_pSmallSnowBackPlayer->draw(m_pScreenQuad);
    }
    if (m_EnableBigSnowBack)
    {
        m_pBigSnowBackPlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
        m_pBigSnowBackPlayer->draw(m_pScreenQuad);
    }
    m_pSnowSceneSeqFramePlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
    m_pSnowSceneSeqFramePlayer->draw(m_pScreenQuad);
    if (m_EnableSmallSnowFore)
    {
        m_pSmallSnowForePlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
        m_pSmallSnowForePlayer->draw(m_pScreenQuad);
    }
    if (m_EnableBigSnowFore)
    {
        m_pBigSnowForePlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
        m_pBigSnowForePlayer->draw(m_pScreenQuad);
    }
}

void CTestSequencePlayerRenderer::handleInput(ERenderType vInputType, bool vIsPointerDown)
{
    if (vInputType == ERenderType::SMALL_SNOW_FORE)
    {
        if (vIsPointerDown && !m_PreviousPointerState)
            m_EnableSmallSnowFore = !m_EnableSmallSnowFore;
    }
    else if (vInputType == ERenderType::SMALL_SNOW_BACK)
    {
        if (vIsPointerDown && !m_PreviousPointerState)
            m_EnableSmallSnowBack = !m_EnableSmallSnowBack;
    }
    else if (vInputType == ERenderType::BIG_SNOW_FORE)
    {
        if (vIsPointerDown && !m_PreviousPointerState)
            m_EnableBigSnowFore = !m_EnableBigSnowFore;
    }
    else if (vInputType == ERenderType::BIG_SNOW_BACK)
    {
        if (vIsPointerDown && !m_PreviousPointerState)
            m_EnableBigSnowBack = !m_EnableBigSnowBack;
    }
    m_PreviousPointerState = vIsPointerDown;
}
