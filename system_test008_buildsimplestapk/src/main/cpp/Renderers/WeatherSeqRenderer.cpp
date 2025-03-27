#include "WeatherSeqRenderer.h"
#include "Common.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "JsonReader.h"
#include "TimeUtils.h"

using namespace hiveVG;

CWeatherSeqRenderer::CWeatherSeqRenderer()
{
}

CWeatherSeqRenderer::~CWeatherSeqRenderer()
{
    if (m_pScreenQuad != nullptr)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
    if (m_pSequenceShaderProgram != nullptr)
    {
        delete m_pSequenceShaderProgram;
        m_pSequenceShaderProgram = nullptr;
    }
    for (auto & m_SeqTexture : m_SeqTextures)
    {
        delete m_SeqTexture;
        m_SeqTexture = nullptr;
    }
}

bool CWeatherSeqRenderer::initTextureAndShaderProgram()
{
    std::string FileName   = "configs/WeatherSeqConfig.json";
    CJsonReader JsonReader = CJsonReader(FileName);
    Json::Value WeatherConfig  = JsonReader.getObject("Weather");

    std::string VertexShader   = WeatherConfig["vertex_shader"].asString();
    std::string FragShader     = WeatherConfig["fragment_shader"].asString();
    m_TexPath      = WeatherConfig["texture_path"].asString();
    m_TextureCount = WeatherConfig["texture_count"].asInt();

    if (!m_TexPath.empty() && m_TexPath.back() != '/')
        m_TexPath += '/';

    for (int i = 0; i < m_PreloadTexture; i++)
    {
        std::string TexPngPath = m_TexPath + "frame_" + std::string(3 - std::to_string(i + 1).length(), '0') + std::to_string(i + 1) + ".png";
        CTexture2D* pBackSeqTex = CTexture2D::loadTexture(TexPngPath);
        m_SeqTextures.push_back(pBackSeqTex);
    }

    m_pSequenceShaderProgram = CShaderProgram::createProgram(
            VertexShader,
            FragShader
    );
    assert(m_pSequenceShaderProgram != nullptr);
    m_pScreenQuad = CScreenQuad::getOrCreate();
    m_LastFrameTime = CTimeUtils::getCurrentTime();
    return true;
}

void CWeatherSeqRenderer::renderScene()
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.345f,0.345f,0.345f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    double FrameTime = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += DeltaTime;
    if (m_AccumFrameTime >= FrameTime)
    {
        m_AccumFrameTime -= FrameTime;
        m_CurrentTexture++;
        if (m_SeqTextures.size() == m_CurrentTexture)
            m_CurrentTexture = 0;
    }

    if(m_PreloadTexture < m_TextureCount)
    {
        m_PreloadTexture++;
        std::string TexPngPath = m_TexPath + "frame_" + std::string(3 - std::to_string(m_PreloadTexture).length(), '0') + std::to_string(m_PreloadTexture) + ".png";
        CTexture2D* pBackSeqTex = CTexture2D::loadTexture(TexPngPath);
        m_SeqTextures.push_back(pBackSeqTex);
    }

    m_pSequenceShaderProgram->useProgram();
    glActiveTexture(GL_TEXTURE0);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    m_pSequenceShaderProgram->setUniform("sequenceTexture", 0);
    m_pScreenQuad->bindAndDraw();
}