#include "RainMultiChannelSeqRenderer.h"
#include "lodepng.h"
#include "Common.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "JsonReader.h"
#include "TimeUtils.h"
#include "StringUtils.h"
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
    Json::Value RainConfig  = JsonReader.getObject("Rain");
    std::string RainPath      =  RainConfig["frames_path"].asString();
    std::string FrameType = RainConfig["frames_type"].asString();
    int RainTextureCount  =  RainConfig["frames_count"].asInt();
    int RainOneTextureFrames = RainConfig["one_texture_frames"].asInt();
    float RainFramePerSecond = RainConfig["fps"].asFloat();
    std::string RainVertexShader = RainConfig["vertex_shader"].asString();
    std::string RainFragShader   = RainConfig["fragment_shader"].asString();

    Json::Value BackGroundConfig = JsonReader.getObject("Background");
    std::string BackImgPath = BackGroundConfig["frames_path"].asString();

    Json::Value LightingConfig = JsonReader.getObject("Lighting");
    std::string LightingPath = LightingConfig["frames_path"].asString();
    std::string LightingType = LightingConfig["frames_type"].asString();
    int    LightingFrameCount = LightingConfig["frames_count"].asInt();
    int    LightingPlayFPS    = LightingConfig["fps"].asInt();
    EPictureType::EPictureType LightingPicType = EPictureType::FromString(LightingType);

    Json::Value CloudConfig = JsonReader.getObject("Cloud");
    std::string CloudPath = CloudConfig["frames_path"].asString();
    std::string CloudType = CloudConfig["frames_type"].asString();
    int   CloudFrameCount = CloudConfig["frames_count"].asInt();
    int   CloudPlayFPS    = CloudConfig["fps"].asInt();
    EPictureType::EPictureType CloudPicType = EPictureType::FromString(CloudType);

    EPictureType::EPictureType RainPictureType = EPictureType::FromString(FrameType);
    m_pRainSeqPlayer = new CSequenceFramePlayer(RainPath, RainTextureCount, RainOneTextureFrames, RainFramePerSecond, RainPictureType);
    m_pRainSeqPlayer->initTextureAndShaderProgram(RainVertexShader, RainFragShader);

    m_pBackgroundPlayer = new CSingleTexturePlayer(BackImgPath);
    m_pBackgroundPlayer->initTextureAndShaderProgram();

    int LightRows = 1;
    int LightCols = 1;
    m_pLightingPlayer = new CSequenceFramePlayer(LightingPath, LightRows, LightCols, LightingFrameCount, LightingPicType);
    m_pLightingPlayer->initTextureAndShaderProgram();
    m_pLightingPlayer->setFrameRate(LightingPlayFPS);

    int CloudRows = 1;
    int CloudCols = 1;
    m_pCloudPlayer = new CSequenceFramePlayer(CloudPath, CloudRows, CloudCols, CloudFrameCount, CloudPicType);
    m_pCloudPlayer->initTextureAndShaderProgram();
    m_pCloudPlayer->setFrameRate(CloudPlayFPS);

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

    m_pCloudPlayer->updateFrameAndUV(DeltaTime);
    m_pLightingPlayer->updateFrameAndUV(DeltaTime);

    if (vRenderChannel == ERenderChannel::R || vRenderChannel == ERenderChannel::G)
    {
        m_pCloudPlayer->drawQuantization(m_pScreenQuad);
    }
    else if (vRenderChannel == ERenderChannel::B || vRenderChannel == ERenderChannel::A)
    {
        m_pLightingPlayer->drawQuantization(m_pScreenQuad);
    }

    m_pRainSeqPlayer->updateMultiChannelFrame(DeltaTime, vRenderChannel);
    m_pRainSeqPlayer->drawQuantization(m_pScreenQuad);
}