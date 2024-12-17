#include "pch.h"
#include "SequenceFramePlayer.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "Common.h"

#define M_PI 3.14159265358979323846

using namespace hiveVG;

CSequenceFramePlayer::CSequenceFramePlayer(const std::string& vTexturePath, int vSequenceRows, int vSequenceCols)
        : m_SequenceRows(vSequenceRows), m_SequenceCols(vSequenceCols), m_TexturePath(vTexturePath)
{
    m_ValidFrames = m_SequenceRows * m_SequenceCols;
}

bool CSequenceFramePlayer::initTextureAndShaderProgram(AAssetManager* vAssetManager)
{
    m_pSequenceTexture = CTexture2D::loadTexture(vAssetManager, m_TexturePath, m_SequeceWidth, m_SequeceHeight);
    m_SequeceSingleWidth  = m_SequeceWidth  / m_SequenceCols;
    m_SequeceSingleHeight = m_SequeceHeight / m_SequenceRows;
    if (!m_pSequenceTexture)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "Error loading texture from path [%s].", m_TexturePath.c_str());
        return false;
    }
    m_pSequenceShaderProgram = CShaderProgram::createProgram(
            vAssetManager,
            "shaders/sequenceTexturePlayer.vert",
            "shaders/sequenceTexturePlayer.frag"
    );
    assert(m_pSequenceShaderProgram != nullptr);
    LOG_INFO(hiveVG::TAG_KEYWORD::SEQFRAME_PALYER_TAG, "%s load Succeed. Program Created Succeed.", m_TexturePath.c_str());
    return true;
}

void CSequenceFramePlayer::updateFrameAndUV(int vWindowWidth, int vWindowHeight, double vDt)
{
    double FrameTime = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += vDt;
    if (m_AccumFrameTime >= FrameTime)
    {
        m_AccumFrameTime -= FrameTime;
        if (m_CurrentFrame == m_ValidFrames - 1)
            m_IsFinished = true;
        else
            m_IsFinished = false;
        m_CurrentFrame  = (m_CurrentFrame + 1) % m_ValidFrames;
    }
    m_WindowSize = glm::vec2(vWindowWidth, vWindowHeight);
}

void CSequenceFramePlayer::draw(CScreenQuad *vQuad)
{
    float RotationAngle   = m_RotationAngle * M_PI / 180.0f;
    int   CurrentFrameRow = m_CurrentFrame / m_SequenceCols;
    int   CurrentFrameCol = m_CurrentFrame % m_SequenceCols;
    float CurrentFrameU0 = CurrentFrameCol / static_cast<float>(m_SequenceCols);
    float CurrentFrameV0 = CurrentFrameRow / static_cast<float>(m_SequenceRows);
    float CurrentFrameU1 = (CurrentFrameCol + 1) / static_cast<float>(m_SequenceCols);
    float CurrentFrameV1 = (CurrentFrameRow + 1) / static_cast<float>(m_SequenceRows);
    glm::vec2 TextureUVOffset = glm::vec2(CurrentFrameU0, CurrentFrameV0);
    glm::vec2 TextureUVScale  = glm::vec2(CurrentFrameU1 - CurrentFrameU0, CurrentFrameV1 - CurrentFrameV0);

    m_pSequenceShaderProgram->useProgram();
    m_pSequenceShaderProgram->setUniform("rotationAngle", RotationAngle);
    m_pSequenceShaderProgram->setUniform("screenUVOffset", m_ScreenUVOffset);
    m_pSequenceShaderProgram->setUniform("screenUVScale", m_ScreenUVScale);
    m_pSequenceShaderProgram->setUniform("orthoBounds", m_WindowSize);
    m_pSequenceShaderProgram->setUniform("texUVOffset", TextureUVOffset);
    m_pSequenceShaderProgram->setUniform("texUVScale", TextureUVScale);
    m_pSequenceShaderProgram->setUniform("sequenceTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    m_pSequenceTexture->bindTexture();
    vQuad->bindAndDraw();
}