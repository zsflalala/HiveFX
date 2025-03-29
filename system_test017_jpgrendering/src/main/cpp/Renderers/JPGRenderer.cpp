#include "JPGRenderer.h"

using namespace hiveVG;

#include "Common.h"
#include "ScreenQuad.h"
#include "JsonReader.h"
#include "TimeUtils.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"

using namespace hiveVG;

CJPGRenderer::CJPGRenderer()
{
    __initAlgorithm();
}

CJPGRenderer::~CJPGRenderer()
{
    if (m_pScreenQuad)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
    if (m_pForeJPGPlayer)
    {
        delete m_pForeJPGPlayer;
        m_pForeJPGPlayer = nullptr;
    }
    if (m_pBackJPGPlayer)
    {
        delete m_pBackJPGPlayer;
        m_pBackJPGPlayer = nullptr;
    }
    if (m_pBackgroundJPGPlayer)
    {
        delete m_pBackgroundJPGPlayer;
        m_pBackgroundJPGPlayer = nullptr;
    }
}

void CJPGRenderer::__initAlgorithm()
{
    std::string FileName   = "configs/MainAppConfig.json";
    CJsonReader JsonReader = CJsonReader(FileName);
    Json::Value SnowBackConfig = JsonReader.getObject("BigSnowBack");
    Json::Value SnowForeConfig = JsonReader.getObject("BigSnowFore");
    Json::Value BackConfig     = JsonReader.getObject("Background");

    std::string SnowFramePath = SnowBackConfig["frames_path"].asString();
    m_PictureType = EPictureType::FromString(SnowBackConfig["frames_type"].asString());
    int  SnowFrameCount = SnowBackConfig["frames_count"].asInt();
    int  SnowRows    = SnowBackConfig["sequenceRows"].asInt();
    int  SnowCols    = SnowBackConfig["sequenceCols"].asInt();
    bool SnowIsLoop  = SnowBackConfig["loop"].asBool();
    int  SnowPlayFPS = SnowBackConfig["fps"].asInt();

    m_pBackJPGPlayer = new CSequenceFramePlayer(SnowFramePath,SnowRows,SnowCols,SnowFrameCount,m_PictureType);
    m_pBackJPGPlayer->initTextureAndShaderProgram();
    m_pBackJPGPlayer->setFrameRate(SnowPlayFPS);
    m_pBackJPGPlayer->setLoopPlayback(SnowIsLoop);

    m_PictureType  = EPictureType::FromString(SnowForeConfig["frames_type"].asString());
    SnowFramePath  = SnowForeConfig["frames_path"].asString();
    SnowFrameCount = SnowForeConfig["frames_count"].asInt();
    SnowRows       = SnowForeConfig["sequenceRows"].asInt();
    SnowCols       = SnowForeConfig["sequenceCols"].asInt();
    SnowIsLoop     = SnowForeConfig["loop"].asBool();
    SnowPlayFPS    = SnowForeConfig["fps"].asInt();
    m_pForeJPGPlayer = new CSequenceFramePlayer(SnowFramePath,SnowRows,SnowCols,SnowFrameCount,m_PictureType);
    m_pForeJPGPlayer->initTextureAndShaderProgram();
    m_pForeJPGPlayer->setFrameRate(SnowPlayFPS);
    m_pForeJPGPlayer->setLoopPlayback(SnowIsLoop);

    std::string BackgroundPath = BackConfig["frames_path"].asString();
    m_pBackgroundJPGPlayer   = new CSingleTexturePlayer(BackgroundPath, m_PictureType);
    m_pBackgroundJPGPlayer->initTextureAndShaderProgram();
    m_pScreenQuad   = CScreenQuad::getOrCreate();
    m_LastFrameTime = CTimeUtils::getCurrentTime();
}

void CJPGRenderer::renderScene(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.345f,0.345f,0.345f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    m_pBackJPGPlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
    m_pBackJPGPlayer->draw(m_pScreenQuad);

    m_pBackgroundJPGPlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();

    m_pForeJPGPlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
    m_pForeJPGPlayer->draw(m_pScreenQuad);
}