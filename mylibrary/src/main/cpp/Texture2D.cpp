#include "pch.h"
#include "Texture2D.h"
#include <webp/decode.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Common.h"
#include "logging.h"
#include "FileUtils.h"
#include "TimeUtils.h"
#include <omp.h>

using namespace hiveVG;

CTexture2D* CTexture2D::loadTexture(const std::string &vTexturePath)
{
    auto pAsset = CFileUtils::openFile(vTexturePath.c_str());
    assert(pAsset);
    if (!pAsset)
        return nullptr;
    size_t AssetSize = CFileUtils::getFileBytes(pAsset);
    std::unique_ptr<unsigned char[]> pBuffer(new unsigned char[AssetSize]);
    size_t Flag = CFileUtils::readFile<unsigned char>(pAsset, pBuffer.get(), AssetSize);
    if(Flag < 0)
        return nullptr;
    CFileUtils::closeFile(pAsset);

    double StartTime = CTimeUtils::getCurrentTime();
    int Width, Height, Channels;
    unsigned char* pImageData = stbi_load_from_memory(pBuffer.get(), static_cast<int>(AssetSize), &Width, &Height, &Channels, 0);
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
    GLuint TextureHandle;
    glGenTextures(1, &TextureHandle);
    glBindTexture(GL_TEXTURE_2D, TextureHandle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, pImageData);
    glGenerateMipmap(GL_TEXTURE_2D);

    bool IsValid = (glIsTexture(TextureHandle) == GL_TRUE);
    if (!IsValid)
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

CTexture2D* CTexture2D::loadTexture(const std::string &vTexturePath, int &voWidth, int &voHeight, EPictureType::EPictureType& vPictureType)
{
    auto pAsset = CFileUtils::openFile(vTexturePath.c_str());
    assert(pAsset);
    if (!pAsset)
        return nullptr;
    size_t AssetSize = CFileUtils::getFileBytes(pAsset);
    std::unique_ptr<unsigned char[]> pBuffer(new unsigned char[AssetSize]);
    size_t Flag = CFileUtils::readFile<unsigned char>(pAsset, pBuffer.get(), AssetSize);
    if(Flag < 0)
        return nullptr;
    CFileUtils::closeFile(pAsset);

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
    GLuint TextureHandle;
    glGenTextures(1, &TextureHandle);
    glBindTexture(GL_TEXTURE_2D, TextureHandle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, Format, voWidth, voHeight, 0, Format, GL_UNSIGNED_BYTE, pImageData);
    glGenerateMipmap(GL_TEXTURE_2D);

    bool IsValid = (glIsTexture(TextureHandle) == GL_TRUE);
    if (!IsValid)
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
    assert(pAsset);
    if (!pAsset)
        return;
    size_t AssetSize = CFileUtils::getFileBytes(pAsset);
    std::unique_ptr<unsigned char[]> pBuffer(new unsigned char[AssetSize]);
    size_t Flag = CFileUtils::readFile<unsigned char>(pAsset, pBuffer.get(), AssetSize);
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

CTexture2D *CTexture2D::loadTextureFromMobile(const std::string &vTexturePath)
{
    int Width, Height, Channels;
    unsigned char* pData = stbi_load(vTexturePath.c_str(), &Width, &Height, &Channels, 0);
    if (pData == nullptr)
    {
        LOG_ERROR(TAG_KEYWORD::TEXTURE2D_TAG, "Failed to load texture asset: [%s]", vTexturePath.c_str());
        return nullptr;
    }

    GLint Format = GL_RGB;
    if (Channels == 3) Format = GL_RGB;
    else if (Channels == 4) Format = GL_RGBA;
    else if (Channels == 1) Format = GL_RED;

    GLuint TextureHandle;
    glGenTextures(1, &TextureHandle);
    glBindTexture(GL_TEXTURE_2D, TextureHandle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, pData);
    glGenerateMipmap(GL_TEXTURE_2D);

    bool IsValid = (glIsTexture(TextureHandle) == GL_TRUE);
    if (!IsValid)
    {
        LOG_ERROR(TAG_KEYWORD::TEXTURE2D_TAG, "Failed to create texture: [%s]", vTexturePath.c_str());
        return nullptr;
    }
    stbi_image_free(pData);
    return new CTexture2D(TextureHandle);
}

CTexture2D* CTexture2D::createEmptyTexture(int vWidth, int vHeight, int vChannels)
{
    GLint Format = GL_RGB;
    if (vChannels == 3) Format = GL_RGB;
    else if (vChannels == 4) Format = GL_RGBA;
    else if (vChannels == 1) Format = GL_RED;
    else LOG_WARN(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Channel Count is invalid, set default format [GL_RGB].");

    double StartTime = CTimeUtils::getCurrentTime();
    GLuint TextureHandle;
    glGenTextures(1, &TextureHandle);
    glBindTexture(GL_TEXTURE_2D, TextureHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, Format, vWidth, vHeight, 0, Format, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    bool IsValid = (glIsTexture(TextureHandle) == GL_TRUE);
    if (!IsValid)
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