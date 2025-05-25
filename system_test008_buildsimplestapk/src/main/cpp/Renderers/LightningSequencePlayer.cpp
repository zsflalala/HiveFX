#include "LightningSequencePlayer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Common.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "JsonReader.h"

using namespace hiveVG;

CLightningSequencePlayer::CLightningSequencePlayer(const std::string &vTextureRootPath, int vSequenceRows, int vSequenceCols, int vTextureCount, EPictureType::EPictureType vPictureType)
    : CSequenceFramePlayer(vTextureRootPath, vSequenceRows, vSequenceCols, vTextureCount, vPictureType)
{}

CLightningSequencePlayer::CLightningSequencePlayer(const std::string& vTextureRootPath, int vTextureCount, int vOneTextureFrames, float vFrameSeconds, EPictureType::EPictureType vPictureType)
        : CSequenceFramePlayer(vTextureRootPath, vTextureCount, vOneTextureFrames, vFrameSeconds, vPictureType)
{

    std::string FileName   = "configs/RainMultiChannelSeqConfig.json";
    CJsonReader JsonReader = CJsonReader(FileName);
    Json::Value CloudConfig = JsonReader.getObject("Cloud");
    std::string CloudPath = CloudConfig["frames_path"].asString();
    std::string CloudType = CloudConfig["frames_type"].asString();
    int   CloudFrameCount = CloudConfig["frames_count"].asInt();
    int   CloudOneTextureFrames = CloudConfig["one_texture_frames"].asInt();
    float CloudPlayFPS    = CloudConfig["fps"].asFloat();
    std::string CloudVertexShader = CloudConfig["vertex_shader"].asString();
    std::string CloudFragShader   = CloudConfig["fragment_shader"].asString();
    EPictureType::EPictureType CloudPicType = EPictureType::FromString(CloudType);

    m_pCloudPlayer = new CSequenceFramePlayer(CloudPath, CloudFrameCount, CloudOneTextureFrames, CloudPlayFPS, CloudPicType);
    m_pCloudPlayer->initTextureAndShaderProgram(CloudVertexShader, CloudFragShader);
    m_SeqCloudTextures = m_pCloudPlayer->getTextures();
    m_CloudFPS = CloudPlayFPS;
    m_OneCloudTexFrames = CloudOneTextureFrames;
    m_CloudSingleTexWidth = m_pCloudPlayer->getSingleTextureWidth();
    m_CloudSingleTexHeight = m_pCloudPlayer->getSingleTextureHeight();
}

CLightningSequencePlayer::~CLightningSequencePlayer()
{
    if (m_pStaticCloud)
    {
        delete m_pStaticCloud;
        m_pStaticCloud = nullptr;
    }
}

void CLightningSequencePlayer::updateFrameAndUV(double vDeltaTime)
{
    if (m_IsWaiting)
    {
        m_WaitTime += vDeltaTime;
        if (m_WaitTime >= m_TargetWaitTime)
        {
            __resetPlayback();
        }
        return;
    }

    if (m_FramePerSecond <= 0.0) return;
    double FrameDuration = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += vDeltaTime;

    if (m_AccumFrameTime >= FrameDuration)
    {
        m_AccumFrameTime = 0.0;

        if (m_CurrentFrame == m_ValidFrames - 1)
        {
            if (m_CurrentTexture == m_TextureCount - 1)
            {
                m_IsFinished = true;
                m_IsWaiting  = true;
                m_TargetWaitTime = m_WaitDist(m_Rng);
                return;
            }
            else
            {
                m_CurrentTexture++;
            }
        }
        m_CurrentFrame = (m_CurrentFrame + 1) % m_ValidFrames;
    }
}

void CLightningSequencePlayer::updateQuantizationFrame(double vDeltaTime)
{
    if (m_IsWaiting)
    {
        m_WaitTime += vDeltaTime;
        if (m_WaitTime >= m_TargetWaitTime)
        {
            __resetPlayback();
        }
        return;
    }

    if (m_FramePerSecond <= 0.0) return;

    double FrameTime = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += vDeltaTime;

    if (m_AccumFrameTime >= FrameTime)
    {
        m_AccumFrameTime -= FrameTime;

        m_CurrentChannel = (m_CurrentChannel + 1) % m_OneTextureFrames;

        if (m_CurrentChannel == 0)
        {
            m_CurrentTexture++;

            if (m_CurrentTexture >= m_SeqTextures.size() / 2)
            {
                m_CurrentTexture = 0; // 可选：是否重置为 0 看业务逻辑
                m_IsFinished = true;
                m_IsWaiting = true;
                m_TargetWaitTime = m_WaitDist(m_Rng);
                return;
            }
        }
    }
}

