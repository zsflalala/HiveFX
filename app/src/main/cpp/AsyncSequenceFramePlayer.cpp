#include "AsyncSequenceFramePlayer.h"
#include <thread>
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "Common.h"
#include "stb_image.h"

using namespace hiveVG;

CAsyncSequenceFramePlayer::CAsyncSequenceFramePlayer(const std::string &vTextureRootPath,
                                                     int vTextureCount):m_TextureRootPath(vTextureRootPath), m_TextureCount(vTextureCount)
{
    m_LoadedTextures = std::vector<STextureData>(vTextureCount);
    m_FrameLoadedGPU = std::vector<std::atomic<bool>>(vTextureCount);
    m_pTextureHandles = new unsigned int [vTextureCount];
}

CAsyncSequenceFramePlayer::~CAsyncSequenceFramePlayer()
{
    if (m_pAsyncShaderProgram)
    {
        delete m_pAsyncShaderProgram;
        m_pAsyncShaderProgram = nullptr;
    }
    if (m_pTextureHandles)
    {
        delete m_pTextureHandles;
        m_pTextureHandles = nullptr;
    }
}

bool CAsyncSequenceFramePlayer::initTextureAndShaderProgram(AAssetManager *vAssetManager)
{
    for (int i = 0;i < m_TextureCount;i++)
    {
        std::string TexturePath = m_TextureRootPath + "/frame_" + std::string(3 - std::to_string(i + 1).length(), '0') + std::to_string(i + 1) + ".png";
        std::thread([this, i, TexturePath, vAssetManager]()
                    {
                        __loadTextureDataAsync(vAssetManager, i, TexturePath, m_LoadedTextures, m_TextureMutex, m_FramesToUploadGPU);
                    }).detach();
    }

    m_pAsyncShaderProgram = CShaderProgram::createProgram(
            vAssetManager,
            "shaders/singleTexturePlayer.vert",
            "shaders/singleTexturePlayer.frag"
    );
    assert(m_pAsyncShaderProgram != nullptr);

    glGenTextures(m_TextureCount, m_pTextureHandles);
    return true;
}

void CAsyncSequenceFramePlayer::updateFrames()
{
    if (!m_FramesToUploadGPU.empty())
    {
        int FrameToUpload = m_FramesToUploadGPU.front();
        m_FramesToUploadGPU.pop();
        __uploadTexturesToGPU(FrameToUpload, m_LoadedTextures, m_pTextureHandles, m_FrameLoadedGPU);
    }

    double Now = __getCurrentTime();
    if (m_FrameLoadedGPU[m_Frame].load())
    {
        m_LastLoadedFrame = m_Frame; // Update the last loaded frame
        m_Frame = (m_Frame + 1) % m_TextureCount;
        m_LastFrameTime = Now;       // Update last render time
    }
    else
    {
        double TimeElapsed = Now - m_LastFrameTime;
        // If the threshold is exceeded, skip the current frame
        if (TimeElapsed > m_FrameLoadTimeThreshold)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "Frame %d  is not loaded for too long, skipping to next frame.", m_Frame);
            m_Frame = (m_Frame + 1) % m_TextureCount;
            m_LastFrameTime = Now;
        }
        else if (m_LastLoadedFrame != -1)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "Frame %d is not loaded, showing last loaded frame: %d.", m_Frame, m_LastLoadedFrame);
        }
        else
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "No frame is loaded yet.");
            return;
        }
    }

    m_pAsyncShaderProgram->useProgram();
    m_pAsyncShaderProgram->setUniform("quadTexture", 0);
    glBindTexture(GL_TEXTURE_2D, m_pTextureHandles[m_LastLoadedFrame]);
    glActiveTexture(GL_TEXTURE0);
}

void
CAsyncSequenceFramePlayer::__loadTextureDataAsync(AAssetManager *vAssetManager, int vFrameIndex,
                                                  const std::string &vTexturePath,
                                                  std::vector<STextureData> &vLoadedTextures,
                                                  std::mutex &vTextureMutex,
                                                  std::queue<int> &vFramesToUploadGPU)
{
    if (!vAssetManager)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "AssetManager is null.");
        return;
    }
    AAsset* pAsset = AAssetManager_open(vAssetManager, vTexturePath.c_str(), AASSET_MODE_BUFFER);
    if (!pAsset)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "Failed to open asset: %s", vTexturePath.c_str());
        return;
    }
    size_t AssetSize = AAsset_getLength(pAsset);
    std::unique_ptr<unsigned char[]> pBuffer(new unsigned char[AssetSize]);
    AAsset_read(pAsset, pBuffer.get(), AssetSize);
    AAsset_close(pAsset);

    int Width, Height, Channels;
    unsigned char* pTexData = stbi_load_from_memory(pBuffer.get(), AssetSize, &Width, &Height, &Channels, 0);
    if (pTexData)
    {
        std::lock_guard<std::mutex> Lock(vTextureMutex);
        auto& Texture = vLoadedTextures[vFrameIndex];
        Texture._ImageData.assign(pTexData, pTexData + (Width * Height * Channels));
        Texture._Width  = Width;
        Texture._Height = Height;
        Texture._Channels = Channels;
        Texture._IsLoaded.store(true);
        stbi_image_free(pTexData);
        vFramesToUploadGPU.push(vFrameIndex);
    }
    else
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "Failed to load texture: %s", vTexturePath.c_str());
    }
}

void CAsyncSequenceFramePlayer::__uploadTexturesToGPU(int vTextureIndex,
                                                      std::vector<STextureData> &vLoadedTextures,
                                                      unsigned int *vTextureHandles,
                                                      std::vector<std::atomic<bool>> &vFrameLoadedCPU)
{
    auto& Texture = vLoadedTextures[vTextureIndex];
    if (Texture._IsLoaded.load())
    {
        glBindTexture(GL_TEXTURE_2D, vTextureHandles[vTextureIndex]);
        GLenum Format = (Texture._Channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, Format, Texture._Width, Texture._Height, 0, Format, GL_UNSIGNED_BYTE, Texture._ImageData.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glGenerateMipmap(GL_TEXTURE_2D);
        vFrameLoadedCPU[vTextureIndex].store(true);
    }
    else
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "%d hasn't loaded yet.", vTextureIndex);
    }
}

double CAsyncSequenceFramePlayer::__getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}