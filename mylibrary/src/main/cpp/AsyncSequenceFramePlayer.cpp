#include "AsyncSequenceFramePlayer.h"
#include <thread>
#include <numeric>
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "Common.h"
#include "TimeUtils.h"
#include "stb_image.h"
#include "webp/decode.h"

using namespace hiveVG;

CAsyncSequenceFramePlayer::CAsyncSequenceFramePlayer(const std::string &vTextureRootPath,
                                                     int vTextureCount, EPictureType::EPictureType vPictureType) : m_TextureRootPath(vTextureRootPath), m_ValidFrames(vTextureCount), m_TextureCount(vTextureCount), m_TextureType(vPictureType),
                                                                                                                   m_ThreadPool(std::thread::hardware_concurrency())
{
    m_LoadedTextures = std::vector<STextureData>(vTextureCount);
    m_FrameLoadedGPU = std::vector<std::atomic<bool>>(vTextureCount);
    m_pTextureHandles = new unsigned int[vTextureCount];
    glGenTextures(m_TextureCount, m_pTextureHandles);
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
    m_CPULoadedTime = CTimeUtils::getCurrentTime();
    std::string PictureSuffix;
    if (m_TextureType == EPictureType::PNG)
        PictureSuffix = ".png";
    else if (m_TextureType == EPictureType::JPG)
        PictureSuffix = ".jpg";
    else if (m_TextureType == EPictureType::WEBP)
        PictureSuffix = ".webp";
    else if (m_TextureType == EPictureType::ASTC)
        PictureSuffix = ".astc";
    for (int i = 0; i < m_TextureCount; i++)
    {
        std::string TexturePath = m_TextureRootPath + "/frame_" + std::string(3 - std::to_string(i + 1).length(), '0') + std::to_string(i + 1) + PictureSuffix;
        m_ThreadPool.enqueueTask([this, vAssetManager, i, TexturePath]()
                                 { this->__loadTextureDataAsync(vAssetManager, i, TexturePath, m_LoadedTextures, m_LoadTextureToCPUMutex, m_FramesToUploadGPU); });
    }

    m_pAsyncShaderProgram = CShaderProgram::createProgram(
        vAssetManager,
        SingleTexPlayVert,
        SingleTexPlayFrag);
    assert(m_pAsyncShaderProgram != nullptr);

    m_GPULoadedTime = CTimeUtils::getCurrentTime();
    return true;
}