void CLightningSequencePlayer::updateCloudSequence(double vDeltaTime)
{
    double FrameTime = 1.0 / m_CloudFPS;
    m_AccumCloudTime += vDeltaTime;
    if (m_AccumCloudTime >= FrameTime)
    {
        m_AccumCloudTime = 0.0;

        m_CurrentCloudChannel++;
        if (m_CurrentCloudChannel >= m_OneCloudTexFrames - 1)
        {
            m_NextCloudTexture++;
            if (m_NextCloudTexture >= m_SeqCloudTextures.size())
            {
                m_NextCloudTexture = 0;
            }
        }
        else if (m_CurrentCloudChannel >= m_OneCloudTexFrames)
        {
            m_CurrentCloudTexture = m_NextCloudTexture;
            m_CurrentCloudChannel = 0;
        }
    }

    m_CloudInterpFactor = m_AccumCloudTime / FrameTime;
}
void CLightningSequencePlayer::draw(CScreenQuad *vQuad)
{
    float RotationAngle = glm::radians(static_cast<float>(m_RotationAngle));
    float FlashProgress = (float)m_CurrentTexture / (float)(m_TextureCount - 1);
    FlashProgress = glm::clamp(FlashProgress, 0.0f, 1.0f);
    int bindTextureIndex = m_CurrentTexture + (m_LightningInFront ? 0 : 8);

    assert(m_pSequenceShaderProgram != nullptr);

    m_pSequenceShaderProgram->setUniform("flashProgress", FlashProgress);
    m_pSequenceShaderProgram->setUniform("flashColor", glm::vec3(1.0f));  // 你可根据需求调整
    m_pSequenceShaderProgram->setUniform("flashAlpha", 0.3f);
    m_pSequenceShaderProgram->setUniform("lightningInFront", m_LightningInFront ? 1 : 0);
    m_pSequenceShaderProgram->setUniform("channelIndex", m_CurrentChannel);

    m_pSequenceShaderProgram->setUniform("Factor", m_CloudInterpFactor);
    m_pSequenceShaderProgram->setUniform("Displacement", 0.01f);  // 你需要定义这个值
    m_pSequenceShaderProgram->setUniform("CurrentChannel", m_CurrentCloudChannel);
    m_pSequenceShaderProgram->setUniform("TexelSize", glm::vec2(
            1.0f / m_CloudSingleTexWidth, 1.0f / m_CloudSingleTexHeight
    ));

    glActiveTexture(GL_TEXTURE0);
    m_SeqCloudTextures[m_CurrentCloudTexture]->bindTexture();
    m_pSequenceShaderProgram->setUniform("CurrentTexture", 0);

    glActiveTexture(GL_TEXTURE1);
    m_SeqCloudTextures[m_NextCloudTexture]->bindTexture();
    m_pSequenceShaderProgram->setUniform("NextTexture", 1);

    glActiveTexture(GL_TEXTURE2);
    m_SeqTextures[bindTextureIndex]->bindTexture();
    m_pSequenceShaderProgram->setUniform("lightningSequenceTexture", 2);

    vQuad->bindAndDraw();
}

void CLightningSequencePlayer::initBackground(const std::string &vTexturePath)
{
    int Width, Height;
    auto PictureType = EPictureType::EPictureType::PNG;
    m_pStaticCloud = CTexture2D::loadTexture(vTexturePath, Width, Height, PictureType);
    if (!m_pStaticCloud)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG,
                  "Error loading texture from path [%s].", vTexturePath.c_str());
        return;
    }
}

bool CLightningSequencePlayer::initTextureAndShaderProgram()
{
    return CSequenceFramePlayer::initTextureAndShaderProgram("shaders/lightning.vert", "shaders/lightning.frag");
}

void CLightningSequencePlayer::__resetPlayback()
{

    m_IsFinished = false;
    m_IsWaiting  = false;
    m_WaitTime   = 0.0;
    m_CurrentFrame   = 0;
    m_CurrentTexture = 0;
    m_CurrentChannel = 0;

    __randomizeLightningParameters();
}

void CLightningSequencePlayer::__randomizeLightningParameters()
{
    m_ScreenUVScale.x = m_ScaleDist(m_Rng);
    m_ScreenUVScale.y = m_ScaleDist(m_Rng);

    float maxOffsetX = std::max(0.0f, 1.0f - m_ScreenUVScale.x);
    float maxOffsetY = std::max(0.0f, 0.8f - m_ScreenUVScale.y);

    std::uniform_real_distribution<float> OffsetXDist(0.0f, maxOffsetX);
    std::uniform_real_distribution<float> OffsetYDist(0.0f, maxOffsetY);

    m_ScreenUVOffset.x = OffsetXDist(m_Rng);
    m_ScreenUVOffset.y = OffsetYDist(m_Rng);

    m_LightningInFront = m_BoolDist(m_Rng) == 1;
}