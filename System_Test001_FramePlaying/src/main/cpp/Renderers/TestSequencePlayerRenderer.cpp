#include "TestSequencePlayerRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <android/asset_manager.h>
#include <json/json.h>
#include "Common.h"
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
    if (m_pScreenQuad)              delete m_pScreenQuad;
    if (m_pSmallSnowForePlayer)     delete m_pSmallSnowForePlayer;
}

void CTestSequencePlayerRenderer::__initAlgorithm()
{
    // TODO : change the string to be universal
    std::string FileName = "configs/BasicFramePlayerConfig.json";
    CJsonReader JsonReader = CJsonReader(m_pApp->activity->assetManager, FileName);
    Json::Value SequenceConfig = JsonReader.getObject("sequence_config");
    std::string FramePath = SequenceConfig["frames_path"].asString();
    std::string FrameType = SequenceConfig["frames_type"].asString();
    int         FrameCount = SequenceConfig["frames_count"].asInt();
    std::string PlayMode  = SequenceConfig["play_mode"].asString();
    bool        IsLoop    = SequenceConfig["loop"].asBool();
    int         PlayFPS   = SequenceConfig["fps"].asInt();
    m_PlayScale   = SequenceConfig["scale"].asFloat();
    m_UVOffset.x  = SequenceConfig["position"]["x"].asFloat();
    m_UVOffset.y  = SequenceConfig["position"]["y"].asFloat();

    int SequenceRows = 1, SequenceCols = 1;
    EPictureType PictureType = EPictureType::PNG;

    if (FrameType == "jpg")
        PictureType = EPictureType::JPG;
    else if (FrameType == "webp")
        PictureType = EPictureType::WEBP;

    if (PlayMode == "partial")
        m_PlayMode = EPlayType::PARTICAL;

    m_pScreenQuad = CScreenQuad::getOrCreate();
    m_pSmallSnowForePlayer = new CSequenceFramePlayer(FramePath, SequenceRows, SequenceCols, FrameCount, PictureType);
    if(!m_pSmallSnowForePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    }
    m_pSmallSnowForePlayer->setFrameRate(PlayFPS);
    m_pSmallSnowForePlayer->setLoopPlayback(IsLoop);
    m_pSmallSnowForePlayer->setScreenUVScale(glm::vec2(m_PlayScale, m_PlayScale));
    m_pSmallSnowForePlayer->setScreenUVOffset(m_UVOffset);
    if (m_PlayMode == EPlayType::PARTICAL)
        m_pSmallSnowForePlayer->setIsMoving(true);
    m_LastFrameTime = __getCurrentTime();
}

void CTestSequencePlayerRenderer::renderScene(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = __getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.1f,0.1f,0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_pSmallSnowForePlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
    m_pSmallSnowForePlayer->draw(m_pScreenQuad);
}

double CTestSequencePlayerRenderer::__getCurrentTime()
{
    struct timeval tv{};
    gettimeofday(&tv, nullptr);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}