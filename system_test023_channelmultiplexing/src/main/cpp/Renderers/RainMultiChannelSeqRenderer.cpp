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

CRainMultiChannelSeqRenderer::CRainMultiChannelSeqRenderer(glm::vec2 vWindowSize):m_WindowSize(vWindowSize)
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
    __deleteSafely(m_pSmallRaindropPlayer);
    __deleteSafely(m_pBigRaindropPlayer);
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
    EPictureType::EPictureType RainPictureType = EPictureType::FromString(RainFrameType);

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

    Json::Value CloudConfig = JsonReader.getObject("Cloud");
    std::string CloudPath = CloudConfig["frames_path"].asString();
    std::string CloudType = CloudConfig["frames_type"].asString();
    int   CloudFrameCount = CloudConfig["frames_count"].asInt();
    int   CloudOneTextureFrames = CloudConfig["one_texture_frames"].asInt();
    float CloudPlayFPS    = CloudConfig["fps"].asFloat();
    std::string CloudVertexShader = CloudConfig["vertex_shader"].asString();
    std::string CloudFragShader   = CloudConfig["fragment_shader"].asString();
    EPictureType::EPictureType CloudPicType = EPictureType::FromString(CloudType);

    Json::Value SmallRaindropConfig = JsonReader.getObject("SmallRaindrop");
    std::string SmallRaindropFramePath = SmallRaindropConfig["frames_path"].asString();
    std::string SmallRaindropFrameType = SmallRaindropConfig["frames_type"].asString();
    int         SmallRaindropFrameCount = SmallRaindropConfig["frames_count"].asInt();
    int         SmallRaindropTextureFrames = SmallRaindropConfig["one_texture_frames"].asInt();
    float       SmallRaindropPlayFPS    = SmallRaindropConfig["fps"].asFloat();
    std::string SmallRaindropVertexShader = SmallRaindropConfig["vertex_shader"].asString();
    std::string SmallRaindropFragShader   = SmallRaindropConfig["fragment_shader"].asString();
    EPictureType::EPictureType SmallRaindropPicType = EPictureType::FromString(SmallRaindropFrameType);

    Json::Value BigRaindropConfig = JsonReader.getObject("BigRaindrop");
    std::string BigRaindropFramePath = BigRaindropConfig["frames_path"].asString();
    std::string BigRaindropFrameType = BigRaindropConfig["frames_type"].asString();
    int         BigRaindropFrameCount = BigRaindropConfig["frames_count"].asInt();
    int         BigRaindropTextureFrames = BigRaindropConfig["one_texture_frames"].asInt();
    float       BigRaindropPlayFPS    = BigRaindropConfig["fps"].asFloat();
    std::string BigRaindropVertexShader = BigRaindropConfig["vertex_shader"].asString();
    std::string BigRaindropFragShader   = BigRaindropConfig["fragment_shader"].asString();
    EPictureType::EPictureType BigRaindropPicType = EPictureType::FromString(BigRaindropFrameType);

    m_pRainSeqPlayer = new CSequenceFramePlayer(RainPath, RainTextureCount, RainOneTextureFrames, RainFramePerSecond, RainPictureType);
    m_pRainSeqPlayer->initTextureAndShaderProgram(RainVertexShader, RainFragShader);

    m_pBackgroundPlayer = new CSingleTexturePlayer(BackImgPath, BackPicType);
    m_pBackgroundPlayer->initTextureAndShaderProgram(BackVertexShader, BackFragShader);

    m_pLightingPlayer = new CSequenceFramePlayer(LightingPath, LightingFrameCount, LightingOneTextureFrames, LightingPlayFPS, LightingPicType);
    m_pLightingPlayer->initTextureAndShaderProgram(LightingVertexShader, LightingFragShader);
    m_pLightingPlayer->setWindowSize(m_WindowSize);
    m_pLightingPlayer->setRatioUniform();

    m_pCloudPlayer = new CSequenceFramePlayer(CloudPath, CloudFrameCount, CloudOneTextureFrames, CloudPlayFPS, CloudPicType);
    m_pCloudPlayer->initTextureAndShaderProgram(CloudVertexShader, CloudFragShader);
    m_pCloudPlayer->setWindowSize(m_WindowSize);
    m_pCloudPlayer->setRatioUniform();

    m_pSmallRaindropPlayer = new CSequenceFramePlayer(SmallRaindropFramePath, SmallRaindropFrameCount, SmallRaindropTextureFrames, SmallRaindropPlayFPS, SmallRaindropPicType);
    m_pSmallRaindropPlayer->initTextureAndShaderProgram(SmallRaindropVertexShader, SmallRaindropFragShader);

    m_pBigRaindropPlayer = new CSequenceFramePlayer(BigRaindropFramePath, BigRaindropFrameCount, BigRaindropTextureFrames, BigRaindropPlayFPS, BigRaindropPicType);
    m_pBigRaindropPlayer->initTextureAndShaderProgram(BigRaindropVertexShader, BigRaindropFragShader);

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

    m_pCloudPlayer->updateLerpQuantFrame(DeltaTime);
    m_pLightingPlayer->updateLerpQuantFrame(DeltaTime);
    if (vRenderChannel == ERenderChannel::R || vRenderChannel == ERenderChannel::G)
    {
        m_pCloudPlayer->drawInterpolation(m_pScreenQuad);
    }
    else if (vRenderChannel == ERenderChannel::B || vRenderChannel == ERenderChannel::A)
    {
        m_pLightingPlayer->drawInterpolation(m_pScreenQuad);
    }

    m_pRainSeqPlayer->updateMultiChannelFrame(DeltaTime, vRenderChannel);
    m_pRainSeqPlayer->drawMultiChannelKTX(m_pScreenQuad);

    m_pSmallRaindropPlayer->updateMultiChannelFrame(DeltaTime, vRenderChannel);
    m_pBigRaindropPlayer->updateMultiChannelFrame(DeltaTime, vRenderChannel);

    if (vRenderChannel == ERenderChannel::R)
    {
        m_pSmallRaindropPlayer->setFrameRate(13);
        m_pSmallRaindropPlayer->drawMultiChannelKTX(m_pScreenQuad);
    }
    else if (vRenderChannel == ERenderChannel::G)
    {
        m_pSmallRaindropPlayer->setFrameRate(18);
        m_pSmallRaindropPlayer->drawMultiChannelKTX(m_pScreenQuad);
    }
    else if (vRenderChannel == ERenderChannel::B)
    {
        m_pBigRaindropPlayer->setFrameRate(10);
        m_pBigRaindropPlayer->drawMultiChannelKTX(m_pScreenQuad);
    }
    else if (vRenderChannel == ERenderChannel::A)
    {
        m_pBigRaindropPlayer->setFrameRate(20);
        m_pBigRaindropPlayer->drawMultiChannelKTX(m_pScreenQuad);
    }
}