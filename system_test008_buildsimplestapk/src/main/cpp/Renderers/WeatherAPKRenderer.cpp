#include "WeatherAPKRenderer.h"
#include "Common.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "JsonReader.h"
#include "TimeUtils.h"

using namespace hiveVG;

CWeatherAPKRenderer::CWeatherAPKRenderer()
{
}

CWeatherAPKRenderer::~CWeatherAPKRenderer()
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
    if (m_pBackgroundTex != nullptr)
    {
        delete m_pBackgroundTex;
        m_pBackgroundTex = nullptr;
    }
    for (auto & m_SeqTexture : m_SeqTextures)
    {
        delete m_SeqTexture;
        m_SeqTexture = nullptr;
    }
}

bool CWeatherAPKRenderer::initTextureAndShaderProgram()
{
    std::string FileName   = "configs/WeatherAppConfig.json";
    CJsonReader JsonReader = CJsonReader(FileName);
    Json::Value WeatherConfig  = JsonReader.getObject("Weather");

    std::string VertexShader   = WeatherConfig["vertex_shader"].asString();
    std::string FragShader     = WeatherConfig["fragment_shader"].asString();
    std::string BackgroundPath = WeatherConfig["background_img"].asString();
    m_BackTexPath  = WeatherConfig["back_path"].asString();
    m_ForeTexPath  = WeatherConfig["fore_path"].asString();
    m_TextureCount = WeatherConfig["texture_count"].asInt();

    if (!m_ForeTexPath.empty() && m_ForeTexPath.back() != '/')
        m_ForeTexPath += '/';
    if (!m_BackTexPath.empty() && m_BackTexPath.back() != '/')
        m_BackTexPath += '/';

    m_pBackgroundTex = CTexture2D::loadTexture(BackgroundPath);
    if (!m_pBackgroundTex)
    {
        LOG_ERROR(TAG_KEYWORD::WEATHER_TAG, "Error loading background texture from path [{%s}].", BackgroundPath.c_str());
        return false;
    }

    for (int i = 0; i < m_PreloadTexture; i++)
    {
        std::string ForeTexPath = m_ForeTexPath + "frame_" + std::string(3 - std::to_string(i + 1).length(), '0') + std::to_string(i + 1) + ".png";
        std::string BackTexPath = m_BackTexPath + "frame_" + std::string(3 - std::to_string(i + 1).length(), '0') + std::to_string(i + 1) + ".png";

        CTexture2D* pBackSeqTex = CTexture2D::loadTexture(BackTexPath);
        m_SeqTextures.push_back(pBackSeqTex);
        CTexture2D* pForeSeqTex = CTexture2D::loadTexture(ForeTexPath);
        m_SeqTextures.push_back(pForeSeqTex);
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

void CWeatherAPKRenderer::renderScene()
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
        m_CurrentTexture += 2;

        if (m_SeqTextures.size() == m_CurrentTexture)
            m_CurrentTexture = 0;
    }

    if(m_PreloadTexture < m_TextureCount)
    {
        m_PreloadTexture++;
        std::string ForeTexPath = m_ForeTexPath + "frame_" +
                                  std::string(3 - std::to_string(m_PreloadTexture).length(),
                                              '0') + std::to_string(m_PreloadTexture) + ".png";
        std::string BackTexPath = m_BackTexPath + "frame_" +
                                  std::string(3 - std::to_string(m_PreloadTexture).length(),
                                              '0') + std::to_string(m_PreloadTexture) + ".png";
        CTexture2D* pBackSeqTex = CTexture2D::loadTexture(BackTexPath);
        m_SeqTextures.push_back(pBackSeqTex);
        CTexture2D* pForeSeqTex = CTexture2D::loadTexture(ForeTexPath);
        m_SeqTextures.push_back(pForeSeqTex);
    }

    m_pSequenceShaderProgram->useProgram();
    glActiveTexture(GL_TEXTURE0);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    m_pSequenceShaderProgram->setUniform("backgroundTex", 0);

    glActiveTexture(GL_TEXTURE1);
    m_pBackgroundTex->bindTexture();
    m_pSequenceShaderProgram->setUniform("middleTex", 1);

    glActiveTexture(GL_TEXTURE2);
    m_SeqTextures[m_CurrentTexture + 1]->bindTexture();
    m_pSequenceShaderProgram->setUniform("foregroundTex", 2);
    m_pScreenQuad->bindAndDraw();
}