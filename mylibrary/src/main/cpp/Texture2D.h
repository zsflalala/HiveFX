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
        struct ASTCHeader {
            uint8_t magic[4];       // 文件标识"ASTC"
            uint8_t block_x;        // 块宽度
            uint8_t block_y;        // 块高度
            uint8_t block_z;        // 块深度（通常为1）
            uint8_t dim_x[3];       // 纹理宽度（3字节）
            uint8_t dim_y[3];       // 纹理高度（3字节）
            uint8_t dim_z[3];       // 纹理深度（通常1）
        };

        static CTexture2D* loadTexture(const std::string &vTexturePath);
        static CTexture2D* loadTexture(const std::string &vTexturePath, int &voWidth, int &voHeight, EPictureType::EPictureType& vPictureType, bool vIsCompressed = false, bool vHasAlpha=true);
        static void        loadTextureFromCompressedPNG(const std::string &vTexturePath, int &voWidth, int &voHeight, std::vector<CTexture2D*>& vTexture2DVec);
        static CTexture2D* createEmptyTexture(int vWidth, int vHeight, int vChannels);
        static bool isASTCSupported();
        ~CTexture2D();

        [[nodiscard]] constexpr GLuint getTextureHandle() const { return m_TextureHandle; }

        void bindTexture() const;
    private:
        inline explicit CTexture2D(GLuint vTextureHandle);
        static GLuint __createHandle(GLint vFormat, int vWidth, int vHeight, unsigned char* vImgData);
        static astc_codec::FootprintType __getFootprintType(uint8_t block_x, uint8_t block_y);
        GLuint m_TextureHandle;
    };
}