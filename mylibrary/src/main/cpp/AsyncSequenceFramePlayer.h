#pragma once

#include <string>
#include <vector>
#include <set>
#include <future>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <functional>
#include <GLES3/gl3.h>
#include <android/asset_manager.h>
#include "Common.h"
#include "ThreadPool.h"

namespace hiveVG
{
    class CTexture2D;
    class CShaderProgram;
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
    struct TextureLoadTask {
        int index;
        std::string path;
        void *assetManager;
    };
    class CAsyncSequenceFramePlayer
    {
    public:
        CAsyncSequenceFramePlayer(const std::string& vTextureRootPath, int vTextureCount, EPictureType::EPictureType vPictureType = EPictureType::PNG);
        ~CAsyncSequenceFramePlayer();

        bool initTextureAndShaderProgram(AAssetManager* vAssetManager);
        void updateFrames();
        void setFrameRate(int vFrameRate) { m_FrameRate = vFrameRate; }
        void setLoopPlayback(bool vLoopTag)   { m_IsLoop = vLoopTag; }
        void setValidFrames(int vValidFrames)   { m_ValidFrames = vValidFrames; }

        [[nodiscard]] bool getFinishState() const { return m_IsFinished; }
        [[nodiscard]] bool getLoopState()   const { return m_IsLoop; }
        [[nodiscard]] int  getSingleTextureWidth() const  { return m_SequeceSingleTextureWidth; }
        [[nodiscard]] int  getSingleTextureHeight() const { return m_SequeceSingleTextureHeight; }
    private:
        void   __loadTextureDataAsync(AAssetManager *vAssetManager, int vFrameIndex, const std::string &vTexturePath, std::vector<STextureData> &vLoadedTextures, std::mutex &vTextureMutex, std::set<int> &vFramesToUploadGPU);
        void   __uploadTexturesToGPU(int vTextureIndex, std::vector<STextureData> &vLoadedTextures, unsigned int *vTextureHandles, std::vector<std::atomic<bool>>& vFrameLoadedGPU);
        double __getCostTime(std::vector<double> &vCostTime);

        bool                                 m_IsLoop          = true;
        bool                                 m_IsFinished      = false;
        int                                  m_TextureCount;
        EPictureType::EPictureType           m_TextureType            = EPictureType::PNG;
        int                                  m_Frame                  = 0;
        int                                  m_LastLoadedFrame        = -1;
        int                                  m_FrameRate              = 60;
        int                                  m_ValidFrames;
        int				                     m_SequeceSingleTextureWidth;
        int				                     m_SequeceSingleTextureHeight;
        double                               m_LastFrameTime          = 0;
        double                               m_FrameLoadTimeThreshold = 0.1f;
        double                               m_CPULoadedTime;
        double                               m_GPULoadedTime;
        std::vector<double>                  m_CPUCostTime;
        std::vector<double>                  m_GPUCostTime;
        std::string                          m_TextureRootPath;
        std::mutex                           m_LoadTextureToCPUMutex;
        std::vector<STextureData>            m_LoadedTextures;
        std::vector<std::atomic<bool>>       m_FrameLoadedGPU;
        std::set<int>                        m_FramesToUploadGPU;
        unsigned int*	                     m_pTextureHandles      = nullptr;
        CShaderProgram*                      m_pAsyncShaderProgram  = nullptr;
        std::vector<std::future<void>>       m_TextureLoadFutures;
        ThreadPool                           m_ThreadPool;
        std::queue<TextureLoadTask>          textureLoadQueue;
        std::mutex                          queueMutex;
    };

}