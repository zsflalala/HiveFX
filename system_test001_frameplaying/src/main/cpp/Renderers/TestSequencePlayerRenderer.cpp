#include "TestSequencePlayerRenderer.h"
#include "Common.h"
#include "TimeUtils.h"
#include "ScreenQuad.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"
#include "BillBoardManager.h"
#include "JsonReader.h"

using namespace hiveVG;

CTestSequencePlayerRenderer::CTestSequencePlayerRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CTestSequencePlayerRenderer::~CTestSequencePlayerRenderer()
{
    if (m_pScreenQuad)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
    __deleteSafely(m_pTestPlayer);
}

void CTestSequencePlayerRenderer::__initAlgorithm()
{
    std::string FileName = "configs/BasicFramePlayerConfig.json";
    CJsonReader JsonReader = CJsonReader(FileName);
    Json::Value SequenceConfig = JsonReader.getObject("sequence_config");
    std::string FramePath = SequenceConfig["frames_path"].asString();
    std::string FrameType = SequenceConfig["frames_type"].asString();
    int         FrameCount = SequenceConfig["frames_count"].asInt();
    std::string PlayMode   = SequenceConfig["play_mode"].asString();
    bool        IsLoop     = SequenceConfig["loop"].asBool();
    int         PlayFPS    = SequenceConfig["fps"].asInt();
    float       MoveSpeedX = SequenceConfig["moving_speed"][0].asFloat();
    float       MoveSpeedY = SequenceConfig["moving_speed"][1].asFloat();
    m_PlayScale   = SequenceConfig["scale"].asFloat();
    m_UVOffset.x  = SequenceConfig["position"]["x"].asFloat();
    m_UVOffset.y  = SequenceConfig["position"]["y"].asFloat();

    int SequenceRows = 1, SequenceCols = 1;
    // String To Enum
    m_PictureType = EPictureType::FromString(FrameType);
    m_PlayMode    = EPlayType::FromString(PlayMode);

    m_pTestPlayer = new CSequenceFramePlayer(FramePath, SequenceRows, SequenceCols, FrameCount, m_PictureType);
    if(!m_pTestPlayer->initTextureAndShaderProgram())
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    }
    m_pTestPlayer->setFrameRate(PlayFPS);
    m_pTestPlayer->setLoopPlayback(IsLoop);
    if (m_PlayMode == EPlayType::PARTIAL)
    {
        m_pTestPlayer->setLifeCycle(true);
        m_pTestPlayer->setIsMoving(true);
//        m_pTestPlayer->setScreenUVOffset(m_UVOffset);
//        m_pTestPlayer->setScreenUVScale(glm::vec2(m_PlayScale, m_PlayScale));
//        m_pTestPlayer->setScreenUVMovingSpeed(glm::vec2(MoveSpeedX, MoveSpeedY));
    }
    m_pScreenQuad = CScreenQuad::getOrCreate();
    m_LastFrameTime = CTimeUtils::getCurrentTime();
}

void CTestSequencePlayerRenderer::renderScene(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.1f,0.1f,0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_pTestPlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
    m_pTestPlayer->draw(m_pScreenQuad);
}