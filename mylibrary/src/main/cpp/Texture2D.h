#pragma once

#include <string>
#include <__stddef_size_t.h>
#include "astc-codec/astc-codec.h"
#include "Common.h"

namespace hiveVG
{
    class CTexture2D
    {
    public:
        static CTexture2D *loadTexture(const std::string &vTexturePath);
        static CTexture2D *loadTexture(const std::string &vTexturePath, int &voWidth, int &voHeight, EPictureType::EPictureType &vPictureType, bool vIsCompressed = false);
        static CTexture2D *loadTextureFromPNG8(const std::string &vTexturePath);
        static void loadTextureFromCompressedPNG(const std::string &vTexturePath, int &voWidth, int &voHeight, std::vector<CTexture2D *> &vTexture2DVec);
        static CTexture2D *createEmptyTexture(int vWidth, int vHeight, int vChannels);
        ~CTexture2D();
        [[nodiscard]] constexpr GLuint getTextureHandle() const { return m_TextureHandle; }
        void bindTexture() const;

    private:
        inline explicit CTexture2D(GLuint vTextureHandle);
        static GLuint __createHandle(GLint vFormat, int vWidth, int vHeight, unsigned char *vImgData);

        GLuint m_TextureHandle;
    };
}
