#include "CombinedSmallSnowFrameRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "Common.h"
#include "ScreenQuad.h"
#include "JsonReader.h"
#include "TimeUtils.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"
#include "iostream"
using namespace hiveVG;

CCombinedSmallSnowFrameRenderer::CCombinedSmallSnowFrameRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CCombinedSmallSnowFrameRenderer::~CCombinedSmallSnowFrameRenderer()
{
    if (m_pScreenQuad)          delete m_pScreenQuad;
    if (m_pCombineBackFramePlayer) delete m_pCombineBackFramePlayer;
    if (m_pCombineForeFramePlayer) delete m_pCombineForeFramePlayer;
    if (m_pBackFramePlayer)     delete m_pBackFramePlayer;
}

void CCombinedSmallSnowFrameRenderer::__initAlgorithm()
{
    m_pScreenQuad = CScreenQuad::getOrCreate();
    std::string  FileName = "configs/MainAppConfig.json";
    CJsonReader JsonReader=CJsonReader(m_pApp->activity->assetManager,FileName);
    Json::Value SnowBackConfig=JsonReader.getObject("SmallSnowBack");
    Json::Value SnowForeConfig=JsonReader.getObject("SmallSnowFore");
    Json::Value BackConfig=JsonReader.getObject("BackGround");

    std::string SnowFramePath = SnowBackConfig["frames_path"].asString();
    m_PictureType= EPictureType::FromString(SnowBackConfig["frames_type"].asString());
    int         SnowFrameCount = SnowBackConfig["frames_count"].asInt();
    int         SnowRows = SnowBackConfig["sequenceRows"].asInt();
    int         SnowCols = SnowBackConfig["sequenceCols"].asInt();
    bool        SnowIsLoop     = SnowBackConfig["loop"].asBool();
    int         SnowPlayFPS    = SnowBackConfig["fps"].asInt();

    m_pCombineBackFramePlayer = new CSequenceFramePlayer(SnowFramePath,SnowRows,SnowCols,SnowFrameCount,m_PictureType);
    m_pCombineBackFramePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
    m_pCombineBackFramePlayer->setFrameRate(SnowPlayFPS);
    m_pCombineBackFramePlayer->setLoopPlayback(SnowIsLoop);

    m_PictureType= EPictureType::FromString(SnowForeConfig["frames_type"].asString());
    SnowFramePath = SnowForeConfig["frames_path"].asString();
    SnowFrameCount = SnowForeConfig["frames_count"].asInt();
    SnowRows = SnowForeConfig["sequenceRows"].asInt();
    SnowCols = SnowForeConfig["sequenceCols"].asInt();
    SnowIsLoop     = SnowForeConfig["loop"].asBool();
    SnowPlayFPS    = SnowForeConfig["fps"].asInt();
    m_pCombineForeFramePlayer = new CSequenceFramePlayer(SnowFramePath,SnowRows,SnowCols,SnowFrameCount,m_PictureType);
    m_pCombineForeFramePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
    m_pCombineForeFramePlayer->setFrameRate(SnowPlayFPS);
    m_pCombineForeFramePlayer->setLoopPlayback(SnowIsLoop);

    m_pBackFramePlayer   = new CSingleTexturePlayer(BackConfig["frames_path"].asString());
    m_pBackFramePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);

}

void CCombinedSmallSnowFrameRenderer::renderScene(int vWindowWidth, int vWindowHeight)
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

