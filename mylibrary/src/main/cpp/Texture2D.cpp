#include "Texture2D.h"
#include <webp/decode.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Common.h"
#include "Logging.h"
#include "FileUtils.h"
#include "TimeUtils.h"
#include <omp.h>


using namespace hiveVG;

CTexture2D* CTexture2D::loadTexture(const std::string &vTexturePath)
{
    int Width, Height;
    EPictureType::EPictureType Type = EPictureType::EPictureType::PNG;
    auto pTexture = loadTexture(vTexturePath, Width, Height, Type);
    return pTexture;
}

CTexture2D* CTexture2D::loadTexture(const std::string &vTexturePath, int &voWidth, int &voHeight, EPictureType::EPictureType& vPictureType, bool vIsCompressed, bool vHasAlpha)
{
    std::unique_ptr<unsigned char[]> pBuffer;
    size_t AssetSize;
    bool IsReadFromAssetManager = true;
    auto pAsset = CFileUtils::openFile(vTexturePath.c_str(),IsReadFromAssetManager);
    if(!pAsset)
    {
        IsReadFromAssetManager = false;
        pAsset = CFileUtils::openFile(vTexturePath.c_str(), IsReadFromAssetManager);
    }

    AssetSize = CFileUtils::getFileBytes(pAsset, IsReadFromAssetManager);
    pBuffer = std::make_unique<unsigned char[]>(AssetSize);
    int Flag = CFileUtils::readFile<unsigned char>(pAsset, pBuffer.get(), AssetSize, IsReadFromAssetManager);
    CFileUtils::closeFile(pAsset, IsReadFromAssetManager);
    if(Flag < 0)
        return nullptr;

    double StartTime = CTimeUtils::getCurrentTime();
    int Channels;
    unsigned char *pImageData = nullptr;

   if (vPictureType == EPictureType::PNG || vPictureType == EPictureType::JPG)
    {
        pImageData = stbi_load_from_memory(pBuffer.get(),  static_cast<int>(AssetSize), &voWidth, &voHeight, &Channels, 0);
    }
    else if (vPictureType == EPictureType::WEBP)
    {
        WebPBitstreamFeatures Features;
        VP8StatusCode Status = WebPGetFeatures(pBuffer.get(), AssetSize, &Features);
        if (Status != VP8_STATUS_OK)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "Failed to get %s WebP features.", vTexturePath.c_str());
            return nullptr;
        }
        bool HasAlpha = Features.has_alpha;

        if (HasAlpha)
        {
            Channels = 4;
            pImageData = WebPDecodeRGBA(pBuffer.get(), AssetSize, &voWidth, &voHeight);
        }
        else
        {
            Channels = 3;
            pImageData = WebPDecodeRGB(pBuffer.get(), AssetSize, &voWidth, &voHeight);
        }
    }else if (vPictureType == EPictureType::ASTC)
    {
        const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
        if (!strstr(extensions, "GL_KHR_texture_compression_astc_ldr")) {
            LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG,"Device does NOT support ASTC compression!");
        }else
            LOG_INFO(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Supported GL extensions: %s", extensions);
       ASTCHeader header;
       if (AssetSize < sizeof(ASTCHeader)) {
           LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Invalid ASTC file: too small");
           return nullptr;
       }
       memcpy(&header, pBuffer.get(), sizeof(ASTCHeader));

       if (memcmp(header.magic, "\x13\xAB\xA1\x5C", 4) != 0) {
           LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG,"Invalid ASTC file: bad magic number");
           return nullptr;
       }

       voWidth = header.dim_x[0] | (header.dim_x[1] << 8) | (header.dim_x[2] << 16);
       voHeight = header.dim_y[0] | (header.dim_y[1] << 8) | (header.dim_y[2] << 16);
       uint32_t blockdim_x = header.block_x;
       uint32_t blockdim_y = header.block_y;
       uint32_t blockdim_z = header.block_z;
       uint32_t xblocks = (voWidth + blockdim_x - 1) / blockdim_x;
       uint32_t yblocks = (voHeight + blockdim_y - 1) / blockdim_y;
       uint32_t zblocks = (1 + blockdim_z - 1) / blockdim_z;
       size_t len = xblocks * yblocks * zblocks * 16;
       if (len != AssetSize - sizeof(ASTCHeader)) {
           LOG_WARN(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Warning: Computed ASTC data size (%zu) doesn't match actual (%zu)", len, AssetSize - sizeof(ASTCHeader));
       }

       GLenum internalFormat = __getASTCInternalFormat(header.block_x, header.block_y);
       if (internalFormat == 0) {
           LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG,"Unsupported ASTC block size: %dx%d", header.block_x, header.block_y);
           return nullptr;
       }
       StartTime = CTimeUtils::getCurrentTime();
       GLuint textureHandle;
       glGenTextures(1, &textureHandle);
       glBindTexture(GL_TEXTURE_2D, textureHandle);

       // 6. 设置纹理参数
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_ASTC_DECODE_PRECISION_EXT, GL_RGBA8);
       // 7. 直接上传压缩数据（跳过16字节头）
       glCompressedTexImage2D(
               GL_TEXTURE_2D,
               0,
               internalFormat,
               voWidth,
               voHeight,
               0,
               len,
               pBuffer.get() + sizeof(ASTCHeader)
       );
        //glGenerateMipmap(GL_TEXTURE_2D);


        int texWidth = 0, texHeight = 0, texCompressed = 0;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &texWidth);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &texHeight);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &texCompressed);

        LOG_INFO(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG,"Texture Width: %d, Height: %d, Compressed: %d", texWidth, texHeight, texCompressed);

        if (texWidth == 0 || texHeight == 0) {
            LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG,"Texture upload failed! Width or height is 0.");
        }
        if (!texCompressed) {
            LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG,"Texture is not compressed as expected.");
        }

        double EndTime = CTimeUtils::getCurrentTime();
        LOG_INFO(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Loading image %s from memory to GPU costs time: %f", vTexturePath.c_str(), EndTime - StartTime);
       // 9. 检查错误
       GLenum err = glGetError();
       if (err != GL_NO_ERROR) {
           LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG,"Failed to upload ASTC texture (GL error: 0x%x)", err);
           glDeleteTextures(1, &textureHandle);
           return nullptr;
       }
       return new CTexture2D(textureHandle);
    }else if (vPictureType == EPictureType::ETC1)
   {
       const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
       if (!strstr(extensions, "GL_OES_compressed_ETC1_RGB8_texture")) {
           LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Device does NOT support ETC1 compression!");
           return nullptr;
       } else {
           LOG_INFO(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Supported GL extensions: %s", extensions);
       }

       if (AssetSize < 16) {
           LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Invalid ETC1 file: too small");
           return nullptr;
       }

       // PKM header validation
       const uint8_t* header = pBuffer.get();
       if (memcmp(header, "PKM ", 4) != 0 && memcmp(header, "PKM 10", 6) != 0) {
           LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Invalid ETC1 file: bad magic number");
           return nullptr;
       }

       voWidth = (header[12] << 8) | header[13];
       voHeight = (header[14] << 8) | header[15];

       GLuint textureHandle;
       glGenTextures(1, &textureHandle);
       glBindTexture(GL_TEXTURE_2D, textureHandle);

       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

       // Upload ETC1 compressed texture (format from extension)
       glCompressedTexImage2D(
               GL_TEXTURE_2D,
               0,
               GL_ETC1_RGB8_OES,
               voWidth,
               voHeight,
               0,
               AssetSize - 16,
               pBuffer.get() + 16
       );

       int texCompressed = 0;
       glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &texCompressed);
       if (!texCompressed) {
           LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Texture is not compressed as expected.");
       }

       GLenum err = glGetError();
       if (err != GL_NO_ERROR) {
           LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Failed to upload ETC1 texture (GL error: 0x%x)", err);
           glDeleteTextures(1, &textureHandle);
           return nullptr;
       }

       return new CTexture2D(textureHandle);
   }

    if (!pImageData)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Failed to load image from memory: %s", vTexturePath.c_str());
        return nullptr;
    }
    else
    {
        double EndTime = CTimeUtils::getCurrentTime();
        LOG_INFO(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Loading image %s from memory to CPU costs time: %f", vTexturePath.c_str(), EndTime - StartTime);
    }

    GLint Format = GL_RGB;
    if (Channels == 3) Format = GL_RGB;
    else if (Channels == 4) Format = GL_RGBA;
    else if (Channels == 1) Format = GL_RED;

    StartTime = CTimeUtils::getCurrentTime();

    GLuint TextureHandle = 0;
    if (!vIsCompressed)
    {
        TextureHandle = __createHandle(Format, voWidth, voHeight, pImageData);
    }
    else
    {
        const uint32_t compHeight = voHeight;
        const uint32_t origHeight = compHeight * 2;
        voHeight *= 2;

        std::vector<uint8_t> OrigPixels;
        OrigPixels.resize(voWidth * origHeight * 4);

        for (uint32_t y = 0; y < compHeight; ++y)
        {
            for (uint32_t x = 0; x < voWidth; ++x)
            {
                const uint8_t* src = pImageData + (y * voWidth + x) * 4;

                // 上半部分（R和Alpha通道）
                uint8_t upperR = src[0];
                uint8_t upperA = src[1];

                // 下半部分（B和Alpha通道）
                uint8_t lowerR = src[2];
                uint8_t lowerA = src[3];

                // 填充目标像素
                uint8_t* upperDst = OrigPixels.data() + ((y * voWidth) + x) * 4;
                upperDst[0] = upperR; // R
                upperDst[1] = upperR; // G
                upperDst[2] = upperR; // B
                upperDst[3] = upperA; // A

                uint8_t* lowerDst = OrigPixels.data() + (((y + compHeight) * voWidth) + x) * 4;
                lowerDst[0] = lowerR; // R
                lowerDst[1] = lowerR; // G
                lowerDst[2] = lowerR; // B
                lowerDst[3] = lowerA; // A
            }
        }
        TextureHandle = __createHandle(Format, voWidth, voHeight, OrigPixels.data());
    }

    if (TextureHandle == 0)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Failed to create texture: %s", vTexturePath.c_str());
        return nullptr;
    }
    else
    {
        double EndTime = CTimeUtils::getCurrentTime();
        LOG_INFO(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Loading image %s from memory to GPU costs time: %f", vTexturePath.c_str(), EndTime - StartTime);
    }
    stbi_image_free(pImageData);
    return new CTexture2D(TextureHandle);
}

