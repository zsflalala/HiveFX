#include "RainQuantizationBicPicRenderer.h"
#include "Common.h"
#include "TimeUtils.h"
#include "StringUtils.h"
#include "ScreenQuad.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "JsonReader.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"
#include "SplashManager.h"

using namespace hiveVG;

CRainQuantizationBicPicRenderer::CRainQuantizationBicPicRenderer(std::string& vConfigPath) : m_ConfigPath(vConfigPath)
{
    __initAlgorithm();
}

CRainQuantizationBicPicRenderer::~CRainQuantizationBicPicRenderer()
{
    if (m_pScreenQuad)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
    __deleteSafely(m_pBackFramePlayer);
    __deleteSafely(m_pForeFramePlayer);
    __deleteSafely(m_pSplashPlayer);
}

void CRainQuantizationBicPicRenderer::__initAlgorithm()
{
    CJsonReader JsonReader = CJsonReader(m_ConfigPath);
    Json::Value SnowForeConfig = JsonReader.getObject("RainFore");
    Json::Value BackConfig     = JsonReader.getObject("Background");

    m_TexPath = SnowForeConfig["frames_path"].asString();
    m_TextureCount = SnowForeConfig["frames_count"].asInt();
    m_SeqRows    = SnowForeConfig["sequenceRows"].asInt();
    m_SeqCols    = SnowForeConfig["sequenceCols"].asInt();
    m_FramePerSecond = SnowForeConfig["fps"].asFloat();
    m_OneTextureFrames = SnowForeConfig["one_texture_frames"].asInt();
    std::string VertexShader = SnowForeConfig["vertex_shader"].asString();
    std::string FragShader   = SnowForeConfig["fragment_shader"].asString();
    m_PlayMode     = EPlayMode::FromString(SnowForeConfig["play_mode"].asString());
    m_PictureType  = EPictureType::FromString(SnowForeConfig["frames_type"].asString());
    std::string BackgroundPath = BackConfig["frames_path"].asString();

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

    m_pForeFramePlayer= new CSingleTexturePlayer(m_TexPath, m_SeqRows, m_SeqCols, m_OneTextureFrames, m_PictureType, m_FramePerSecond, m_TextureCount);
    m_pForeFramePlayer->initTextureAndShaderProgram(VertexShader,FragShader);
    m_pBackFramePlayer = new CSingleTexturePlayer(BackgroundPath);
    m_pBackFramePlayer->initTextureAndShaderProgram();

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
    m_pSplashManager->initSequenceState(BackgroundPath, SplashPlayScale);

    m_pScreenQuad   = CScreenQuad::getOrCreate();
    m_LastFrameTime = CTimeUtils::getCurrentTime();
}

void CRainQuantizationBicPicRenderer::renderScene()
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.345f,0.345f,0.345f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_pBackFramePlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();

    m_pForeFramePlayer->updateCompressedFrame(m_PlayMode, DeltaTime);
    m_pForeFramePlayer->drawCompressedFrame(m_pScreenQuad);

    glBlendFunc(GL_ONE, GL_ONE);
    m_pSplashManager->updateFrameAndUV(DeltaTime);
    m_pSplashManager->updateSequenceState(static_cast<float>(DeltaTime));
    m_pSplashManager->draw(m_pScreenQuad);
}
