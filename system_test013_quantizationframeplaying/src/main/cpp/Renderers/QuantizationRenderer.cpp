#include "QuantizationRenderer.h"
#include "lodepng.h"
#include "Common.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "JsonReader.h"
#include "TimeUtils.h"
#include "SingleTexturePlayer.h"

using namespace hiveVG;

CQuantizationRenderer::CQuantizationRenderer() = default;

CQuantizationRenderer::~CQuantizationRenderer()
{
    if (m_pScreenQuad != nullptr)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
    for (auto& m_SeqTexture : m_SeqTextures)
    {
        __deleteSafely(m_SeqTexture);
    }
    __deleteSafely(m_pSequenceShaderProgram);
    __deleteSafely(m_pBackgroundPlayer);
}

bool CQuantizationRenderer::initTextureAndShaderProgram()
{
    std::string FileName   = "configs/QuantizationConfig.json";
    CJsonReader JsonReader = CJsonReader(FileName);
    Json::Value QuantizationConfig  = JsonReader.getObject("Quantization");

    std::string VertexShader   = QuantizationConfig["vertex_shader"].asString();
    std::string FragShader     = QuantizationConfig["fragment_shader"].asString();
    std::string BackgroundPath = QuantizationConfig["background_img"].asString();
    m_TexPath      = QuantizationConfig["back_path"].asString();
    m_TextureCount = QuantizationConfig["texture_count"].asInt();
    m_OneTextureFrames = QuantizationConfig["one_texture_frames"].asInt();
    m_FramePerSecond   = QuantizationConfig["fps"].asFloat();

    m_pBackgroundPlayer = new CSingleTexturePlayer(BackgroundPath);
    m_pBackgroundPlayer->initTextureAndShaderProgram();

    if (!m_TexPath.empty() && m_TexPath.back() != '/')
        m_TexPath += '/';

    for (int i = 0; i < m_PreloadTexture; i++)
    {
        std::string TexPngPath = m_TexPath + "frame_" + std::string(3 - std::to_string(i + 1).length(), '0') + std::to_string(i + 1) + ".png";
        CTexture2D* pBackSeqTex = CTexture2D::loadTexture(TexPngPath);
        m_SeqTextures.push_back(pBackSeqTex);
    }

    m_pSequenceShaderProgram = CShaderProgram::createProgram(VertexShader,FragShader);
    assert(m_pSequenceShaderProgram != nullptr);
    m_pScreenQuad   = CScreenQuad::getOrCreate();
    m_LastFrameTime = CTimeUtils::getCurrentTime();
    return true;
}

void CQuantizationRenderer::renderScene()
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.345f,0.345f,0.345f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    double FrameTime = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += DeltaTime;
    if (m_AccumFrameTime >= FrameTime)
    {
        m_AccumFrameTime -= FrameTime;
        m_CurrentChannel = (m_CurrentChannel + 1) % m_OneTextureFrames;
        if (m_CurrentChannel == 0)
        {
            m_CurrentTexture++;
            if (m_SeqTextures.size() - 1 == m_CurrentTexture)
                m_CurrentTexture = 0;
        }
        LOG_INFO(TAG_KEYWORD::RENDERER_TAG, "SeqTexture: %d, Current Channel: %d" , m_CurrentTexture, m_CurrentChannel);
    }

    if(m_PreloadTexture < m_TextureCount)
    {
        m_PreloadTexture++;
        std::string TexPngPath = m_TexPath + "frame_" + std::string(3 - std::to_string(m_PreloadTexture).length(), '0') + std::to_string(m_PreloadTexture) + ".png";
        CTexture2D* pBackSeqTex = CTexture2D::loadTexture(TexPngPath);
        m_SeqTextures.push_back(pBackSeqTex);
    }

    m_pBackgroundPlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();
    m_pSequenceShaderProgram->useProgram();
    glActiveTexture(GL_TEXTURE0);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    m_pSequenceShaderProgram->setUniform("indexTexture", 0);
    m_pSequenceShaderProgram->setUniform("channelIndex", m_CurrentChannel);
    m_pScreenQuad->bindAndDraw();
}