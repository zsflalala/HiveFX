#include "TestSnowRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <android/asset_manager.h>
#include <json/json.h>
#include "Common.h"
#include "TimeUtils.h"
#include "ScreenQuad.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"
#include "BillBoardManager.h"
#include "JsonReader.h"

using namespace hiveVG;

CTestSnowRenderer::CTestSnowRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CTestSnowRenderer::~CTestSnowRenderer()
{
    if (m_pScreenQuad)              delete m_pScreenQuad;
    if (m_pSmallSnowForePlayer)     delete m_pSmallSnowForePlayer;
}

void CTestSnowRenderer::__initAlgorithm()
{
    int SequenceRows = 1, SequenceCols = 1, FrameCount = 64;
    bool UseCompressedPNG = true;
    std::string FramePath = "textures/SmallSnow_back_compressed";

    m_pScreenQuad = CScreenQuad::getOrCreate();
    m_pSmallSnowForePlayer = new CSequenceFramePlayer(FramePath, SequenceRows, SequenceCols, FrameCount, UseCompressedPNG);
    if(!m_pSmallSnowForePlayer->initTextureAndShaderProgram())
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    }

    FramePath = "textures/SmallSnow_fore_compressed";
    m_pSmallSnowBackPlayer = new CSequenceFramePlayer(FramePath, SequenceRows, SequenceCols, FrameCount, UseCompressedPNG);
    if(!m_pSmallSnowBackPlayer->initTextureAndShaderProgram())
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    }

    FramePath = "textures/snowScene.png";
    m_pSinglePlayer = new CSingleTexturePlayer(FramePath);
    m_pSinglePlayer->initTextureAndShaderProgram();
    m_LastFrameTime = CTimeUtils::getCurrentTime();
}

void CTestSnowRenderer::renderScene(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.345f, 0.345f, 0.345f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_pSmallSnowForePlayer->updateFrameAndUV(DeltaTime);
    m_pSmallSnowForePlayer->draw(m_pScreenQuad);
    m_pSinglePlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();
    m_pSmallSnowBackPlayer->updateFrameAndUV(DeltaTime);
    m_pSmallSnowForePlayer->draw(m_pScreenQuad);
}