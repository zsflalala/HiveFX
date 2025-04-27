#include "SceneRenderer.h"
#include "JsonReader.h"
#include "TimeUtils.h"
#include "ScreenQuad.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"

using namespace hiveVG;

CSceneRenderer::CSceneRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CSceneRenderer::~CSceneRenderer()
{
    if (m_pScreenQuad)     delete m_pScreenQuad;
    if (m_pSequencePlayer) delete m_pSequencePlayer;
}

void CSceneRenderer::render(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.0f,0.0f,0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//    m_pSingleTexture->updateFrame();
//    m_pScreenQuad->bindAndDraw();

    m_pSequencePlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
    m_pSequencePlayer->draw(m_pScreenQuad);
}

void CSceneRenderer::__initAlgorithm()
{
    std::string ConfigFile = "configs/RenderConfig.json";
    CJsonReader JsonReader = CJsonReader(ConfigFile);
    Json::Value SequenceConfig = JsonReader.getObject("sequence_config");
    std::string FramePath = SequenceConfig["frames_path"].asString();
    std::string FrameType = SequenceConfig["frames_type"].asString();
    int         FrameCount = SequenceConfig["frames_count"].asInt();
    std::string PlayMode   = SequenceConfig["play_mode"].asString();
    bool        IsLoop     = SequenceConfig["loop"].asBool();
    int         PlayFPS    = SequenceConfig["fps"].asInt();
    int         SequenceRows = SequenceConfig["ranks"]["rows"].asInt();
    int         SequenceCols = SequenceConfig["ranks"]["cols"].asInt();

    EPictureType::EPictureType PictureType = EPictureType::FromString(FrameType);
    EPlayType::EPlayType PlayType = EPlayType::FromString(PlayMode);

    m_pSequencePlayer = new CSequenceFramePlayer(FramePath, SequenceRows, SequenceCols, FrameCount, PictureType);

    if(!m_pSequencePlayer->initTextureAndShaderProgram())
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    }

    m_pSequencePlayer->setFrameRate(PlayFPS);
    m_pSequencePlayer->setLoopPlayback(IsLoop);

    std::string BackgroundPic = JsonReader.getString("background_path");
    m_pSingleTexture = new CSingleTexturePlayer(BackgroundPic);
    m_pSingleTexture->initTextureAndShaderProgram();

    m_OutputFile = JsonReader.getString("output");

    m_pScreenQuad = CScreenQuad::getOrCreate();
    m_LastFrameTime = CTimeUtils::getCurrentTime();
}
