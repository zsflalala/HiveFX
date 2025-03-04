#include "CombinedSmallRainFrameRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "Common.h"
#include "ScreenQuad.h"
#include "JsonReader.h"
#include "TimeUtils.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"
using namespace hiveVG;

CCombinedSmallRainFrameRenderer::CCombinedSmallRainFrameRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CCombinedSmallRainFrameRenderer::~CCombinedSmallRainFrameRenderer()
{
    if (m_pScreenQuad)          delete m_pScreenQuad;
    if (m_pCombineBackFramePlayer) delete m_pCombineBackFramePlayer;
    if (m_pCombineForeFramePlayer) delete m_pCombineForeFramePlayer;
    if (m_pBackFramePlayer)     delete m_pBackFramePlayer;
}

void CCombinedSmallRainFrameRenderer::__initAlgorithm()
{
    m_pScreenQuad = CScreenQuad::getOrCreate();
    std::string  FileName = "configs/MainAppConfig.json";
    CJsonReader JsonReader=CJsonReader(m_pApp->activity->assetManager,FileName);
    Json::Value RainBackConfig=JsonReader.getObject("SmallRainBack");
    Json::Value RainForeConfig=JsonReader.getObject("SmallRainFore");
    Json::Value BackConfig=JsonReader.getObject("BackGround");

    std::string RainFramePath = RainBackConfig["frames_path"].asString();
    m_PictureType= EPictureType::FromString(RainBackConfig["frames_type"].asString());
    int         RainFrameCount = RainBackConfig["frames_count"].asInt();
    int         RainRows = RainBackConfig["sequenceRows"].asInt();
    int         RainCols = RainBackConfig["sequenceCols"].asInt();
    bool        RainIsLoop     = RainBackConfig["loop"].asBool();
    int         RainPlayFPS    = RainBackConfig["fps"].asInt();

    m_pCombineBackFramePlayer = new CSequenceFramePlayer(RainFramePath,RainRows,RainCols,RainFrameCount,m_PictureType);
    m_pCombineBackFramePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
    m_pCombineBackFramePlayer->setFrameRate(RainPlayFPS);
    m_pCombineBackFramePlayer->setLoopPlayback(RainIsLoop);

    m_PictureType= EPictureType::FromString(RainForeConfig["frames_type"].asString());
    RainFramePath = RainForeConfig["frames_path"].asString();
    RainFrameCount = RainForeConfig["frames_count"].asInt();
    RainRows = RainForeConfig["sequenceRows"].asInt();
    RainCols = RainForeConfig["sequenceCols"].asInt();
    RainIsLoop     = RainForeConfig["loop"].asBool();
    RainPlayFPS    = RainForeConfig["fps"].asInt();
    m_pCombineForeFramePlayer = new CSequenceFramePlayer(RainFramePath,RainRows,RainCols,RainFrameCount,m_PictureType);
    m_pCombineForeFramePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
    m_pCombineForeFramePlayer->setFrameRate(RainPlayFPS);
    m_pCombineForeFramePlayer->setLoopPlayback(RainIsLoop);

    m_pBackFramePlayer   = new CSingleTexturePlayer(BackConfig["frames_path"].asString());
    m_pBackFramePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);

}

void CCombinedSmallRainFrameRenderer::renderScene(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.1f,0.1f,0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);

    m_pCombineBackFramePlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
    m_pCombineBackFramePlayer->draw(m_pScreenQuad);

    m_pBackFramePlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_pCombineForeFramePlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
    m_pCombineForeFramePlayer->draw(m_pScreenQuad);


}

