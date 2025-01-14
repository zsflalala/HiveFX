#pragma once

#include <string>
#include <vector>
#include <queue>
#include <GLES3/gl3.h>
#include <android/asset_manager.h>

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
        std::atomic<bool> _IsLoaded { false };
    };

    class CAsyncSequenceFramePlayer
    {
    public:
        CAsyncSequenceFramePlayer(const std::string& vTextureRootPath, int vTextureCount);
        ~CAsyncSequenceFramePlayer();

        bool initTextureAndShaderProgram(AAssetManager* vAssetManager);
        void updateFrames();

    private:
        void   __loadTextureDataAsync(AAssetManager *vAssetManager, int vFrameIndex, const std::string &vTexturePath, std::vector<STextureData> &vLoadedTextures, std::mutex &vTextureMutex, std::queue<int> &vFramesToUploadGPU);
        void   __uploadTexturesToGPU(int vTextureIndex, std::vector<STextureData> &vLoadedTextures, unsigned int *vTextureHandles, std::vector<std::atomic<bool>>& vFrameLoadedCPU);
        double __getCurrentTime();
        double __getCostTime(std::vector<double> &vCostTime);

        int                                  m_TextureCount;
        int                                  m_Frame                  = 0;
        int                                  m_LastLoadedFrame        = -1;
        double                               m_LastFrameTime          = 0;
        double                               m_FrameLoadTimeThreshold = 0.1f;
        double                               m_CPULoadedTime;
        double                               m_GPULoadedTime;
        std::vector<double>                  m_CPUCostTime;
        std::vector<double>                  m_GPUCostTime;
        std::string                          m_TextureRootPath;
        std::mutex                           m_TextureMutex;
        std::vector<STextureData>            m_LoadedTextures;
        std::vector<std::atomic<bool>>       m_FrameLoadedGPU;
        std::queue<int>                      m_FramesToUploadGPU;

        unsigned int*	  m_pTextureHandles      = nullptr;
        CShaderProgram*   m_pAsyncShaderProgram  = nullptr;
    };
}