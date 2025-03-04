#include "CombinedSnowCoverFrameRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "Common.h"
#include "ScreenQuad.h"
#include "JsonReader.h"
#include "TimeUtils.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"
#include "iostream"
using namespace hiveVG;

CCombinedSnowCoverFrameRenderer::CCombinedSnowCoverFrameRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CCombinedSnowCoverFrameRenderer::~CCombinedSnowCoverFrameRenderer()
{
    if (m_pScreenQuad)          delete m_pScreenQuad;
    if (m_pCombineFramePlayer) delete m_pCombineFramePlayer;
}

void CCombinedSnowCoverFrameRenderer::__initAlgorithm()
{
    m_pScreenQuad = CScreenQuad::getOrCreate();
    std::string  FileName = "configs/MainAppConfig.json";
    CJsonReader JsonReader=CJsonReader(m_pApp->activity->assetManager,FileName);
    Json::Value SnowBackConfig=JsonReader.getObject("SnowCover");

    std::string SnowFramePath = SnowBackConfig["frames_path"].asString();
    m_PictureType= EPictureType::FromString(SnowBackConfig["frames_type"].asString());
    int         SnowFrameCount = SnowBackConfig["frames_count"].asInt();
    int         SnowRows = SnowBackConfig["sequenceRows"].asInt();
    int         SnowCols = SnowBackConfig["sequenceCols"].asInt();
    bool        SnowIsLoop     = SnowBackConfig["loop"].asBool();
    int         SnowPlayFPS    = SnowBackConfig["fps"].asInt();

    m_pCombineFramePlayer = new CSequenceFramePlayer(SnowFramePath,SnowRows,SnowCols,SnowFrameCount,m_PictureType);
    m_pCombineFramePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
    m_pCombineFramePlayer->setFrameRate(SnowPlayFPS);
    m_pCombineFramePlayer->setLoopPlayback(SnowIsLoop);


}

void CCombinedSnowCoverFrameRenderer::renderScene(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.1f,0.1f,0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_pCombineFramePlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
    m_pCombineFramePlayer->draw(m_pScreenQuad);





}

