#include "QuantizationBicPicRenderer.h"
#include "Common.h"
#include "TimeUtils.h"
#include "StringUtils.h"
#include "ScreenQuad.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "JsonReader.h"
#include "SingleTexturePlayer.h"

using namespace hiveVG;

CQuantizationBicPicRenderer::CQuantizationBicPicRenderer()
{
    __initAlgorithm();
}

CQuantizationBicPicRenderer::~CQuantizationBicPicRenderer()
{
    if (m_pScreenQuad)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
    __deleteSafely(m_pBackFramePlayer);
}

void CQuantizationBicPicRenderer::__initAlgorithm()
{
    std::string FileName = "configs/QuantizationBigPicConfig.json";
    CJsonReader JsonReader = CJsonReader(FileName);
    Json::Value SnowForeConfig = JsonReader.getObject("SmallSnowFore");
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

    m_pForeFramePlayer= new CSingleTexturePlayer(m_TexPath, m_SeqRows, m_SeqCols, m_OneTextureFrames, m_PictureType, m_FramePerSecond, m_TextureCount);
    m_pForeFramePlayer->initTextureAndShaderProgram(VertexShader,FragShader);
    m_pBackFramePlayer = new CSingleTexturePlayer(BackgroundPath);
    m_pBackFramePlayer->initTextureAndShaderProgram();
    m_pScreenQuad   = CScreenQuad::getOrCreate();
    m_LastFrameTime = CTimeUtils::getCurrentTime();
}

void CQuantizationBicPicRenderer::renderScene()
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
    if(CStringUtils::checkWeatherCondition(m_TexPath) == 1)
    {
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }
    m_pForeFramePlayer->updateCompressedFrame(m_PlayMode, DeltaTime);
    m_pForeFramePlayer->drawCompressedFrame(m_pScreenQuad);
}
