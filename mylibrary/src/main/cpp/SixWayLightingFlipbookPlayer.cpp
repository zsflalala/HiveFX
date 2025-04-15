#include "SixWayLightingFlipbookPlayer.h"
#include <random>
#include <webp/decode.h>
#include <glm/ext/matrix_transform.hpp>
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "Board.h"

#define M_PI 3.14159265358979323846

using namespace hiveVG;




CSixWayLightingFlipbookPlayer::CSixWayLightingFlipbookPlayer(const std::string &vTextureRootPath, int vSequenceRows, int vSequenceCols, int vTextureCount, EPictureType::EPictureType vPictureType)
        : CSequenceFramePlayer(vTextureRootPath, vSequenceRows, vSequenceCols, vTextureCount, vPictureType)
{

}

CSixWayLightingFlipbookPlayer::CSixWayLightingFlipbookPlayer(const std::string &vTextureRootPath, int vSequenceRows, int vSequenceCols, int vTextureCount, bool vUseCompressedPNG)
        : CSequenceFramePlayer(vTextureRootPath, vSequenceRows, vSequenceCols, vTextureCount, vUseCompressedPNG)
{
    m_TextureType = EPictureType::PNG;
}

CSixWayLightingFlipbookPlayer::~CSixWayLightingFlipbookPlayer()
{
    for (int i = m_SixWayLightingTextures.size() - 1; i >= 0; i--)
    {
        if (m_SixWayLightingTextures[i] != nullptr)
        {
            delete m_SixWayLightingTextures[i]->m_pSixWayLightingTextureA;
            delete m_SixWayLightingTextures[i]->m_pSixWayLightingTextureB;
            delete m_SixWayLightingTextures[i]->m_pSixWayLightingTextureAlbedo;
            delete m_SixWayLightingTextures[i];
        }
    }
    m_SixWayLightingTextures.clear();
    if (m_pShaderProgram)
    {
        delete m_pShaderProgram;
        m_pShaderProgram = nullptr;
    }
}

bool CSixWayLightingFlipbookPlayer::initTextureAndShaderProgram(bool vIsCompressed,bool vFlipYAxis)
{
    if (!m_TextureRootPath.empty() && m_TextureRootPath.back() != '/')
        m_TextureRootPath += '/';
    std::string PictureSuffix;
    if (m_TextureType == EPictureType::PNG)       PictureSuffix = ".png";
    else if (m_TextureType == EPictureType::JPG)  PictureSuffix = ".jpg";
    else if (m_TextureType == EPictureType::WEBP) PictureSuffix = ".webp";
    for (int i = 0; i < m_TextureCount; i++)
    {
        std::string TexturePaths[3];
        std::string Prefix = m_TextureRootPath + "frame_" + std::string(3 - std::to_string(i + 1).length(), '0') + std::to_string(i + 1);
        TexturePaths[0].append(Prefix).append("_A" + PictureSuffix);
        TexturePaths[1].append(Prefix).append( "_B" + PictureSuffix);
        TexturePaths[2].append(Prefix).append( "_Albedo" + PictureSuffix);

        CTexture2D* pTextures[3];
        if (!m_UseCompressedPNG)
        {
            for(int i = 0; i < 3; i++){
                pTextures[i] = CTexture2D::loadTexture(TexturePaths[i], m_SequenceWidth, m_SequenceHeight, m_TextureType,vIsCompressed,vFlipYAxis);
                if (pTextures[i] == nullptr)
                {
                    LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "Error loading texture from path [%s].", TexturePaths[0].c_str());
                    return false;
                }
            }
            auto *pSixWayLightingTexture = new SSixWayLightingTexture();
            pSixWayLightingTexture->m_pSixWayLightingTextureA = pTextures[0];
            pSixWayLightingTexture->m_pSixWayLightingTextureB = pTextures[1];
            pSixWayLightingTexture->m_pSixWayLightingTextureAlbedo = pTextures[2];
            m_SixWayLightingTextures.push_back(pSixWayLightingTexture);
        }
        else
        {

            LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "Error loading texture from path [%s]. Procedure not implemented.", TexturePaths[0].c_str());
            return false;

        }
    }
    m_SeqSingleTexWidth  = m_SequenceWidth / m_SequenceCols;
    m_SeqSingleTexHeight = m_SequenceHeight / m_SequenceRows;
    m_pShaderProgram = CShaderProgram::createProgram(SixWayLightTexVert,SixWayLightTexFrag);
    if (!m_pShaderProgram)
    {
        LOG_INFO(hiveVG::TAG_KEYWORD::SEQFRAME_PALYER_TAG, "[%s] ShaderProgram init Failed.", m_TextureRootPath.c_str());
        return false;
    }
    assert(m_pShaderProgram != nullptr);
    LOG_INFO(hiveVG::TAG_KEYWORD::SEQFRAME_PALYER_TAG, "%s frames load Succeed. Program Created Succeed.", m_TextureRootPath.c_str());
    return true;
}

