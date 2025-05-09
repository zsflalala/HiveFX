#include "Texture2D.h"
#include <omp.h>
#include <webp/decode.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "lodepng.h"
#include "Common.h"
#include "Logging.h"
#include "FileUtils.h"
#include "TimeUtils.h"
#include "ktx.h"

using namespace hiveVG;

CTexture2D *CTexture2D::loadTexture(const std::string &vTexturePath)
{
    int Width, Height;
    EPictureType::EPictureType Type = EPictureType::EPictureType::PNG;
    auto pTexture = loadTexture(vTexturePath, Width, Height, Type);
    return pTexture;
}

CTexture2D *CTexture2D::loadTexture(const std::string &vTexturePath, int &voWidth, int &voHeight, EPictureType::EPictureType &vPictureType, bool vIsCompressed)
{
    std::unique_ptr<unsigned char[]> pBuffer;
    size_t AssetSize;
    bool IsReadFromAssetManager = true;
    auto pAsset = CFileUtils::openFile(vTexturePath.c_str(), IsReadFromAssetManager);
    if (!pAsset)
    {
        IsReadFromAssetManager = false;
        pAsset = CFileUtils::openFile(vTexturePath.c_str(), IsReadFromAssetManager);
    }

    AssetSize = CFileUtils::getFileBytes(pAsset, IsReadFromAssetManager);
    pBuffer = std::make_unique<unsigned char[]>(AssetSize);
    int Flag = CFileUtils::readFile<unsigned char>(pAsset, pBuffer.get(), AssetSize, IsReadFromAssetManager);
    CFileUtils::closeFile(pAsset, IsReadFromAssetManager);
    if (Flag < 0)
        return nullptr;

    double StartTime = CTimeUtils::getCurrentTime();
    int Channels;
    unsigned char *pImageData = nullptr;

    if (vPictureType == EPictureType::PNG || vPictureType == EPictureType::JPG)
    {
        pImageData = stbi_load_from_memory(pBuffer.get(), static_cast<int>(AssetSize), &voWidth, &voHeight, &Channels, 0);
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
    else if (vPictureType == EPictureType::KTX2)
    {
        ktxTexture2 *pTexture = nullptr;
        KTX_error_code Result = ktxTexture2_CreateFromMemory(
            pBuffer.get(),
            AssetSize,
            KTX_TEXTURE_CREATE_NO_FLAGS,
            &pTexture);

        if (Result != KTX_SUCCESS)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG,
                      "Failed to load KTX2 pTexture from memory. Error code: %d", Result);
            return nullptr;
        }
        if (ktxTexture_NeedsTranscoding(ktxTexture(pTexture)))
        {
            Result = ktxTexture2_TranscodeBasis(pTexture, KTX_TTF_ETC2_RGBA, 0);
            if (Result != KTX_SUCCESS)
            {
                LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG,
                          "Failed to transcode KTX2 pTexture. Error code: %d", Result);
                ktxTexture_Destroy(ktxTexture(pTexture));
                return nullptr;
            }
        }

        GLuint TextureHandle = 0;
        GLenum Target = 0;
        GLenum GlError = GL_NO_ERROR;

        KTX_error_code GlUploadResult = ktxTexture_GLUpload(
            reinterpret_cast<ktxTexture *>(pTexture),
            &TextureHandle,
            &Target,
            &GlError);

        glBindTexture(GL_TEXTURE_2D, TextureHandle);
