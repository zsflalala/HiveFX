#pragma once

#include <string>
#include <GLES3/gl3.h>
#include <android/asset_manager.h>
#include "Common.h"

namespace hiveVG
{
    class CTexture2D
    {
    public:
        static CTexture2D* loadTexture(AAssetManager *vAssetManager, const std::string &vTexturePath);
        static CTexture2D* loadTexture(AAssetManager *vAssetManager, const std::string &vTexturePath, int &voWidth, int &voHeight, EPictureType::EPictureType& vPictureType);

        ~CTexture2D();

        [[nodiscard]] constexpr GLuint getTextureHandle() const { return m_TextureHandle; }

        void bindTexture() const;

    private:
        inline explicit CTexture2D(GLuint vTextureHandle);
        static double __getCurrentTime();

        GLuint m_TextureHandle;
    };
}