void CSixWayLightingFlipbookPlayer::updateFrameIndex(double vDeltaTime)
{
    double FrameTime = 1.0 / m_FramePerSecond;
    m_AccumFrameTime += vDeltaTime;
    double epsilon = 1E-5;
    m_CurrentFrame = static_cast<int>(m_AccumFrameTime/FrameTime) % m_ValidFrames;
    m_CurrentFrameIndex = m_AccumFrameTime/FrameTime;

    m_CurrentFrameIndexFraction = static_cast<float>(m_AccumFrameTime/FrameTime) - static_cast<int>(m_AccumFrameTime/FrameTime);
}




void CSixWayLightingFlipbookPlayer::draw(CBoard *vBoard)
{

    using namespace glm;

    mat4x4 Modeling = translate( scale( identity<mat4x4>(), vec3(2,0.5,1)), vec3(0.0f, 0.0f, 0.0f));
 //   scale( Modeling, vec3(m_ScreenUVScale, m_ScreenUVScale))
    mat4x4 ModelingInverseTranspose = transpose(inverse(Modeling));
    mat4x4 View = identity<mat4x4>();
    mat4x4 Projection = identity<mat4x4>();



    assert(m_pShaderProgram != nullptr);
    m_pShaderProgram->useProgram();
    m_pShaderProgram->setUniform("uModeling", Modeling);
    //LOG_INFO(hiveVG::TAG_KEYWORD::SEQFRAME_PALYER_TAG, "1OpenGL Error Occurred: [%d].", glGetError());
    m_pShaderProgram->setUniform("uModelIT", ModelingInverseTranspose);
    //LOG_INFO(hiveVG::TAG_KEYWORD::SEQFRAME_PALYER_TAG, "2OpenGL Error Occurred: [%d].", glGetError());
    m_pShaderProgram->setUniform("uView", View);
    //LOG_INFO(hiveVG::TAG_KEYWORD::SEQFRAME_PALYER_TAG, "3OpenGL Error Occurred: [%d].", glGetError());
    m_pShaderProgram->setUniform("uProjection", Projection);
    //LOG_INFO(hiveVG::TAG_KEYWORD::SEQFRAME_PALYER_TAG, "4OpenGL Error Occurred: [%d].", glGetError());
    m_pShaderProgram->setUniform("uSeqFrameParams.currFrameIndex",  static_cast<float>(m_CurrentFrame) + m_CurrentFrameIndexFraction);
    //LOG_INFO(hiveVG::TAG_KEYWORD::SEQFRAME_PALYER_TAG, "5OpenGL Error Occurred: [%d].", glGetError());
    m_pShaderProgram->setUniform("uSeqFrameParams.flipBookShape", vec2(m_SequenceCols,m_SequenceRows));
    //LOG_INFO(hiveVG::TAG_KEYWORD::SEQFRAME_PALYER_TAG, "6OpenGL Error Occurred: [%d].", glGetError());
    m_pShaderProgram->setUniform("uDistantLight.intensity", vec3(1.0f, 1.0f, 1.0f));
    //LOG_INFO(hiveVG::TAG_KEYWORD::SEQFRAME_PALYER_TAG, "7penGL Error Occurred: [%d].", glGetError());
    m_pShaderProgram->setUniform("uDistantLight.direction", vec3(-1, -0.6f, -0.6f));
    //LOG_INFO(hiveVG::TAG_KEYWORD::SEQFRAME_PALYER_TAG, "8OpenGL Error Occurred: [%d].", glGetError());
    assert(glGetError() == GL_NO_ERROR);
   /* LOG_INFO(hiveVG::TAG_KEYWORD::SEQFRAME_PALYER_TAG, "frame index %.2f ",
             static_cast<float>(m_CurrentFrame) + m_CurrentFrameIndexFraction);*/
    auto *pCurrTexture = m_SixWayLightingTextures[m_CurrentTexture];

    m_pShaderProgram->setUniform("uFlipBookAlbedo", 0);
  //  LOG_INFO(hiveVG::TAG_KEYWORD::SEQFRAME_PALYER_TAG, "9OpenGL Error Occurred: [%d].", glGetError());
    glActiveTexture(GL_TEXTURE0);

    pCurrTexture->m_pSixWayLightingTextureAlbedo->bindTexture();


    m_pShaderProgram->setUniform("uFlipBookLightMap.mapRTB", 1);
   // LOG_INFO(hiveVG::TAG_KEYWORD::SEQFRAME_PALYER_TAG, "10OpenGL Error Occurred: [%d].", glGetError());

    glActiveTexture(GL_TEXTURE0 + 1);
    pCurrTexture->m_pSixWayLightingTextureA->bindTexture();

    m_pShaderProgram->setUniform("uFlipBookLightMap.mapLBF", 2);
   // LOG_INFO(hiveVG::TAG_KEYWORD::SEQFRAME_PALYER_TAG, "11OpenGL Error Occurred: [%d].", glGetError());

    glActiveTexture(GL_TEXTURE0 + 2);
    pCurrTexture->m_pSixWayLightingTextureB->bindTexture();

    vBoard->bindAndDraw();
}



