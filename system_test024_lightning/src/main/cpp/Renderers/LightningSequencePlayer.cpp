#include "LightningSequencePlayer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Common.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"

using namespace hiveVG;

CLightningSequencePlayer::CLightningSequencePlayer(const std::string &vTextureRootPath, int vSequenceRows, int vSequenceCols, int vTextureCount, EPictureType::EPictureType vPictureType)
    : CSequenceFramePlayer(vTextureRootPath, vSequenceRows, vSequenceCols, vTextureCount, vPictureType)
{}

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

void CLightningSequencePlayer::draw(CScreenQuad *vQuad)
{
    float RotationAngle = glm::radians(static_cast<float>(m_RotationAngle));
    float uFlashProgress = (float)m_CurrentTexture / (float)(m_TextureCount - 1);
    uFlashProgress = glm::clamp(uFlashProgress, 0.0f, 1.0f);

    assert(m_pSequenceShaderProgram != nullptr);
    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("uFlashProgress", uFlashProgress);
    m_pSequenceShaderProgram->setUniform("uFlashColor", glm::vec3(1.0f));
    m_pSequenceShaderProgram->setUniform("uFlashAlpha", 0.3f);
    m_pSequenceShaderProgram->setUniform("rotationAngle", RotationAngle);
    m_pSequenceShaderProgram->setUniform("lightningInFront", m_LightningInFront);
    m_pSequenceShaderProgram->setUniform("screenUVOffset", m_ScreenUVOffset);
    m_pSequenceShaderProgram->setUniform("screenUVScale", m_ScreenUVScale);
    m_pSequenceShaderProgram->setUniform("lightningSequenceTexture", 0);
    m_pSequenceShaderProgram->setUniform("cloudTexture", 1);
    glActiveTexture(GL_TEXTURE0);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    glActiveTexture(GL_TEXTURE1);
    m_pStaticCloud->bindTexture();
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
    CSequenceFramePlayer::initTextureAndShaderProgram("shaders/lightning.vert", "shaders/lightning.frag");
    return true;
}

void CLightningSequencePlayer::__resetPlayback()
{

    m_IsFinished = false;
    m_IsWaiting  = false;
    m_WaitTime   = 0.0;
    m_CurrentFrame   = 0;
    m_CurrentTexture = 0;

    __randomizeLightningParameters();
}

void CLightningSequencePlayer::__randomizeLightningParameters()
{
    m_ScreenUVScale.x = m_ScaleDist(m_Rng);
    m_ScreenUVScale.y = m_ScaleDist(m_Rng);

    float maxOffsetX = std::max(0.0f, 1.0f - m_ScreenUVScale.x);
    float maxOffsetY = std::max(0.0f, 0.5f - m_ScreenUVScale.y);

    std::uniform_real_distribution<float> OffsetXDist(0.0f, maxOffsetX);
    std::uniform_real_distribution<float> OffsetYDist(0.0f, maxOffsetY);

    m_ScreenUVOffset.x = OffsetXDist(m_Rng);
    m_ScreenUVOffset.y = OffsetYDist(m_Rng);

    m_LightningInFront = m_BoolDist(m_Rng) == 1;
}