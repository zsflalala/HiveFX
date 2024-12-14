#pragma once

#include <memory>
#include <string>
#include <vector>
#include <android/asset_manager.h>
#include <GLES3/gl3.h>

class CTextureAsset
{
public:
    static std::shared_ptr<CTextureAsset> loadAsset(AAssetManager *vAssetManager, const std::string &vAssetPath);
    ~CTextureAsset();

    [[nodiscard]] constexpr GLuint getTextureID() const { return m_TextureID; }
    static GLuint loadTextureFromAssets(AAssetManager* vAssetManager, const std::string& vAssetPath);

private:
    inline explicit CTextureAsset(GLuint vTextureId);

    GLuint m_TextureID;
};