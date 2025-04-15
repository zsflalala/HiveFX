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
    if (m_pTexture)
    {
        delete m_pTexture;
        m_pTexture = nullptr;
    }

    if (m_pShaderProgram)
    {
        delete m_pShaderProgram;
        m_pShaderProgram = nullptr;
    }
}

void CSlideWindow::updateFrameAndDraw(int vWindowWidth, int vWindowHeight, double vDeltaTime, CScreenQuad *vQuad)
{
    glm::vec2 ScreenParams = glm::vec2(vWindowWidth, vWindowHeight);
    m_CoordBias += static_cast<float>(vDeltaTime) * m_SlideSpeed;

    auto TextureWidth = static_cast<float>(m_TextureWidth);
    auto TextureHeight = static_cast<float>(m_TextureHeight);

    if (m_SlideDirection == "horizontal")
    {
        if (m_CoordBias / TextureWidth > 1.0) m_CoordBias -= TextureWidth;
        if (m_CoordBias / TextureWidth < -1.0) m_CoordBias += TextureWidth;
    }
    if (m_SlideDirection == "vertical")
    {
        if (m_CoordBias / TextureHeight > 1.0) m_CoordBias -= TextureHeight;
        if (m_CoordBias / TextureHeight < -1.0) m_CoordBias += TextureHeight;
    }
    
    m_pShaderProgram->useProgram();
    m_pShaderProgram->setUniform("_ScreenParams", ScreenParams);
    m_pShaderProgram->setUniform("_TextureParams", glm::vec2(m_TextureWidth, m_TextureHeight));
    m_pShaderProgram->setUniform("_CoordBias", m_CoordBias);
    m_pShaderProgram->setUniform("Texture", 0);
    glActiveTexture(GL_TEXTURE0);
    m_pTexture->bindTexture();

    vQuad->bindAndDraw();
}

bool CSlideWindow::initTextureAndShaderProgram()
{
    m_pTexture = CTexture2D::loadTexture(m_TexturePath, m_TextureWidth, m_TextureHeight, m_TextureType,m_UseCompressed);

    if (m_SlideDirection == "horizontal")
        m_pShaderProgram = CShaderProgram::createProgram(SlideWindowVert, SlideWindowHFrag);
    if (m_SlideDirection == "vertical")
        m_pShaderProgram = CShaderProgram::createProgram(SlideWindowVert, SlideWindowVFrag);
    if (!m_pShaderProgram)
    {
        LOG_INFO(hiveVG::TAG_KEYWORD::SEQFRAME_PALYER_TAG, "SlideWindow ShaderProgram init Failed.");
        return false;
    }
    assert(m_pShaderProgram != nullptr);
    LOG_INFO(hiveVG::TAG_KEYWORD::SEQFRAME_PALYER_TAG, "%s frames load Succeed. Program Created Succeed.", m_TexturePath.c_str());
    return true;
}
