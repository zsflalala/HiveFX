#include "RainMultiChannelSeqRenderer.h"
#include "Common.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "JsonReader.h"
#include "TimeUtils.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"

using namespace hiveVG;

CRainMultiChannelSeqRenderer::CRainMultiChannelSeqRenderer()
{
    __initAlgorithm();
}

CRainMultiChannelSeqRenderer::~CRainMultiChannelSeqRenderer()
{
    if (m_pScreenQuad != nullptr)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
    __deleteSafely(m_pRainSeqPlayer);
    __deleteSafely(m_pBackgroundPlayer);
    __deleteSafely(m_pLightingPlayer);
    __deleteSafely(m_pCloudPlayer);
}

bool CRainMultiChannelSeqRenderer::__initAlgorithm()
{
    std::string FileName   = "configs/RainMultiChannelSeqConfig.json";
    CJsonReader JsonReader = CJsonReader(FileName);
    Json::Value RainConfig = JsonReader.getObject("Rain");
    std::string RainPath   =  RainConfig["frames_path"].asString();
    std::string RainFrameType = RainConfig["frames_type"].asString();
    int RainTextureCount   =  RainConfig["frames_count"].asInt();
    int RainOneTextureFrames = RainConfig["one_texture_frames"].asInt();
    float RainFramePerSecond = RainConfig["fps"].asFloat();
    std::string RainVertexShader = RainConfig["vertex_shader"].asString();
    std::string RainFragShader   = RainConfig["fragment_shader"].asString();

    Json::Value BackGroundConfig = JsonReader.getObject("Background");
    std::string BackImgPath      = BackGroundConfig["frames_path"].asString();
    std::string BackFrameType    = BackGroundConfig["frames_type"].asString();
    std::string BackVertexShader = BackGroundConfig["vertex_shader"].asString();
    std::string BackFragShader   = BackGroundConfig["fragment_shader"].asString();
    EPictureType::EPictureType BackPicType = EPictureType::FromString(BackFrameType);

    Json::Value LightingConfig = JsonReader.getObject("Lighting");
    std::string LightingPath = LightingConfig["frames_path"].asString();
    std::string LightingType = LightingConfig["frames_type"].asString();
    int   LightingFrameCount = LightingConfig["frames_count"].asInt();
    int   LightingOneTextureFrames = LightingConfig["one_texture_frames"].asInt();
    float LightingPlayFPS    = LightingConfig["fps"].asFloat();
    std::string LightingVertexShader = LightingConfig["vertex_shader"].asString();
    std::string LightingFragShader   = LightingConfig["fragment_shader"].asString();
    EPictureType::EPictureType LightingPicType = EPictureType::FromString(LightingType);

    EPictureType::EPictureType RainPictureType = EPictureType::FromString(RainFrameType);
    m_pRainSeqPlayer = new CSequenceFramePlayer(RainPath, RainTextureCount, RainOneTextureFrames, RainFramePerSecond, RainPictureType);
    m_pRainSeqPlayer->initTextureAndShaderProgram(RainVertexShader, RainFragShader);

    m_pBackgroundPlayer = new CSingleTexturePlayer(BackImgPath, BackPicType);
    m_pBackgroundPlayer->initTextureAndShaderProgram(BackVertexShader, BackFragShader);

    m_pLightingPlayer = new CSequenceFramePlayer(LightingPath, LightingFrameCount, LightingOneTextureFrames, LightingPlayFPS, LightingPicType);
    m_pLightingPlayer->initTextureAndShaderProgram(LightingVertexShader, LightingFragShader);

    m_pScreenQuad   = CScreenQuad::getOrCreate();
    m_LastFrameTime = CTimeUtils::getCurrentTime();
    return true;
}

void CRainMultiChannelSeqRenderer::renderScene(ERenderChannel vRenderChannel)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.345f,0.345f,0.345f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    m_pBackgroundPlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();

    m_pLightingPlayer->updateLerpQuantFrame(DeltaTime);
    m_pLightingPlayer->drawInterpolation(m_pScreenQuad);

    m_pRainSeqPlayer->updateMultiChannelFrame(DeltaTime, vRenderChannel);
    m_pRainSeqPlayer->drawQuantization(m_pScreenQuad);
}