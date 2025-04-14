#include "CombinedBigSnowFrameRenderer.h"
#include "Common.h"
#include "ScreenQuad.h"
#include "JsonReader.h"
#include "TimeUtils.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"

using namespace hiveVG;

CCombinedBigSnowFrameRenderer::CCombinedBigSnowFrameRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CCombinedBigSnowFrameRenderer::~CCombinedBigSnowFrameRenderer()
{
    if (m_pScreenQuad)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
    __deleteSafely(m_pCombineForeFramePlayer);
    __deleteSafely(m_pCombineBackFramePlayer);
    __deleteSafely(m_pBackFramePlayer);
}

void CCombinedBigSnowFrameRenderer::__initAlgorithm()
{
    std::string FileName   = "configs/MainAppConfig.json";
    CJsonReader JsonReader = CJsonReader(FileName);
    Json::Value SnowBackConfig = JsonReader.getObject("SmallSnowBack");
    Json::Value SnowForeConfig = JsonReader.getObject("SmallSnowFore");
    Json::Value BackConfig     = JsonReader.getObject("Background");

    std::string SnowFramePath = SnowBackConfig["frames_path"].asString();
    m_PictureType = EPictureType::FromString(SnowBackConfig["frames_type"].asString());
    int  SnowFrameCount = SnowBackConfig["frames_count"].asInt();
    int  SnowRows    = SnowBackConfig["sequenceRows"].asInt();
    int  SnowCols    = SnowBackConfig["sequenceCols"].asInt();
    bool SnowIsLoop  = SnowBackConfig["loop"].asBool();
    int  SnowPlayFPS = SnowBackConfig["fps"].asInt();

    m_pCombineBackFramePlayer = new CSequenceFramePlayer(SnowFramePath,SnowRows,SnowCols,SnowFrameCount,m_PictureType);
    m_pCombineBackFramePlayer->initTextureAndShaderProgram();
    m_pCombineBackFramePlayer->setFrameRate(SnowPlayFPS);
    m_pCombineBackFramePlayer->setLoopPlayback(SnowIsLoop);

    m_PictureType  = EPictureType::FromString(SnowForeConfig["frames_type"].asString());
    SnowFramePath  = SnowForeConfig["frames_path"].asString();
    SnowFrameCount = SnowForeConfig["frames_count"].asInt();
    SnowRows       = SnowForeConfig["sequenceRows"].asInt();
    SnowCols       = SnowForeConfig["sequenceCols"].asInt();
    SnowIsLoop     = SnowForeConfig["loop"].asBool();
    SnowPlayFPS    = SnowForeConfig["fps"].asInt();
    m_pCombineForeFramePlayer = new CSequenceFramePlayer(SnowFramePath,SnowRows,SnowCols,SnowFrameCount,m_PictureType);
    m_pCombineForeFramePlayer->initTextureAndShaderProgram();
    m_pCombineForeFramePlayer->setFrameRate(SnowPlayFPS);
    m_pCombineForeFramePlayer->setLoopPlayback(SnowIsLoop);

    std::string BackgroundPath = BackConfig["frames_path"].asString();
    m_pBackFramePlayer   = new CSingleTexturePlayer(BackgroundPath);
    m_pBackFramePlayer->initTextureAndShaderProgram();
    m_pScreenQuad   = CScreenQuad::getOrCreate();
    m_LastFrameTime = CTimeUtils::getCurrentTime();
}

void CCombinedBigSnowFrameRenderer::renderScene(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.345f,0.345f,0.345f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_pCombineBackFramePlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
    m_pCombineBackFramePlayer->draw(m_pScreenQuad);

    m_pBackFramePlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();

    m_pCombineForeFramePlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
    m_pCombineForeFramePlayer->draw(m_pScreenQuad);
}