void CTexture2D::loadTextureFromCompressedPNG(const std::string &vTexturePath, int &voWidth, int &voHeight, std::vector<CTexture2D*>& vTexture2DVec)
{
    auto pAsset = CFileUtils::openFile(vTexturePath.c_str());
    if (!pAsset)
        return;
    size_t AssetSize = CFileUtils::getFileBytes(pAsset);
    std::unique_ptr<unsigned char[]> pBuffer(new unsigned char[AssetSize]);
    int Flag = CFileUtils::readFile<unsigned char>(pAsset, pBuffer.get(), AssetSize);
    if(Flag < 0)
        return;
    CFileUtils::closeFile(pAsset);

    double StartTime = CTimeUtils::getCurrentTime();
    int Channels;
    unsigned char *pImageData = stbi_load_from_memory(pBuffer.get(),  static_cast<int>(AssetSize), &voWidth, &voHeight, &Channels, 0);

    if (!pImageData)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Failed to load image from memory: %s", vTexturePath.c_str());
        return;
    }
    else
    {
        double EndTime = CTimeUtils::getCurrentTime();
        LOG_INFO(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Loading image %s from memory to CPU costs time: %f", vTexturePath.c_str(), EndTime - StartTime);
    }

    const int TotalPixels = voWidth * voHeight;
    auto *pImage1 = new unsigned char[voWidth * voHeight * Channels]; // 第一张图 (RGBA)
    auto *pImage2 = new unsigned char[voWidth * voHeight * Channels]; // 第二张图 (RGBA)

#pragma omp parallel for
    for (int i = 0; i < TotalPixels; ++i)
    {
        pImage1[i * Channels + 0] = pImageData[i * Channels + 0]; // R
        pImage1[i * Channels + 3] = pImageData[i * Channels + 1]; // A
        memset(&pImage1[i * Channels + 1], pImage1[i * Channels + 0], 2);

        pImage2[i * Channels + 0] = pImageData[i * Channels + 2]; // B
        pImage2[i * Channels + 3] = pImageData[i * Channels + 3]; // A
        memset(&pImage2[i * Channels + 1], pImage2[i * Channels + 0], 2);
    }

    StartTime = CTimeUtils::getCurrentTime();
    GLuint BaseTexHandle;
    glGenTextures(1, &BaseTexHandle);
    glBindTexture(GL_TEXTURE_2D, BaseTexHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, voWidth, voHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImage1);
    glGenerateMipmap(GL_TEXTURE_2D);

    GLuint EnhancedTexHandle;
    glGenTextures(1, &EnhancedTexHandle);
    glBindTexture(GL_TEXTURE_2D, EnhancedTexHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, voWidth, voHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pImage2);
    glGenerateMipmap(GL_TEXTURE_2D);

    bool IsValid = (glIsTexture(BaseTexHandle) == GL_TRUE) && (glIsTexture(EnhancedTexHandle) == GL_TRUE);
    if (!IsValid)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Failed to create texture: %s", vTexturePath.c_str());
        stbi_image_free(pImageData);
        return ;
    }
    else
    {
        double EndTime = CTimeUtils::getCurrentTime();
        LOG_INFO(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Loading image %s from memory to GPU costs time: %f", vTexturePath.c_str(), EndTime - StartTime);
    }

    vTexture2DVec.push_back(new CTexture2D(BaseTexHandle));
    vTexture2DVec.push_back(new CTexture2D(EnhancedTexHandle));

    stbi_image_free(pImageData);
    delete [] pImage1;
    delete [] pImage2;
}

