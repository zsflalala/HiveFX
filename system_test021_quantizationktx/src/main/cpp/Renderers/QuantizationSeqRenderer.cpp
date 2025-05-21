#include "QuantizationSeqRenderer.h"
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

CQuantizationSeqRenderer::CQuantizationSeqRenderer()
{
    __initAlgorithm();
};

CQuantizationSeqRenderer::~CQuantizationSeqRenderer()
{
    if (m_pScreenQuad != nullptr)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }

    __deleteSafely(m_pSequencePlayer);
    __deleteSafely(m_pBackgroundPlayer);
}

bool CQuantizationSeqRenderer::__initAlgorithm()
{
    std::string FileName   = "configs/QuantizationConfig.json";
    CJsonReader JsonReader = CJsonReader(FileName);
    Json::Value QuantizationConfig  = JsonReader.getObject("Quantization");
    m_TexPath      =  QuantizationConfig["frames_path"].asString();
    std::string FrameType = QuantizationConfig["frames_type"].asString();
    m_TextureCount =  QuantizationConfig["frames_count"].asInt();
    m_OneTextureFrames = QuantizationConfig["one_texture_frames"].asInt();
    m_FramePerSecond   = QuantizationConfig["fps"].asFloat();
    std::string VertexShader   = QuantizationConfig["vertex_shader"].asString();
    std::string FragShader     = QuantizationConfig["fragment_shader"].asString();

    Json::Value BackGroundConfig = JsonReader.getObject("Background");
    std::string ImgPath = BackGroundConfig["frames_path"].asString();

    m_PictureType = EPictureType::FromString(FrameType);
    m_pSequencePlayer = new CSequenceFramePlayer(m_TexPath, m_TextureCount, m_OneTextureFrames, m_FramePerSecond, m_PictureType);
    m_pSequencePlayer->initTextureAndShaderProgram(VertexShader, FragShader);
    m_pBackgroundPlayer = new CSingleTexturePlayer(ImgPath);
    m_pBackgroundPlayer->initTextureAndShaderProgram();

    m_pScreenQuad   = CScreenQuad::getOrCreate();
    m_LastFrameTime = CTimeUtils::getCurrentTime();
    return true;
}

void CQuantizationSeqRenderer::renderScene()
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
    if(CStringUtils::checkWeatherCondition(m_TexPath) == 1)
    {
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }
    m_pSequencePlayer->updateQuantizationFrame(DeltaTime);
    m_pSequencePlayer->drawMultiChannelKTX(m_pScreenQuad);
}