#include "pch.h"
#include "Texture2D.h"
#include <webp/decode.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Common.h"
#include "TimeUtils.h"

using namespace hiveVG;

CTexture2D* CTexture2D::loadTexture(AAssetManager *vAssetManager, const std::string &vTexturePath)
{
    if (!vAssetManager)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "AssetManager is null.");
        return nullptr;
    }

    AAsset* pAsset = AAssetManager_open(vAssetManager, vTexturePath.c_str(), AASSET_MODE_BUFFER);
    if (!pAsset)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Failed to open asset: %s", vTexturePath.c_str());
        return nullptr;
    }

    size_t AssetSize = AAsset_getLength(pAsset);
    std::unique_ptr<unsigned char[]> pBuffer(new unsigned char[AssetSize]);
    AAsset_read(pAsset, pBuffer.get(), AssetSize);
    AAsset_close(pAsset);

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

CTexture2D* CTexture2D::loadTexture(AAssetManager *vAssetManager, const std::string &vTexturePath, int &voWidth, int &voHeight, EPictureType::EPictureType& vPictureType)
{
    if (!vAssetManager)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "AssetManager is null.");
        return nullptr;
    }

    AAsset* pAsset = AAssetManager_open(vAssetManager, vTexturePath.c_str(), AASSET_MODE_BUFFER);
    if (!pAsset)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Failed to open asset: %s", vTexturePath.c_str());
        return nullptr;
    }

    size_t AssetSize = AAsset_getLength(pAsset);
    std::unique_ptr<unsigned char[]> pBuffer(new unsigned char[AssetSize]);
    AAsset_read(pAsset, pBuffer.get(), AssetSize);
    AAsset_close(pAsset);

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

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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