void CAsyncSequenceFramePlayer::updateFrames()
{
    if (!m_FramesToUploadGPU.empty())
    {
        int FrameToUpload = *m_FramesToUploadGPU.begin();
        m_FramesToUploadGPU.erase(m_FramesToUploadGPU.begin());
        __uploadTexturesToGPU(FrameToUpload, m_LoadedTextures, m_pTextureHandles, m_FrameLoadedGPU);
    }
    double CurrentTime = CTimeUtils::getCurrentTime();
    if (m_CPUCostTime.size() == m_TextureCount)
    {
        double CPUAverageTime = __getCostTime(m_CPUCostTime);
        LOG_INFO(TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "Loading all textures in CPU costs average time : %f, costs sum time : %f, actually costs time : %f", CPUAverageTime, CPUAverageTime * m_TextureCount, CurrentTime - m_CPULoadedTime);
        m_CPUCostTime.clear();
    }
    if (m_GPUCostTime.size() == m_TextureCount)
    {
        double GPUAverageTime = __getCostTime(m_GPUCostTime);
        LOG_INFO(TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "Loading all textures in GPU costs average time : %f and costs sum time : %f, actually costs time : %f", GPUAverageTime, GPUAverageTime * m_TextureCount, CurrentTime - m_GPULoadedTime);
        m_GPUCostTime.clear();
    }

    if (m_FrameLoadedGPU[m_Frame].load())
    {
        m_LastLoadedFrame = m_Frame;
        double FrameTime = 1.0 / m_FrameRate;
        if (CurrentTime - m_LastFrameTime >= FrameTime)
        {
            if (!m_IsLoop && m_Frame == m_ValidFrames - 1)
            {
                m_IsFinished = true;
                return;
            }
            m_Frame = (m_Frame + 1) % m_TextureCount;
            m_LastFrameTime = CurrentTime;
        }
    }
    else
    {
        double TimeElapsed = CurrentTime - m_LastFrameTime;
        // If the threshold is exceeded, skip the current frame
        if (TimeElapsed > m_FrameLoadTimeThreshold)
        {
            double FrameTime = 1.0 / m_FrameRate;
            if (TimeElapsed >= FrameTime)
            {
                if (!m_IsLoop && m_Frame == m_ValidFrames - 1)
                {
                    m_IsFinished = true;
                    return;
                }
                m_Frame = (m_Frame + 1) % m_TextureCount;
                m_LastFrameTime = CurrentTime;
                LOG_ERROR(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "Frame %d is not loaded for too long, skipping to next frame.", m_Frame);
            }
        }
        else if (m_LastLoadedFrame != -1)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "Frame %d is not loaded, showing last loaded frame: %d.", m_Frame, m_LastLoadedFrame);
            return;
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

void CAsyncSequenceFramePlayer::__loadTextureDataAsync(AAssetManager *vAssetManager, int vFrameIndex,
                                                       const std::string &vTexturePath,
                                                       std::vector<STextureData> &vLoadedTextures,
                                                       std::mutex &vTextureMutex,
                                                       std::set<int> &vFramesToUploadGPU)
{
    if (!vAssetManager)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "AssetManager is null.");
        return;
    }
    AAsset *pAsset = AAssetManager_open(vAssetManager, vTexturePath.c_str(), AASSET_MODE_BUFFER);
    if (!pAsset)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "Failed to open asset: %s", vTexturePath.c_str());
        return;
    }
    size_t AssetSize = AAsset_getLength(pAsset);
    std::unique_ptr<unsigned char[]> pBuffer(new unsigned char[AssetSize]);
    AAsset_read(pAsset, pBuffer.get(), AssetSize);
    AAsset_close(pAsset);

    double StartTime = CTimeUtils::getCurrentTime();
    int Width, Height, Channels;
    uint8_t *pTexData = nullptr;

    if (m_TextureType == EPictureType::PNG)
    {
        pTexData = stbi_load_from_memory(pBuffer.get(), AssetSize, &Width, &Height, &Channels, 0);
    }
    else if (m_TextureType == EPictureType::WEBP)
    {
        WebPBitstreamFeatures Features;
        VP8StatusCode Status = WebPGetFeatures(pBuffer.get(), AssetSize, &Features);
        if (Status != VP8_STATUS_OK)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "Failed to get %s WebP features.", vTexturePath.c_str());
            return;
        }

        bool HasAlpha = Features.has_alpha;
        if (HasAlpha)
        {
            Channels = 4;
            pTexData = WebPDecodeRGBA(pBuffer.get(), AssetSize, &Width, &Height);
        }
        else
        {
            Channels = 3;
            pTexData = WebPDecodeRGB(pBuffer.get(), AssetSize, &Width, &Height);
        }
    }

    if (pTexData)
    {
        std::lock_guard<std::mutex> Lock(vTextureMutex);
        auto &Texture = vLoadedTextures[vFrameIndex];
        Texture._ImageData.assign(pTexData, pTexData + (Width * Height * Channels));
        Texture._Width = Width;
        Texture._Height = Height;
        Texture._Channels = Channels;
        Texture._IsLoaded.store(true);
        vFramesToUploadGPU.insert(vFrameIndex);
        double EndTime = CTimeUtils::getCurrentTime();
        double Duration = EndTime - StartTime;
        m_CPUCostTime.push_back(Duration);
        LOG_INFO(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "CPU load frame %d costs time: %f", vFrameIndex, Duration);
        if (m_TextureType == EPictureType::WEBP)
            WebPFree(pTexData);
        else if (m_TextureType == EPictureType::PNG)
            stbi_image_free(pTexData);
        else
            free(pTexData);
    }
    else
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "Failed to load texture: %s", vTexturePath.c_str());
    }
}

void CAsyncSequenceFramePlayer::__uploadTexturesToGPU(int vTextureIndex,
                                                      std::vector<STextureData> &vLoadedTextures,
                                                      unsigned int *vTextureHandles,
                                                      std::vector<std::atomic<bool>> &vFrameLoadedGPU)
{
    auto &Texture = vLoadedTextures[vTextureIndex];
    if (Texture._IsLoaded.load())
    {
        double StartTime = CTimeUtils::getCurrentTime();
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, vTextureHandles[vTextureIndex]);
        GLenum Format = (Texture._Channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, Format, Texture._Width, Texture._Height, 0, Format, GL_UNSIGNED_BYTE, Texture._ImageData.data());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glGenerateMipmap(GL_TEXTURE_2D);

        vFrameLoadedGPU[vTextureIndex].store(true);
        double EndTime = CTimeUtils::getCurrentTime();
        double Duration = EndTime - StartTime;
        m_GPUCostTime.push_back(Duration);
        LOG_INFO(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "GPU load frame %d costs time: %f", vTextureIndex, Duration);
    }
    else
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "%d hasn't loaded yet.", vTextureIndex);
    }
}

double CAsyncSequenceFramePlayer::__getCostTime(std::vector<double> &vCostTime)
{
    if (vCostTime.empty())
        return 0.0;
    double Sum = std::accumulate(vCostTime.begin(), vCostTime.end(), 0.0); // 计算总和
    double Average = Sum / vCostTime.size();                               // 计算平均值
    return Average;
}
