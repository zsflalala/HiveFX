#include "WeatherAsyncRenderer.h"
#include <thread>
#include <numeric>
#include <webp/decode.h>
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "Common.h"
#include "TimeUtils.h"
#include "stb_image.h"
#include "FileUtils.h"
#include "JsonReader.h"

using namespace hiveVG;

CWeatherAsyncRenderer::CWeatherAsyncRenderer() : m_ThreadPool(std::thread::hardware_concurrency())
{
}

CWeatherAsyncRenderer::~CWeatherAsyncRenderer()
{
    if (m_pScreenQuad != nullptr)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
    if (m_pAsyncShaderProgram)
    {
        delete m_pAsyncShaderProgram;
        m_pAsyncShaderProgram = nullptr;
    }
    if (m_pTextureHandles)
    {
        glDeleteTextures(m_TextureCount, m_pTextureHandles);
        delete[] m_pTextureHandles;
        m_pTextureHandles = nullptr;
    }
}

bool CWeatherAsyncRenderer::initTextureAndShaderProgram()
{
    std::string FileName   = "configs/WeatherAsyncConfig.json";
    CJsonReader JsonReader = CJsonReader(FileName);
    Json::Value WeatherConfig  = JsonReader.getObject("Weather");

    std::string VertexShader   = WeatherConfig["vertex_shader"].asString();
    std::string FragShader     = WeatherConfig["fragment_shader"].asString();
    std::string PicType        = WeatherConfig["texture_type"].asString();
    m_TextureRootPath          = WeatherConfig["texture_path"].asString();
    m_TextureCount             = WeatherConfig["texture_count"].asInt();

    if (!m_TextureRootPath.empty() && m_TextureRootPath.back() != '/')
        m_TextureRootPath += '/';
    m_TextureType = EPictureType::FromString(PicType);
    m_CPULoadedTime = CTimeUtils::getCurrentTime();
    std::string PictureSuffix;
    if (m_TextureType == EPictureType::PNG)
        PictureSuffix = ".png";
    else if (m_TextureType == EPictureType::WEBP)
        PictureSuffix = ".webp";

    m_pAsyncShaderProgram = CShaderProgram::createProgram(VertexShader,FragShader);
    assert(m_pAsyncShaderProgram != nullptr);

    m_pScreenQuad = CScreenQuad::getOrCreate();
    m_LoadedTextures = std::vector<STextureData>(m_TextureCount);
    m_FrameLoadedGPU = std::vector<std::atomic<bool>>(m_TextureCount);
    m_pTextureHandles = new unsigned int[m_TextureCount];
    glGenTextures(m_TextureCount, m_pTextureHandles);

    for (int i = 0; i < m_TextureCount; i++)
    {
        std::string TexturePath = m_TextureRootPath + "frame_" + std::string(3 - std::to_string(i + 1).length(), '0') + std::to_string(i + 1) + PictureSuffix;
        m_ThreadPool.enqueueTask([this, i, TexturePath]()
                                 { this->__loadTextureDataAsync(i, TexturePath); });
    }

    m_GPULoadedTime = CTimeUtils::getCurrentTime();
    return true;
}

void CWeatherAsyncRenderer::renderScene()
{
    glClearColor(0.345f,0.345f,0.345f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    if (!m_FramesToUploadGPU.empty())
    {
        std::vector<int> FramesToUpload;  // 用于存储待上传的帧
        {
            std::lock_guard<std::mutex> lock(m_LoadTextureToCPUMutex);  // 加锁访问
            FramesToUpload.assign(m_FramesToUploadGPU.begin(), m_FramesToUploadGPU.end());
            m_FramesToUploadGPU.clear();  // 清空待上传队列
        }

        for (int FrameToUpload : FramesToUpload)
        {
            __uploadTexturesToGPU(FrameToUpload);
        }
    }
    double CurrentTime = CTimeUtils::getCurrentTime();

    if (m_FrameLoadedGPU[m_Frame].load())
    {
        m_LastLoadedFrame = m_Frame;
        double FrameTime = 1.0 / m_FrameRate;
        if (CurrentTime - m_LastFrameTime >= FrameTime)
        {
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
                m_Frame = (m_Frame + 1) % m_TextureCount;
                m_LastFrameTime = CurrentTime;
                LOG_ERROR(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "Frame %d is not loaded for too long, skipping to next frame.", m_Frame);
            }
        }
        else
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "No frame is loaded yet.");
            return;
        }
    }

    m_pAsyncShaderProgram->useProgram();
    m_pAsyncShaderProgram->setUniform("sequenceTexture", 0);
    glBindTexture(GL_TEXTURE_2D, m_pTextureHandles[m_LastLoadedFrame]);
    glActiveTexture(GL_TEXTURE0);
    m_pScreenQuad->bindAndDraw();
}

void CWeatherAsyncRenderer::__loadTextureDataAsync(int vFrameIndex, const std::string &vTexturePath)
{
    auto pAsset = CFileUtils::openFile(vTexturePath.c_str());
    assert(pAsset);
    size_t AssetSize = CFileUtils::getFileBytes(pAsset);
    std::unique_ptr<unsigned char[]> pBuffer(new unsigned char[AssetSize]);
    int Flag = CFileUtils::readFile<unsigned char>(pAsset, pBuffer.get(), AssetSize);
    if(Flag < 0)
        return;
    CFileUtils::closeFile(pAsset);

    double StartTime = CTimeUtils::getCurrentTime();
    int Width, Height, Channels;
    uint8_t *pTexData = nullptr;

    if (m_TextureType == EPictureType::PNG)
    {
        std::lock_guard<std::mutex> lock(m_StbMutex);
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
        std::lock_guard<std::mutex> Lock(m_LoadTextureToCPUMutex);
        auto &Texture = m_LoadedTextures[vFrameIndex];
        Texture._ImageData.assign(pTexData, pTexData + (Width * Height * Channels));
        Texture._Width = Width;
        Texture._Height = Height;
        Texture._Channels = Channels;
        Texture._IsLoaded.store(true);
        m_FramesToUploadGPU.insert(vFrameIndex);

        double EndTime = CTimeUtils::getCurrentTime();
        double Duration = EndTime - StartTime;
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

void CWeatherAsyncRenderer::__uploadTexturesToGPU(int vTextureIndex)
{
    auto &Texture = m_LoadedTextures[vTextureIndex];
    if (Texture._IsLoaded.load())
    {
        std::lock_guard<std::mutex> Lock(m_LoadTextureToCPUMutex);
        glBindTexture(GL_TEXTURE_2D, m_pTextureHandles[vTextureIndex]);
        GLenum Format = (Texture._Channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, Format, Texture._Width, Texture._Height, 0, Format, GL_UNSIGNED_BYTE, Texture._ImageData.data());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glGenerateMipmap(GL_TEXTURE_2D);
        m_FrameLoadedGPU[vTextureIndex].store(true);
        Texture._ImageData.clear();
        Texture._ImageData.shrink_to_fit();
    }
    else
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "%d hasn't loaded yet.", vTextureIndex);
    }
}