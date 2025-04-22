#include "SlideWindow.h"
#include <glm/glm.hpp>
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include <algorithm>
using namespace hiveVG;

CSlideWindow::CSlideWindow(const std::string& vTexturePath, float vSpeed, const std::string& vDirection, EPictureType::EPictureType vPictureType, bool vUseCompressed)
                        : m_TexturePath(vTexturePath), m_SlideSpeed(vSpeed), m_SlideDirection(vDirection), m_TextureType(vPictureType), m_UseCompressed(vUseCompressed){}

CSlideWindow::~CSlideWindow()
{
    __deleteSafely(m_pTexture);
    __deleteSafely(m_pShaderProgram);
}

void CSlideWindow::updateFrameAndDraw(int vWindowWidth, int vWindowHeight, double vDeltaTime, CScreenQuad* vQuad)
{
    // 直接累加偏移量，不限制范围（Shader会处理循环）
    m_CoordBias += static_cast<float>(vDeltaTime) * m_SlideSpeed;
    float textureHeight = static_cast<float>(m_TextureHeight);
    if(abs(m_CoordBias) >= textureHeight)
    {
        m_CoordBias = fmod(m_CoordBias, textureHeight);
    }
    m_pShaderProgram->useProgram();
    m_pShaderProgram->setUniform("_ScreenParams", glm::vec2(vWindowWidth, vWindowHeight));
    m_pShaderProgram->setUniform("_TextureParams", glm::vec2(m_TextureWidth, m_TextureHeight));
    m_pShaderProgram->setUniform("_CoordBias", m_CoordBias);
    m_pShaderProgram->setUniform("_Channel", 0);

    glActiveTexture(GL_TEXTURE0);
    m_pTexture->bindTexture();
    vQuad->bindAndDraw();
}

bool CSlideWindow::initTextureAndShaderProgram()
{
    m_pTexture = CTexture2D::loadTexture(m_TexturePath, m_TextureWidth, m_TextureHeight, m_TextureType);

    if (m_SlideDirection == "horizontal")
    {
        if (!m_UseCompressed)
            m_pShaderProgram = CShaderProgram::createProgram(SlideWindowVert, SlideWindowHFrag);
        else
            m_pShaderProgram = CShaderProgram::createProgram(SlideWindowVert, SlideWindowHCFrag);
    }
    else if (m_SlideDirection == "vertical")
    {
        if (!m_UseCompressed)
            m_pShaderProgram = CShaderProgram::createProgram(SlideWindowVert, SlideWindowVFrag);
        else
            m_pShaderProgram = CShaderProgram::createProgram(SlideWindowVert, SlideWindowVCFrag);
    }

    if (!m_pShaderProgram)
    {
        LOG_INFO(hiveVG::TAG_KEYWORD::SEQFRAME_PALYER_TAG, "SlideWindow ShaderProgram init Failed.");
        return false;
    }
    assert(m_pShaderProgram != nullptr);
    LOG_INFO(hiveVG::TAG_KEYWORD::SEQFRAME_PALYER_TAG, "%s frames load Succeed. Program Created Succeed.", m_TexturePath.c_str());
    return true;
}
bool CSlideWindow::initTextureAndShaderProgram(std::string& vVertexShaderPath, std::string& vFragShaderShaderPath)
{
    m_pTexture = CTexture2D::loadTexture(m_TexturePath, m_TextureWidth, m_TextureHeight,
                                         m_TextureType);
    m_pShaderProgram = CShaderProgram::createProgram(vVertexShaderPath, vFragShaderShaderPath);
    if (!m_pShaderProgram)
    {
        LOG_INFO(hiveVG::TAG_KEYWORD::SEQFRAME_PALYER_TAG, "SlideWindow ShaderProgram init Failed.");
        return false;
    }
    assert(m_pShaderProgram != nullptr);
    LOG_INFO(hiveVG::TAG_KEYWORD::SEQFRAME_PALYER_TAG, "%s frames load Succeed. Program Created Succeed.", m_TexturePath.c_str());
    return true;
}