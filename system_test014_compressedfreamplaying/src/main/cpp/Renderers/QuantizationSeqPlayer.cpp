#include "QuantizationSeqPlayer.h"
#include "Common.h"
#include "TimeUtils.h"
#include "ScreenQuad.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "JsonReader.h"
#include "SingleTexturePlayer.h"

using namespace hiveVG;

CQuantizationSeqPlayer::CQuantizationSeqPlayer()
{
    __initAlgorithm();
}

CQuantizationSeqPlayer::~CQuantizationSeqPlayer()
{
    if (m_pScreenQuad)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
    for (auto& m_SeqTexture : m_SeqTextures)
    {
        __deleteSafely(m_SeqTexture);
    }
    __deleteSafely(m_pSequenceShaderProgram);
    __deleteSafely(m_pBackFramePlayer);
}

void CQuantizationSeqPlayer::__initAlgorithm()
{
    std::string FileName = "configs/QuantizationBigPicConfig.json";
    CJsonReader JsonReader = CJsonReader(FileName);
    Json::Value SnowForeConfig = JsonReader.getObject("SmallSnowFore");
    Json::Value BackConfig     = JsonReader.getObject("Background");

    m_TexPath = SnowForeConfig["frames_path"].asString();
    m_TextureCount = SnowForeConfig["frames_count"].asInt();
    m_SeqRows    = SnowForeConfig["sequenceRows"].asInt();
    m_SeqCols    = SnowForeConfig["sequenceCols"].asInt();
    m_ValidFrames = m_SeqRows * m_SeqCols;
    m_FramePerSecond = SnowForeConfig["fps"].asFloat();
    m_OneTextureFrames = SnowForeConfig["one_texture_frames"].asInt();
    std::string VertexShader = SnowForeConfig["vertex_shader"].asString();
    std::string FragShader   = SnowForeConfig["fragment_shader"].asString();
    m_PlayMode     = EPlayMode::FromString(SnowForeConfig["play_mode"].asString());
    m_PictureType  = EPictureType::FromString(SnowForeConfig["frames_type"].asString());
    std::string BackgroundPath = BackConfig["frames_path"].asString();

    if (!m_TexPath.empty() && m_TexPath.back() != '/')
        m_TexPath += '/';

    for (int i = 0; i < m_TextureCount; i++)
    {
        std::string TexPngPath = m_TexPath + "frame_" + std::string(3 - std::to_string(i + 1).length(), '0') + std::to_string(i + 1) + ".png";
        CTexture2D* pBackSeqTex = CTexture2D::loadTexture(TexPngPath);
        m_SeqTextures.push_back(pBackSeqTex);
    }
    m_pSequenceShaderProgram = CShaderProgram::createProgram(VertexShader, FragShader);
    assert(m_pSequenceShaderProgram != nullptr);
    m_pBackFramePlayer = new CSingleTexturePlayer(BackgroundPath);
    m_pBackFramePlayer->initTextureAndShaderProgram();
    m_pScreenQuad   = CScreenQuad::getOrCreate();
    m_LastFrameTime = CTimeUtils::getCurrentTime();
}

void CQuantizationSeqPlayer::renderScene(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.345f,0.345f,0.345f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    double FrameTime = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += DeltaTime;
    if (m_AccumFrameTime >= FrameTime)
    {
        m_AccumFrameTime -= FrameTime;

        if (m_PlayMode == EPlayMode::DEPTH)
        {
            m_CurrentChannel = (m_CurrentChannel + 1) % m_OneTextureFrames;
            if (m_CurrentChannel == 0)
            {
                m_CurrentFrame = (m_CurrentFrame + 1) % m_ValidFrames;
                if (m_CurrentFrame == 0)
                    m_CurrentTexture = (m_CurrentTexture + 1) % static_cast<int>(m_SeqTextures.size());
            }
        }
        else if (m_PlayMode == EPlayMode::CHANNEL)
        {
            m_CurrentFrame = (m_CurrentFrame + 1) % m_ValidFrames;
            if (m_CurrentFrame == 0)
            {
                m_CurrentChannel = (m_CurrentChannel + 1) % m_OneTextureFrames;
                if (m_CurrentChannel == 0)
                    m_CurrentTexture = (m_CurrentTexture + 1) % static_cast<int>(m_SeqTextures.size());
            }
        }
        LOG_INFO(TAG_KEYWORD::RENDERER_TAG, "Frame: %d, SeqTexture: %d, Current Channel: %d", m_CurrentFrame, m_CurrentTexture, m_CurrentChannel);
    }

    int   CurrentFrameRow = m_CurrentFrame / m_SeqCols;
    int   CurrentFrameCol = m_CurrentFrame % m_SeqCols;
    float CurrentFrameU0 = static_cast<float>(CurrentFrameCol) / static_cast<float>(m_SeqCols);
    float CurrentFrameV0 = static_cast<float>(CurrentFrameRow) / static_cast<float>(m_SeqRows);
    float CurrentFrameU1 = static_cast<float>(CurrentFrameCol + 1) / static_cast<float>(m_SeqCols);
    float CurrentFrameV1 = static_cast<float>(CurrentFrameRow + 1) / static_cast<float>(m_SeqRows);
    glm::vec2 TextureUVOffset = glm::vec2(CurrentFrameU0, CurrentFrameV0);
    glm::vec2 TextureUVScale  = glm::vec2(CurrentFrameU1 - CurrentFrameU0, CurrentFrameV1 - CurrentFrameV0);

    m_pBackFramePlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();
    m_pSequenceShaderProgram->useProgram();
    glActiveTexture(GL_TEXTURE0);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    m_pSequenceShaderProgram->setUniform("texUVOffset", TextureUVOffset);
    m_pSequenceShaderProgram->setUniform("texUVScale", TextureUVScale);
    m_pSequenceShaderProgram->setUniform("indexTexture", 0);
    m_pSequenceShaderProgram->setUniform("channelIndex", m_CurrentChannel);
    m_pScreenQuad->bindAndDraw();
}