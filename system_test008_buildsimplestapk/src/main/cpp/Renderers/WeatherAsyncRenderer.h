#pragma once

#include <string>
#include <vector>
#include <set>
#include <future>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <functional>
#include "Common.h"
#include "ThreadPool.h"

namespace hiveVG
{
    class CTexture2D;
    class CShaderProgram;
    class CScreenQuad;

    struct STextureData
    {
        std::vector<unsigned char> _ImageData;
        int _Width    = 0;
        int _Height   = 0;
        int _Channels = 0;
        int _Format   = 0;
        unsigned char* _Data = nullptr;
        std::atomic<bool> _IsLoaded { false };
    };

    class CWeatherAsyncRenderer
    {
    public:
        CWeatherAsyncRenderer();
        ~CWeatherAsyncRenderer();

        bool initTextureAndShaderProgram();
        void renderScene();

    private:
        void   __loadTextureDataAsync(int vFrameIndex, const std::string &vTexturePath);
        void   __uploadTexturesToGPU(int vTextureIndex);

        int                                  m_TextureCount{};
        EPictureType::EPictureType           m_TextureType            = EPictureType::PNG;
        int                                  m_Frame                  = 0;
        int                                  m_LastLoadedFrame        = -1;
        int                                  m_FrameRate              = 24;
        double                               m_LastFrameTime          = 0;
        double                               m_FrameLoadTimeThreshold = 0.1f;
        double                               m_CPULoadedTime          = 0;
        double                               m_GPULoadedTime          = 0;
        std::string                          m_TextureRootPath;
        std::mutex                           m_LoadTextureToCPUMutex;
        std::mutex                           m_StbMutex;
        std::vector<STextureData>            m_LoadedTextures;
        std::vector<std::atomic<bool>>       m_FrameLoadedGPU;
        std::set<int>                        m_FramesToUploadGPU;
        unsigned int*	                     m_pTextureHandles      = nullptr;
        CShaderProgram*                      m_pAsyncShaderProgram  = nullptr;
        CScreenQuad*                         m_pScreenQuad          = nullptr;
        std::vector<std::future<void>>       m_TextureLoadFutures;
        CThreadPool                          m_ThreadPool;
    };
}