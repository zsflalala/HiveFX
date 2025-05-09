#include "SplashRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <android/asset_manager.h>
#include <json/json.h>
#include "Common.h"
#include "TimeUtils.h"
#include "ScreenQuad.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"
#include "SplashManager.h"
#include "JsonReader.h"

using namespace hiveVG;

CSplashRenderer::CSplashRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CSplashRenderer::~CSplashRenderer()
{
    if (m_pScreenQuad)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
    __deleteSafely(m_pSplashPlayer);
    __deleteSafely(m_pBackFramePlayer);
}

void CSplashRenderer::__initAlgorithm()
{
    std::string FileName = "configs/SplashPlayerConfig.json";
    CJsonReader JsonReader = CJsonReader(FileName);
    Json::Value SplashConfig = JsonReader.getObject("splash");
    Json::Value BackgroundConfig = JsonReader.getObject("background");
    Json::Value RainConfig = JsonReader.getObject("rain");

    std::string FramePath = SplashConfig["frames_path"].asString();
    std::string FrameType = SplashConfig["frames_type"].asString();
    int         FrameCount = SplashConfig["frames_count"].asInt();
    std::string PlayMode   = SplashConfig["play_mode"].asString();
    bool        IsLoop     = SplashConfig["loop"].asBool();
    int         PlayFPS    = SplashConfig["fps"].asInt();
    float       MoveSpeedX = SplashConfig["moving_speed"][0].asFloat();
    float       MoveSpeedY = SplashConfig["moving_speed"][1].asFloat();
    m_PlayScale = SplashConfig["scale"].asFloat();

    std::string BackgroundPath = BackgroundConfig["frames_path"].asString();
    m_pBackFramePlayer = new CSingleTexturePlayer(BackgroundPath);
    m_pBackFramePlayer->initTextureAndShaderProgram();

    int SequenceRows = 1, SequenceCols = 1;
    // String To Enum
    m_PictureType = EPictureType::FromString(FrameType);
    m_PlayMode    = EPlayType::FromString(PlayMode);

    m_pScreenQuad = CScreenQuad::getOrCreate();
    m_pSplashPlayer = new CSequenceFramePlayer(FramePath, SequenceRows, SequenceCols, FrameCount, m_PictureType);
    if(!m_pSplashPlayer->initTextureAndShaderProgram())
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    }
    m_pSplashPlayer->setFrameRate(PlayFPS);
    m_pSplashPlayer->setLoopPlayback(IsLoop);
    m_pSplashPlayer->setScreenUVScale(glm::vec2(m_PlayScale, m_PlayScale));
    m_pSplashPlayer->setScreenUVMovingSpeed(glm::vec2(MoveSpeedX, MoveSpeedY));

    m_pSplashManager = std::make_unique<CSplashManager>();
    int SplashNum = 40;
    for (int i = 0; i < SplashNum; i++)
    {
        m_pSplashManager->pushBack(m_pSplashPlayer->clone());
    }
    m_pSplashManager->initSequenceState(BackgroundPath, m_PlayScale);

    FramePath = RainConfig["frames_path"].asString();
    FrameType = RainConfig["frames_type"].asString();
    FrameCount = RainConfig["frames_count"].asInt();
    PlayMode = RainConfig["play_mode"].asString();
    IsLoop = RainConfig["loop"].asBool();
    PlayFPS = RainConfig["fps"].asInt();
    m_PictureType = EPictureType::FromString(FrameType);
    m_PlayMode = EPlayType::FromString(PlayMode);
    m_pRainPlayer = new CSequenceFramePlayer(FramePath, SequenceRows, SequenceCols, FrameCount, m_PictureType);
    m_pRainPlayer->initTextureAndShaderProgram();
    m_pRainPlayer->setFrameRate(PlayFPS);
    m_pRainPlayer->setLoopPlayback(IsLoop);

    m_LastFrameTime = CTimeUtils::getCurrentTime();
}

void CSplashRenderer::renderScene(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.0f,0.0f,0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//    m_pBackFramePlayer->updateFrame();
//    m_pScreenQuad->bindAndDraw();
    DeltaTime = 0.0417 * 2;
//    LOG_INFO(TAG_KEYWORD::RENDERER_TAG, " %lf ", DeltaTime);
    m_pSplashManager->updateFrameAndUV(DeltaTime);
    m_pSplashManager->updateSequenceState(DeltaTime);
    m_pSplashManager->draw(m_pScreenQuad);

//    m_pRainPlayer->updateFrameAndUV(DeltaTime);
//    m_pRainPlayer->draw(m_pScreenQuad);
}