#include "SlideWindow.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"

using namespace hiveVG;

CSlideWindow::CSlideWindow(std::string& vTexturePath, float vSpeed, std::string& vDirection)
                        : m_TexturePath(vTexturePath), m_SlideSpeed(vSpeed), m_SlideDirection(vDirection){}

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

void CSlideWindow::updateFrame(int vWindowWidth, int vWindowHeight, double vDeltaTime, CScreenQuad* vQuad)
{
    glm::vec2 ScreenParams = glm::vec2(vWindowWidth, vWindowHeight);
    m_CoordBias += static_cast<float>(vDeltaTime);

    m_pShaderProgram->useProgram();
    m_pShaderProgram->setUniform("_ScreenParams", ScreenParams);
    m_pShaderProgram->setUniform("_TextureParams", glm::vec2(m_TextureWidth, m_TextureHeight));
    m_pShaderProgram->setUniform("_CoordBias", m_CoordBias * m_SlideSpeed);

    vQuad->bindAndDraw();
}

void CSlideWindow::createProgram(AAssetManager *vAssetManager)
{
    if (m_SlideDirection == "horizontal") m_pShaderProgram = CShaderProgram::createProgram(vAssetManager, SlideWindowVert, SlideWindowHFrag);
    if (m_SlideDirection == "vertical")   m_pShaderProgram = CShaderProgram::createProgram(vAssetManager, SlideWindowVert, SlideWindowVFrag);
}

void CSlideWindow::loadTextures(AAssetManager* vAssetManager)
{
    m_pTexture = CTexture2D::loadTexture(vAssetManager, m_TexturePath, m_TextureWidth, m_TextureHeight, m_TextureType);
}