#include "ASTCPlayerRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "Common.h"
#include "TimeUtils.h"
#include "ScreenQuad.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"
#include "BillBoardManager.h"
#include "JsonReader.h"

using namespace hiveVG;

CASTCPlayerRenderer::CASTCPlayerRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CASTCPlayerRenderer::~CASTCPlayerRenderer()
{
    if (m_pScreenQuad)              delete m_pScreenQuad;
    if (m_pTestPlayer)              delete m_pTestPlayer;
    if (m_pSingleFramePlayer)       delete m_pSingleFramePlayer;
}

void CASTCPlayerRenderer::__initAlgorithm()
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
    Json::Value BackGroundConfig = JsonReader.getObject("Background");
    std::string ImgPath = BackGroundConfig["frames_path"].asString();
    m_pSingleFramePlayer   = new CSingleTexturePlayer(ImgPath,EPictureType::EPictureType::PNG);
    m_pSingleFramePlayer->initTextureAndShaderProgram();

    m_pTestPlayer->setFrameRate(PlayFPS);
    m_pTestPlayer->setLoopPlayback(IsLoop);
    m_pScreenQuad = CScreenQuad::getOrCreate();
    m_LastFrameTime = CTimeUtils::getCurrentTime();
}

void CASTCPlayerRenderer::renderScene(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(1.0f,1.0f,1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_pSingleFramePlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    m_pTestPlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
    m_pTestPlayer->draw(m_pScreenQuad);

}