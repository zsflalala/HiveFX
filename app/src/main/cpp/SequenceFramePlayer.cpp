#include "pch.h"
#include "SequenceFramePlayer.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
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
    m_pSequenceTexture = CTexture2D::loadTexture(vAssetManager, m_TexturePath);
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
    m_LastFrameTime = __getCurrentTime();
    return true;
}

void CSequenceFramePlayer::updateFrameAndUV(int vWindowWidth, int vWindowHeight)
{
    double FrameTime   = 1.0 / m_FramePerSecond;
    double CurrentTime = __getCurrentTime();
    if (CurrentTime - m_LastFrameTime >= FrameTime)
    {
        m_LastFrameTime = CurrentTime;
        if (m_CurrentFrame == m_SequenceRows * m_SequenceCols - 1) m_IsFinished = true;
        m_CurrentFrame  = (m_CurrentFrame + 1) % m_ValidFrames;
    }
    float RotationAngle   = m_RotationAngle * M_PI / 180.0f;
    int   CurrentFrameRow = m_CurrentFrame / m_SequenceCols;
    int   CurrentFrameCol = m_CurrentFrame % m_SequenceCols;
    float CurrentFrameU0  = CurrentFrameCol / static_cast<float>(m_SequenceCols);
    float CurrentFrameV0  = CurrentFrameRow / static_cast<float>(m_SequenceRows);
    float CurrentFrameU1  = (CurrentFrameCol + 1) / static_cast<float>(m_SequenceCols);
    float CurrentFrameV1  = (CurrentFrameRow + 1) / static_cast<float>(m_SequenceRows);
    glm::vec2 TextureUVOffset = glm::vec2(CurrentFrameU0, CurrentFrameV0);
    glm::vec2 TextureUVScale  = glm::vec2(CurrentFrameU1 - CurrentFrameU0, CurrentFrameV1 - CurrentFrameV0);
    glm::vec2 OrthoBounds     = glm::vec2(vWindowWidth, vWindowHeight);

    m_pSequenceShaderProgram->useProgram();
//    m_pSequenceShaderProgram->setUniform("rotationAngle", RotationAngle);
//    m_pSequenceShaderProgram->setUniform("screenUVOffset", m_ScreenUVOffset);
//    m_pSequenceShaderProgram->setUniform("screenUVScale", m_ScreenUVScale);
//    m_pSequenceShaderProgram->setUniform("orthoBounds", OrthoBounds);
    m_pSequenceShaderProgram->setUniform("texUVOffset", TextureUVOffset);
    m_pSequenceShaderProgram->setUniform("texUVScale", TextureUVScale);
    m_pSequenceShaderProgram->setUniform("sequenceTexture", 0);
    m_pSequenceTexture->bindTexture();
}

double CSequenceFramePlayer::__getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}