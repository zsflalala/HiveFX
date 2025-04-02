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
    }else if (vPictureType == EPictureType::ASTC) {

       ASTCHeader header;
       if (AssetSize < sizeof(ASTCHeader)) {
           LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG,"Invalid ASTC file: too small");
           return nullptr;
       }
       memcpy(&header, pBuffer.get(), sizeof(ASTCHeader));

       if (memcmp(header.magic, "\x13\xAB\xA1\x5C", 4) != 0) {
           LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG,"Invalid ASTC file: bad magic number");
           return nullptr;
       }

       astc_codec::FootprintType footprint = __getFootprintType(header.block_x, header.block_y);

       // 从头部获取纹理尺寸（3字节小端序）
       voWidth = header.dim_x[0] | (header.dim_x[1] << 8) | (header.dim_x[2] << 16);
       voHeight = header.dim_y[0] | (header.dim_y[1] << 8) | (header.dim_y[2] << 16);

       LOG_INFO(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG,"Loading ASTC %dx%d texture with block size %dx%d",
                voWidth, voHeight, header.block_x, header.block_y);

       // 计算输出缓冲区大小（RGBA格式）
       size_t out_buffer_size = voWidth * voHeight * 4;
       std::vector<uint8_t> decoded_data(out_buffer_size);

       // 解压ASTC数据（跳过16字节头）
       bool success = astc_codec::ASTCDecompressToRGBA(
               pBuffer.get() + sizeof(ASTCHeader),
               AssetSize - sizeof(ASTCHeader),
               voWidth, voHeight,
               footprint,
               decoded_data.data(),
               out_buffer_size,
               voWidth * 4);
       if (!success)
       {
           LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Failed to decompress ASTC data: %s", vTexturePath.c_str());
           return nullptr;
       }
       double EndTime = CTimeUtils::getCurrentTime();
       LOG_INFO(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Loading image %s from memory to CPU costs time: %f", vTexturePath.c_str(), EndTime - StartTime);
       StartTime = CTimeUtils::getCurrentTime();
       GLint Format = GL_RGBA;
       GLuint tex = __createHandle(Format, voWidth, voHeight, decoded_data.data());
       double EndTime2 = CTimeUtils::getCurrentTime();

       LOG_INFO(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Loading image %s from memory to GPU costs time: %f", vTexturePath.c_str(), EndTime2 - StartTime);
       stbi_image_free(pImageData);
       return new CTexture2D(tex);
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
bool CTexture2D::isASTCSupported() {
    const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
    return strstr(extensions, "GL_KHR_texture_compression_astc_ldr") != nullptr;
}
astc_codec::FootprintType  CTexture2D::__getFootprintType(uint8_t block_x, uint8_t block_y) {
    if (block_x == 4 && block_y == 4) return astc_codec::FootprintType::k4x4;
    if (block_x == 5 && block_y == 4) return astc_codec::FootprintType::k5x4;
    if (block_x == 5 && block_y == 5) return astc_codec::FootprintType::k5x5;
    if (block_x == 6 && block_y == 5) return astc_codec::FootprintType::k6x5;
    if (block_x == 6 && block_y == 6) return astc_codec::FootprintType::k6x6;
    if (block_x == 8 && block_y == 5) return astc_codec::FootprintType::k8x5;
    if (block_x == 8 && block_y == 6) return astc_codec::FootprintType::k8x6;
    if (block_x == 10 && block_y == 5) return astc_codec::FootprintType::k10x5;
    if (block_x == 10 && block_y == 6) return astc_codec::FootprintType::k10x6;
    if (block_x == 8 && block_y == 8) return astc_codec::FootprintType::k8x8;
    if (block_x == 10 && block_y == 8) return astc_codec::FootprintType::k10x8;
    if (block_x == 10 && block_y == 10) return astc_codec::FootprintType::k10x10;
    if (block_x == 12 && block_y == 10) return astc_codec::FootprintType::k12x10;
    if (block_x == 12 && block_y == 12) return astc_codec::FootprintType::k12x12;
    return astc_codec::FootprintType::k4x4; // 默认值
}