#pragma once

#include <string>
#include <GLES3/gl3.h>
#include <android/asset_manager.h>

namespace hiveVG
{
    class CTexture2D
    {
    public:
        static CTexture2D* loadTexture(AAssetManager *vAssetManager, const std::string & vTexturePath);

        ~CTexture2D();

        [[nodiscard]] constexpr GLuint getTextureHandle() const { return m_TextureHandle; }

        void bindTexture() const;

    private:
        inline explicit CTexture2D(GLuint vTextureHandle);

        GLuint m_TextureHandle;
    };
}