CTexture2D* CTexture2D::createEmptyTexture(int vWidth, int vHeight, int vChannels)
{
    GLint Format = GL_RGB;
    if (vChannels == 3) Format = GL_RGB;
    else if (vChannels == 4) Format = GL_RGBA;
    else if (vChannels == 1) Format = GL_RED;
    else LOG_WARN(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Channel Count is invalid, set default format [GL_RGB].");

    double StartTime = CTimeUtils::getCurrentTime();

    GLuint TextureHandle = __createHandle(Format, vWidth, vHeight, nullptr);

    if (TextureHandle == 0)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Failed to create empty texture.");
        return nullptr;
    }
    else
    {
        double EndTime = CTimeUtils::getCurrentTime();
        LOG_INFO(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Creating empty image costs time: %f", EndTime - StartTime);
    }

    return new CTexture2D(TextureHandle);
}

CTexture2D::~CTexture2D()
{
    glDeleteTextures(1, &m_TextureHandle);
    m_TextureHandle = 0;
}

void CTexture2D::bindTexture() const
{
    LOG_INFO(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Binding texture ID: %d", m_TextureHandle);
    glBindTexture(GL_TEXTURE_2D, m_TextureHandle);
}

CTexture2D::CTexture2D(GLuint vTextureHandle) : m_TextureHandle(vTextureHandle) {}

GLuint CTexture2D::__createHandle(GLint vFormat, int vWidth, int vHeight, unsigned char *vImgData)
{
    GLuint TextureHandle;
    glGenTextures(1, &TextureHandle);
    glBindTexture(GL_TEXTURE_2D, TextureHandle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, vFormat, vWidth, vHeight, 0, vFormat, GL_UNSIGNED_BYTE, vImgData);
    glGenerateMipmap(GL_TEXTURE_2D);
    bool IsValid = (glIsTexture(TextureHandle) == GL_TRUE);
    if (!IsValid)
        return 0;
    else
        return TextureHandle;
}


GLenum CTexture2D::__getASTCInternalFormat(uint8_t block_x, uint8_t block_y) {
    static const std::map<std::pair<uint8_t, uint8_t>, GLenum> formatMap = {
            {{4, 4},   GL_COMPRESSED_RGBA_ASTC_4x4_KHR},
            {{5, 4},   GL_COMPRESSED_RGBA_ASTC_5x4_KHR},
            {{5, 5},   GL_COMPRESSED_RGBA_ASTC_5x5_KHR},
            {{6, 5},   GL_COMPRESSED_RGBA_ASTC_6x5_KHR},
            {{6, 6},   GL_COMPRESSED_RGBA_ASTC_6x6_KHR},
            {{8, 5},   GL_COMPRESSED_RGBA_ASTC_8x5_KHR},
            {{8, 6},   GL_COMPRESSED_RGBA_ASTC_8x6_KHR},
            {{10, 5},  GL_COMPRESSED_RGBA_ASTC_10x5_KHR},
            {{10, 6},  GL_COMPRESSED_RGBA_ASTC_10x6_KHR},
            {{8, 8},   GL_COMPRESSED_RGBA_ASTC_8x8_KHR},
            {{10, 8},  GL_COMPRESSED_RGBA_ASTC_10x8_KHR},
            {{10, 10}, GL_COMPRESSED_RGBA_ASTC_10x10_KHR},
            {{12, 10}, GL_COMPRESSED_RGBA_ASTC_12x10_KHR},
            {{12, 12}, GL_COMPRESSED_RGBA_ASTC_12x12_KHR}
    };

    // 查找对应的格式
    auto it = formatMap.find({block_x, block_y});
    return (it != formatMap.end()) ? it->second : 0;  // 找到返回 GLenum，否则返回 0
}