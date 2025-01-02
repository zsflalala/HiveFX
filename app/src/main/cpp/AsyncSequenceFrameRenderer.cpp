#include "AsyncSequenceFrameRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <android/asset_manager.h>
#include <atomic>
#include <thread>
#include <mutex>
#include <queue>
#include "Common.h"
#include "ScreenQuad.h"
#include "ShaderProgram.h"
#include "stb_image.h"

using namespace hiveVG;

CAsyncSequenceFrameRenderer::CAsyncSequenceFrameRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initRenderer();
    __initAlgorithm();
}

CAsyncSequenceFrameRenderer::~CAsyncSequenceFrameRenderer()
{
    if (m_Display != EGL_NO_DISPLAY)
    {
        eglMakeCurrent(m_Display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (m_Context != EGL_NO_CONTEXT)
        {
            eglDestroyContext(m_Display, m_Context);
            m_Context = EGL_NO_CONTEXT;
        }
        if (m_Surface != EGL_NO_SURFACE)
        {
            eglDestroySurface(m_Display, m_Surface);
            m_Surface = EGL_NO_SURFACE;
        }
        eglTerminate(m_Display);
        m_Display = EGL_NO_DISPLAY;
    }
    delete m_pScreenQuad;
}

void CAsyncSequenceFrameRenderer::__initRenderer()
{
    constexpr EGLint Attributes[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_BLUE_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_RED_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_NONE
    };

    // The default Display is probably what you want on Android
    auto Display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(Display, nullptr, nullptr);

    // figure out how many configs there are
    EGLint NumConfigs;
    eglChooseConfig(Display, Attributes, nullptr, 0, &NumConfigs);

    // get the list of configurations
    std::unique_ptr<EGLConfig[]> pSupportedConfigs(new EGLConfig[NumConfigs]);
    eglChooseConfig(Display, Attributes, pSupportedConfigs.get(), NumConfigs, &NumConfigs);

    // Find a pConfig we like.
    // Could likely just grab the first if we don't care about anything else in the pConfig.
    // Otherwise hook in your own heuristic
    auto pConfig = *std::find_if(
            pSupportedConfigs.get(),
            pSupportedConfigs.get() + NumConfigs,
            [&Display](const EGLConfig &Config)
            {
                EGLint Red, Green, Blue, Depth;
                if (eglGetConfigAttrib(Display, Config, EGL_RED_SIZE, &Red)
                    && eglGetConfigAttrib(Display, Config, EGL_GREEN_SIZE, &Green)
                    && eglGetConfigAttrib(Display, Config, EGL_BLUE_SIZE, &Blue)
                    && eglGetConfigAttrib(Display, Config, EGL_DEPTH_SIZE, &Depth))
                {

                    LOG_INFO(hiveVG::TAG_KEYWORD::RENDERER_TAG, "Found pConfig with Red: %d, Green: %d, Blue: %d, Depth: %d", Red, Green, Blue, Depth);
                    return Red == 8 && Green == 8 && Blue == 8 && Depth == 24;
                }
                return false;
            });

    LOG_INFO(hiveVG::TAG_KEYWORD::RENDERER_TAG, "Found %d configs", NumConfigs);

    // create the proper window Surface
    EGLint Format;
    eglGetConfigAttrib(Display, pConfig, EGL_NATIVE_VISUAL_ID, &Format);
    EGLSurface Surface = eglCreateWindowSurface(Display, pConfig, m_pApp->window, nullptr);

    // Create a GLES 3 Context
    EGLint ContextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    EGLContext Context = eglCreateContext(Display, pConfig, nullptr, ContextAttribs);

    // get some window metrics
    auto MadeCurrent = eglMakeCurrent(Display, Surface, Surface, Context);
    assert(MadeCurrent);

    m_Display = Display;
    m_Surface = Surface;
    m_Context = Context;
}

void CAsyncSequenceFrameRenderer::__initAlgorithm()
{
    m_pScreenQuad = CScreenQuad::getOrCreate();
    m_pSingleShaderProgram = CShaderProgram::createProgram(
            m_pApp->activity->assetManager,
            "shaders/singleTexturePlayer.vert",
            "shaders/singleTexturePlayer.frag"
    );
    double start = __getCurrentTime();
    glGenTextures(FRAME_COUNT, m_pTextureIDs);
    __recordTime(start, "glGenTextures");

    for (int i = 0;i < FRAME_COUNT;i++)
    {
        std::string TexturePath = "Textures/Cloud2Scene/cloud2_frame_" + std::string(3 - std::to_string(i + 1).length(), '0') + std::to_string(i + 1) + ".png";
        std::thread([this, i, TexturePath]()
                    {
                        __loadTextureDataAsync(m_pApp->activity->assetManager, i, TexturePath, m_LoadedTextures, m_TextureMutex, m_FramesToUploadGPU);
                    }).detach();
    }
}

void CAsyncSequenceFrameRenderer::renderScene()
{
    glClearColor(0.1f,0.1f,0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    __updateRenderArea();

    if (!m_FramesToUploadGPU.empty())
    {
        int FrameToUpload = m_FramesToUploadGPU.front();
        m_FramesToUploadGPU.pop();
        __uploadTexturesToGPU(FrameToUpload, m_LoadedTextures, m_pTextureIDs, m_pFrameLoadedGPU);
    }

// If the current frame is not loaded, determine whether the time exceeds the threshold
    double Now = __getCurrentTime();
    if (m_pFrameLoadedGPU[m_Frame].load())
    {
        m_LastLoadedFrame = m_Frame; // Update the last loaded frame
        m_Frame = (m_Frame + 1) % FRAME_COUNT;
        m_LastFrameTime = Now;       // Update last render time
    }
    else
    {
        double TimeElapsed = Now - m_LastFrameTime;
        // If the threshold is exceeded, skip the current frame
        if (TimeElapsed > m_Threshold)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "Frame %d  is not loaded for too long, skipping to next frame.", m_Frame);
            m_Frame = (m_Frame + 1) % FRAME_COUNT;
            m_LastFrameTime = Now;
        }
        else if (m_LastLoadedFrame != -1)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "Frame %d is not loaded, showing last loaded frame: %d.", m_Frame, m_LastLoadedFrame);
        }
        else
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "No frame is loaded yet.");
            auto SwapResult = eglSwapBuffers(m_Display, m_Surface);
            assert(SwapResult == EGL_TRUE);
            return;
        }
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_pSingleShaderProgram->useProgram();
    m_pSingleShaderProgram->setUniform("quadTexture", 0);
    glBindTexture(GL_TEXTURE_2D, m_pTextureIDs[m_LastLoadedFrame]);
    glActiveTexture(GL_TEXTURE0);
    m_pScreenQuad->bindAndDraw();

    auto SwapResult = eglSwapBuffers(m_Display, m_Surface);
    assert(SwapResult == EGL_TRUE);
}

