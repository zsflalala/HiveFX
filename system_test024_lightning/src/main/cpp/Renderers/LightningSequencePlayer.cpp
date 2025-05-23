#include "LightningSequencePlayer.h"
#include "Common.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"

using namespace hiveVG;

CLightningSequencePlayer::CLightningSequencePlayer(const std::string &vTextureRootPath, int vSequenceRows, int vSequenceCols, int vTextureCount, EPictureType::EPictureType vPictureType)
    : CSequenceFramePlayer(vTextureRootPath, vSequenceRows, vSequenceCols, vTextureCount, vPictureType)
{ }

CLightningSequencePlayer::~CLightningSequencePlayer()
{
    if(m_pStaticCloud)
        delete m_pStaticCloud;
}

void CLightningSequencePlayer::draw(CScreenQuad *vQuad)
{
    if (m_UseLifeCycle && !m_SequenceState._IsAlive)
        return ;

    if (!m_IsLoop && m_IsFinished)
    {
        m_CurrentFrame   = m_ValidFrames - 1;
        m_CurrentTexture = m_TextureCount - 1;
    }
    float RotationAngle   = m_RotationAngle * M_PI / 180.0f;
    int   CurrentFrameRow = m_CurrentFrame / m_SequenceCols;
    int   CurrentFrameCol = m_CurrentFrame % m_SequenceCols;
    float CurrentFrameU0 = static_cast<float>(CurrentFrameCol) / static_cast<float>(m_SequenceCols);
    float CurrentFrameV0 = static_cast<float>(CurrentFrameRow) / static_cast<float>(m_SequenceRows);
    float CurrentFrameU1 = static_cast<float>(CurrentFrameCol + 1) / static_cast<float>(m_SequenceCols);
    float CurrentFrameV1 = static_cast<float>(CurrentFrameRow + 1) / static_cast<float>(m_SequenceRows);
    glm::vec2 TextureUVOffset = glm::vec2(CurrentFrameU0, CurrentFrameV0);
    glm::vec2 TextureUVScale  = glm::vec2(CurrentFrameU1 - CurrentFrameU0, CurrentFrameV1 - CurrentFrameV0);

    assert(m_pSequenceShaderProgram != nullptr);
    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("rotationAngle", RotationAngle);
    m_pSequenceShaderProgram->setUniform("screenUVOffset", m_ScreenUVOffset);
    m_pSequenceShaderProgram->setUniform("screenUVScale", m_ScreenUVScale);
    m_pSequenceShaderProgram->setUniform("texUVOffset", TextureUVOffset);
    m_pSequenceShaderProgram->setUniform("texUVScale", TextureUVScale);
    m_pSequenceShaderProgram->setUniform("lightningSequenceTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    m_SeqTextures[m_CurrentTexture]->bindTexture();

    m_pSequenceShaderProgram->setUniform("cloudTexture", 1);
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
