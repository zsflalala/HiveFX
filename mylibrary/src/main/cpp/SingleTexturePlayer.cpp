#include "SingleTexturePlayer.h"
#include "Common.h"
#include "Texture2D.h"
#include "ShaderProgram.h"

using namespace hiveVG;

CSingleTexturePlayer::CSingleTexturePlayer(const std::string &vTexturePath) : m_TexturePath(vTexturePath){}

CSingleTexturePlayer::~CSingleTexturePlayer()
{
    if (m_pSingleTexture)
    {
        delete m_pSingleTexture;
        m_pSingleTexture = nullptr;
    }
    if (m_pSingleShaderProgram)
    {
        delete m_pSingleShaderProgram;
        m_pSingleShaderProgram = nullptr;
    }
}

bool CSingleTexturePlayer::initTextureAndShaderProgram(AAssetManager *vAssetManager)
{
    m_pSingleTexture = CTexture2D::loadTexture(vAssetManager, m_TexturePath);
    if (!m_pSingleTexture)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG,
                  "Error loading texture from path [%s].", m_TexturePath.c_str());
        return false;
    }
    m_pSingleShaderProgram = CShaderProgram::createProgram(
            vAssetManager,
            SingleTexPlayVert,
            SingleTexPlayFrag
    );
    assert(m_pSingleShaderProgram != nullptr);
    LOG_INFO(hiveVG::TAG_KEYWORD::SINGLE_PALYER_TAG, "%s frames load Succeed. Program Created Succeed.", m_TexturePath.c_str());
    return true;
}

void CSingleTexturePlayer::updateFrame()
{
    m_pSingleShaderProgram->useProgram();
    m_pSingleShaderProgram->setUniform("quadTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    m_pSingleTexture->bindTexture();
}