double CAsyncSequenceFrameRenderer::__getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void CAsyncSequenceFrameRenderer::__updateRenderArea()
{
    EGLint Width, Height;
    eglQuerySurface(m_Display, m_Surface, EGL_WIDTH, &Width);
    eglQuerySurface(m_Display, m_Surface, EGL_HEIGHT, &Height);

    if (Width != m_WindowWidth || Height != m_WindowHeight)
    {
        m_WindowWidth = Width;
        m_WindowHeight = Height;
        glViewport(0, 0, m_WindowWidth, m_WindowHeight);
    }
}

void
CAsyncSequenceFrameRenderer::__loadTextureDataAsync(AAssetManager *vAssetManager, int vFrameIndex, const std::string &vTexturePath,
                                                    std::vector<STextureData> &vLoadedTextures,
                                                    std::mutex &vTextureMutex,
                                                    std::queue<int> &vFramesToUploadGPU)
{
    double start = __getCurrentTime();
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
        __recordTime(start, "CPU load frame costs time: ");
    }
    else
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG, "Failed to load texture: %s", vTexturePath.c_str());
    }
}

void CAsyncSequenceFrameRenderer::__uploadTexturesToGPU(int vTextureIndex,
                                                        std::vector<STextureData> &vLoadedTextures,
                                                        unsigned int *vTextureIDs,
                                                        std::atomic<bool> *vFrameLoadedCPU)
{
    double start = __getCurrentTime();
    auto& Texture = vLoadedTextures[vTextureIndex];
    if (Texture._IsLoaded.load())
    {
        glBindTexture(GL_TEXTURE_2D, vTextureIDs[vTextureIndex]);
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
    __recordTime(start, "GPU Upload");
}

void
CAsyncSequenceFrameRenderer::__recordTime(const double &vLastTime, const std::string &vProcessName)
{
    auto CurrentTime = __getCurrentTime();
    double Duration = CurrentTime - vLastTime;
    LOG_INFO(hiveVG::TAG_KEYWORD::ASYNC_SEQFRAME_PALYER_TAG,"%s cost time: %f", vProcessName.c_str(), Duration);
}