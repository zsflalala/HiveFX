#include "SingleTexturePlayer.h"
#include "Common.h"
#include "Texture2D.h"
#include "ShaderProgram.h"

using namespace hiveVG;

CSingleTexturePlayer::CSingleTexturePlayer(const std::string &vTexturePath) : m_TexturePath(vTexturePath){}

bool CSingleTexturePlayer::initTextureAndShaderProgram(AAssetManager *vAssetManager)
{
    m_pSingleTexture = CTexture2D::loadTexture(vAssetManager, m_TexturePath);
    if (!m_pSingleTexture)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "Error loading texture from path [%s].", m_TexturePath.c_str());
        return false;
    }
    m_pSingleShaderProgram = CShaderProgram::createProgram(
            vAssetManager,
            "shaders/singleTexturePlayer.vert",
            "shaders/singleTexturePlayer.frag"
    );
    assert(m_pSingleShaderProgram != nullptr);
    return true;
}

void CSingleTexturePlayer::updateShaderAndTexture()
{
    m_pSingleShaderProgram->useProgram();
    m_pSingleTexture->bindTexture();
}
