#pragma once

#include <EGL/egl.h>
#include <vector>
#include <queue>
#include <android/asset_manager.h>

struct android_app;

#define FRAME_COUNT 128

namespace hiveVG
{
    class CScreenQuad;
    class CSingleTexturePlayer;
    class CSequenceFramePlayer;
    class CShaderProgram;

    struct STextureData
    {
        std::vector<unsigned char> _ImageData;
        int _Width    = 0;
        int _Height   = 0;
        int _Channels = 0;
        std::atomic<bool> _IsLoaded { false };
    };

    class CAsyncSequenceFrameRenderer
    {
    public:
        CAsyncSequenceFrameRenderer(android_app *vApp);
        virtual ~CAsyncSequenceFrameRenderer();

        void renderScene();

    private:
        void   __initRenderer();
        void   __initAlgorithm();
        double __getCurrentTime();
        void   __updateRenderArea();
        void   __loadTextureDataAsync(AAssetManager *vAssetManager, int vFrameIndex, const std::string& vTexturePath, std::vector<STextureData>& vLoadedTextures, std::mutex& vTextureMutex, std::queue<int>& vFramesToUploadGPU);
        void   __uploadTexturesToGPU(int vTextureIndex, std::vector<STextureData>& vLoadedTextures, unsigned int* vTextureIDs, std::atomic<bool>* vFrameLoadedCPU);
        void   __recordTime(const double& vLastTime, const std::string& vProcessName);

        int                       m_WindowWidth      = -1;
        int                       m_WindowHeight     = -1;
        int                       m_Frame            = 0;
        int                       m_LastLoadedFrame  = 0;
        double                    m_LastFrameTime    = 0.0f;
        double                    m_Threshold        = 0.01f;
        std::mutex                m_TextureMutex;
        std::vector<STextureData> m_LoadedTextures{FRAME_COUNT};
        std::queue<int>           m_FramesToUploadGPU;
        std::atomic<bool>         m_pFrameLoadedGPU[FRAME_COUNT] = {false};
        unsigned int              m_pTextureIDs[FRAME_COUNT];
        android_app*              m_pApp                 = nullptr;
        EGLDisplay                m_Display              = EGL_NO_DISPLAY;
        EGLSurface                m_Surface              = EGL_NO_SURFACE;
        EGLContext                m_Context              = EGL_NO_CONTEXT;
        CScreenQuad*              m_pScreenQuad          = nullptr;
        CShaderProgram*           m_pSingleShaderProgram = nullptr;
    };
}