/*        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // 重要！
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (GlUploadResult != KTX_SUCCESS || GlError != GL_NO_ERROR)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG,
                      "ktxTexture_GLUpload failed. Error: %d, GL Error: 0x%x",
                      GlUploadResult, GlError);
            ktxTexture_Destroy(reinterpret_cast<ktxTexture *>(pTexture));
            return nullptr;
        }

        voWidth = pTexture->baseWidth;
        voHeight = pTexture->baseHeight;

        LOG_INFO(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG,
                 "Successfully loaded [%s] KTX pTexture. Width: %d, Height: %d, Target: 0x%x",
                 vTexturePath.c_str(), voWidth, voHeight, Target);

        ktxTexture_Destroy(reinterpret_cast<ktxTexture *>(pTexture));
        return new CTexture2D(TextureHandle);
    }
    else if (vPictureType == EPictureType::PKM)
    {
        const char *pExtensions = (const char *)glGetString(GL_EXTENSIONS);
        if (!strstr(pExtensions, "GL_OES_compressed_ETC1_RGB8_texture"))
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Device does NOT support ETC1 compression!");
            return nullptr;
        }
        else
        {
            LOG_INFO(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Supported GL extensions: %s", pExtensions);
        }

        if (AssetSize < 16)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Invalid ETC1 file: too small");
            return nullptr;
        }

        const uint8_t *pHeader = pBuffer.get();
        if (memcmp(pHeader, "PKM ", 4) != 0 && memcmp(pHeader, "PKM 10", 6) != 0)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Invalid ETC1 file: bad magic number");
            return nullptr;
        }

        voWidth = (pHeader[12] << 8) | pHeader[13];
        voHeight = (pHeader[14] << 8) | pHeader[15];

        GLuint TextureHandle;
        glGenTextures(1, &TextureHandle);
        glBindTexture(GL_TEXTURE_2D, TextureHandle);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glCompressedTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_ETC1_RGB8_OES,
            voWidth,
            voHeight,
            0,
            AssetSize - 16,
            pBuffer.get() + 16);

        int TexCompressed = 0;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &TexCompressed);
        if (!TexCompressed)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Texture is not compressed as expected.");
        }

        GLenum Error = glGetError();
        if (Error != GL_NO_ERROR)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Failed to upload ETC1 texture (GL error: 0x%x)", Error);
            glDeleteTextures(1, &TextureHandle);
            return nullptr;
        }

        return new CTexture2D(TextureHandle);
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
    if (Channels == 3)
        Format = GL_RGB;
    else if (Channels == 4)
        Format = GL_RGBA;
    else if (Channels == 1)
        Format = GL_RED;

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
                const uint8_t *src = pImageData + (y * voWidth + x) * 4;

                // 上半部分（R和Alpha通道）
                uint8_t upperR = src[0];
                uint8_t upperA = src[1];

                // 下半部分（B和Alpha通道）
                uint8_t lowerR = src[2];
                uint8_t lowerA = src[3];

                // 填充目标像素
                uint8_t *upperDst = OrigPixels.data() + ((y * voWidth) + x) * 4;
                upperDst[0] = upperR; // R
                upperDst[1] = upperR; // G
                upperDst[2] = upperR; // B
                upperDst[3] = upperA; // A

                uint8_t *lowerDst = OrigPixels.data() + (((y + compHeight) * voWidth) + x) * 4;
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

CTexture2D *CTexture2D::loadTextureFromPNG8(const std::string &vTexturePath)
{
    std::unique_ptr<unsigned char[]> pBuffer;
    size_t AssetSize;
    bool IsReadFromAssetManager = true;
    auto pAsset = CFileUtils::openFile(vTexturePath.c_str(), IsReadFromAssetManager);
    if (!pAsset)
    {
        IsReadFromAssetManager = false;
        pAsset = CFileUtils::openFile(vTexturePath.c_str(), IsReadFromAssetManager);
    }

    AssetSize = CFileUtils::getFileBytes(pAsset, IsReadFromAssetManager);
    pBuffer = std::make_unique<unsigned char[]>(AssetSize);
    int Flag = CFileUtils::readFile<unsigned char>(pAsset, pBuffer.get(), AssetSize, IsReadFromAssetManager);
    CFileUtils::closeFile(pAsset, IsReadFromAssetManager);
    if (Flag < 0)
        return nullptr;

    double StartTime = CTimeUtils::getCurrentTime();
    unsigned char *pImageData = nullptr;

    std::vector<unsigned char> Image;
    unsigned Width, Height;
    LodePNGState State;
    lodepng_state_init(&State);
    State.info_raw.colortype = LCT_RGBA;
    State.info_raw.bitdepth = 8;

    unsigned Error = lodepng_decode(&pImageData, &Width, &Height, &State, pBuffer.get(), AssetSize);
    if (Error)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Failed to create png8 texture: %s", vTexturePath.c_str());
        return nullptr;
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

    GLint Format = GL_RGBA;
    StartTime = CTimeUtils::getCurrentTime();
    GLuint TextureHandle = 0;
    TextureHandle = __createHandle(Format, Width, Height, pImageData);
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
    return new CTexture2D(TextureHandle);
}

CTexture2D *CTexture2D::createEmptyTexture(int vWidth, int vHeight, int vChannels)
{
    GLint Format = GL_RGB;
    if (vChannels == 3)
        Format = GL_RGB;
    else if (vChannels == 4)
        Format = GL_RGBA;
    else if (vChannels == 1)
        Format = GL_RED;
    else
        LOG_WARN(hiveVG::TAG_KEYWORD::TEXTURE2D_TAG, "Channel Count is invalid, set default format [GL_RGB].");

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
