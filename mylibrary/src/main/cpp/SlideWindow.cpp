#include "SlideWindow.h"
#include <glm/glm.hpp>
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"

using namespace hiveVG;

CSlideWindow::CSlideWindow(const std::string& vTexturePath, float vSpeed, const std::string& vDirection, EPictureType::EPictureType vPictureType, bool vUseCompressed)
                        : m_TexturePath(vTexturePath), m_SlideSpeed(vSpeed), m_SlideDirection(vDirection), m_TextureType(vPictureType), m_UseCompressed(vUseCompressed){}

CSlideWindow::~CSlideWindow()
{
    __deleteSafely(m_pTexture);
    __deleteSafely(m_pShaderProgram);
}

void CSlideWindow::updateFrameAndDraw(int vWindowWidth, int vWindowHeight, double vDeltaTime, CScreenQuad *vQuad)
{
    glm::vec2 ScreenParams = glm::vec2(vWindowWidth, vWindowHeight);
    m_CoordBias += static_cast<float>(vDeltaTime) * m_SlideSpeed;

    const bool IsHorizontal = (m_SlideDirection == "horizontal");
    const int TextureSize = IsHorizontal ? m_TextureWidth : m_TextureHeight;
    const double ratio = m_CoordBias / static_cast<float>(TextureSize);
    const int Steps = static_cast<int>(ratio + (ratio > 0 ? -1e-9 : 1e-9));
    if (Steps != 0)
    {
        m_CoordBias -= static_cast<float>(Steps * TextureSize); // 一步完成正负方向的调整
        if (m_UseCompressed)
            m_Channel = (m_Channel + abs(Steps)) % 4; // 支持快速滑动时的多步调整
    }
    
    m_pShaderProgram->useProgram();
    m_pShaderProgram->setUniform("_ScreenParams", ScreenParams);
    m_pShaderProgram->setUniform("_TextureParams", glm::vec2(m_TextureWidth, m_TextureHeight));
    m_pShaderProgram->setUniform("_CoordBias", m_CoordBias);
    if(m_UseCompressed) m_pShaderProgram->setUniform("_Channel", m_Channel);
    m_pShaderProgram->setUniform("Texture", 0);
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