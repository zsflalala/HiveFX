#include "pch.h"
#include "SequenceFramePlayer.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "Common.h"
#include "webp/decode.h"

#define M_PI 3.14159265358979323846

using namespace hiveVG;

CSequenceFramePlayer::CSequenceFramePlayer(const std::string& vTextureRootPath, int vSequenceRows, int vSequenceCols, int vTextureCount, EPictureType vPictureType)
        : m_SequenceRows(vSequenceRows), m_SequenceCols(vSequenceCols), m_TextureRootPath(vTextureRootPath), m_TextureCount(vTextureCount), m_TextureType(vPictureType)
{
    m_ValidFrames = m_SequenceRows * m_SequenceCols;
}

CSequenceFramePlayer::~CSequenceFramePlayer()
{
    for (int i = m_SeqTextures.size() - 1; i >= 0; i--)
    {
        if (m_SeqTextures[i])
        {
            delete m_SeqTextures[i];
            m_SeqTextures[i] = nullptr;
            m_SeqTextures.pop_back();
        }
    }

    if (m_pSequenceShaderProgram)
    {
        delete m_pSequenceShaderProgram;
        m_pSequenceShaderProgram = nullptr;
    }
}

bool CSequenceFramePlayer::initTextureAndShaderProgram(AAssetManager* vAssetManager)
{
    std::string PictureSuffix;
    if (m_TextureType == EPictureType::PNG) PictureSuffix = ".png";
    else if (m_TextureType == EPictureType::JPG) PictureSuffix = ".jpg";
    else if (m_TextureType == EPictureType::WEBP) PictureSuffix = ".webp";
    else if (m_TextureType == EPictureType::ASTC) PictureSuffix = ".astc";
    for (int i = 0; i < m_TextureCount; i++)
    {
        std::string TexturePath = m_TextureRootPath + "/frame_" + std::string(3 - std::to_string(i + 1).length(), '0') + std::to_string(i + 1) + PictureSuffix;;
        CTexture2D* pSequenceTexture = CTexture2D::loadTexture(vAssetManager, TexturePath, m_SequeceWidth, m_SequeceHeight, m_TextureType);
        if (!pSequenceTexture)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "Error loading texture from path [%s].", TexturePath.c_str());
            return false;
        }
        m_SeqTextures.push_back(pSequenceTexture);
    }
    m_SequeceSingleTextureWidth  = m_SequeceWidth / m_SequenceCols;
    m_SequeceSingleTextureHeight = m_SequeceHeight / m_SequenceRows;
    m_pSequenceShaderProgram = CShaderProgram::createProgram(
            vAssetManager,
            "shaders/sequenceTexturePlayer.vert",
            "shaders/sequenceTexturePlayer.frag"
    );
    assert(m_pSequenceShaderProgram != nullptr);
    LOG_INFO(hiveVG::TAG_KEYWORD::SEQFRAME_PALYER_TAG, "%s frames load Succeed. Program Created Succeed.", m_TextureRootPath.c_str());
    return true;
}

void CSequenceFramePlayer::updateFrameAndUV(int vWindowWidth, int vWindowHeight, double vDeltaTime)
{
    double FrameTime = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += vDeltaTime;
    if (m_AccumFrameTime >= FrameTime)
    {
        m_AccumFrameTime = 0.0f;
        if (m_CurrentFrame == m_ValidFrames - 1)
        {
            if (m_CurrentTexture == m_TextureCount - 1) m_IsFinished = true;
            m_CurrentTexture = (m_CurrentTexture + 1) % m_TextureCount;
        }
        m_CurrentFrame = (m_CurrentFrame + 1) % m_ValidFrames;
    }
    m_WindowSize = glm::vec2(vWindowWidth, vWindowHeight);
}

void CSequenceFramePlayer::draw(CScreenQuad *vQuad)
{
    if (!m_IsLoop && m_IsFinished)
    {
        m_CurrentFrame   = m_ValidFrames - 1;
        m_CurrentTexture = m_TextureCount - 1;
    }
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
    m_pSequenceShaderProgram->setUniform("screenUVScale", m_ScreenUVScale * m_ScreenRandScale);
    m_pSequenceShaderProgram->setUniform("texUVOffset", TextureUVOffset);
    m_pSequenceShaderProgram->setUniform("texUVScale", TextureUVScale);
    m_pSequenceShaderProgram->setUniform("sequenceTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    m_SeqTextures[m_CurrentTexture]->bindTexture();
    vQuad->bindAndDraw();
}