void CSixWayLightingFlipbookPlayer::__initSequenceParams()
{
    std::random_device Rd;
    std::mt19937 Gen(Rd());
    std::uniform_int_distribution<> IntDistribution(0, 1);
    std::uniform_real_distribution<float> FloatDistribution(0.0, 1.0);

    m_SequenceState._IsAlive = true;

    FloatDistribution.param(std::uniform_real_distribution<float>::param_type(30.0f, 40.0f));
    m_SequenceState._PlannedLivingTime = FloatDistribution(Gen);

    FloatDistribution.param(std::uniform_real_distribution<float>::param_type(1.0f, 2.0f));
    m_SequenceState._PlannedDeadTime   = FloatDistribution(Gen);
    m_SequenceState._AlreadyDeadTime   = 0;
    m_SequenceState._AlreadyLivingTime = 0;

    FloatDistribution.param(std::uniform_real_distribution<float>::param_type(0.2f, 1.0f));
    float ScreenRandomUV = FloatDistribution(Gen);
    m_ScreenUVScale = glm::vec2(ScreenRandomUV, ScreenRandomUV);

    FloatDistribution.param(std::uniform_real_distribution<float>::param_type(-0.5f, 0.5f));
    float ScreenRandomOffset = FloatDistribution(Gen);
    // TODO : move from up to down is to be completed later
    float ScreenMaxUV = 1.0f;
    m_ScreenUVOffset = m_MovingSpeed.x > 0 ? glm::vec2(-ScreenMaxUV - ScreenRandomUV, ScreenRandomOffset) : glm::vec2(ScreenMaxUV + ScreenRandomUV, ScreenRandomOffset);

    float MovingDistance = 2.0f + 2 * m_SequenceState._UVScale; // 2.0f is from -1.0 ~ 1.0; * 2 is from left to right
    float Speed = MovingDistance / m_SequenceState._PlannedLivingTime;
    m_MovingSpeed.x = m_MovingSpeed.x > 0 ? Speed : -Speed;
}