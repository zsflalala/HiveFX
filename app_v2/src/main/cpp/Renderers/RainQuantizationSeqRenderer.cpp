#include "RainQuantizationSeqRenderer.h"
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
#include "SplashManager.h"

using namespace hiveVG;

CRainQuantizationSeqRenderer::CRainQuantizationSeqRenderer()
{
    __initAlgorithm();
}

CRainQuantizationSeqRenderer::~CRainQuantizationSeqRenderer()
{
    if (m_pScreenQuad != nullptr)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
    __deleteSafely(m_pRainSeqPlayer);
    __deleteSafely(m_pBackgroundPlayer);
    __deleteSafely(m_pSplashPlayer);
}

bool CRainQuantizationSeqRenderer::__initAlgorithm()
{
    std::string FileName   = "configs/RainQuantizationConfig.json";
    CJsonReader JsonReader = CJsonReader(FileName);
    Json::Value QuantizationConfig  = JsonReader.getObject("Quantization");
    m_TexPath      =  QuantizationConfig["frames_path"].asString();
    std::string FrameType = QuantizationConfig["frames_type"].asString();
    m_TextureCount =  QuantizationConfig["frames_count"].asInt();
    m_OneTextureFrames = QuantizationConfig["one_texture_frames"].asInt();
    m_FramePerSecond   = QuantizationConfig["fps"].asFloat();
    std::string VertexShader = QuantizationConfig["vertex_shader"].asString();
    std::string FragShader   = QuantizationConfig["fragment_shader"].asString();

    Json::Value BackGroundConfig = JsonReader.getObject("Background");
    std::string BackImgPath = BackGroundConfig["frames_path"].asString();

    Json::Value SplashConfig = JsonReader.getObject("Splash");
    std::string SplashPath = SplashConfig["frames_path"].asString();
    std::string SplashType = SplashConfig["frames_type"].asString();
    int         SplashFrameCount = SplashConfig["frames_count"].asInt();
    std::string SplashPlayMode   = SplashConfig["play_mode"].asString();
    int         SplashPlayFPS    = SplashConfig["fps"].asInt();
    float  SplashPlayScale = SplashConfig["scale"].asFloat();
    int    SplashSeqRows = 1;
    int    SplashSeqCols = 1;
    EPictureType::EPictureType SplashPicType = EPictureType::FromString(SplashType);

    m_PictureType = EPictureType::FromString(FrameType);
    m_pRainSeqPlayer = new CSequenceFramePlayer(m_TexPath, m_TextureCount, m_OneTextureFrames, m_FramePerSecond, m_PictureType);
    m_pRainSeqPlayer->initTextureAndShaderProgram(VertexShader, FragShader);
    m_pBackgroundPlayer = new CSingleTexturePlayer(BackImgPath);
    m_pBackgroundPlayer->initTextureAndShaderProgram();

    m_pSplashPlayer = new CSequenceFramePlayer(SplashPath, SplashSeqRows, SplashSeqCols, SplashFrameCount, SplashPicType);
    m_pSplashPlayer->initTextureAndShaderProgram();
    m_pSplashPlayer->setFrameRate(SplashPlayFPS);
    m_pSplashPlayer->setScreenUVScale(glm::vec2(SplashPlayScale, SplashPlayScale));
    m_pSplashManager = std::make_unique<CSplashManager>();
    int SplashNum = 7;
    for (int i = 0; i < SplashNum; i++)
    {
        m_pSplashManager->pushBack(m_pSplashPlayer->clone());
    }
    m_pSplashManager->initSequenceState(BackImgPath, SplashPlayScale);

    m_pScreenQuad   = CScreenQuad::getOrCreate();
    m_LastFrameTime = CTimeUtils::getCurrentTime();
    return true;
}

void CRainQuantizationSeqRenderer::renderScene()
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

    m_pRainSeqPlayer->updateQuantizationFrame(DeltaTime);
    m_pRainSeqPlayer->drawQuantization(m_pScreenQuad);

    glBlendFunc(GL_ONE, GL_ONE);
    m_pSplashManager->updateFrameAndUV(DeltaTime);
    m_pSplashManager->updateSequenceState(static_cast<float>(DeltaTime));
    m_pSplashManager->draw(m_pScreenQuad);
}