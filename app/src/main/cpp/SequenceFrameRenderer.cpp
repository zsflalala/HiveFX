#include "SequenceFrameRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <android/asset_manager.h>
#include "Common.h"
#include "ScreenQuad.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"
#include "SequencePlayerManager.h"

using namespace hiveVG;

CSequenceFrameRenderer::CSequenceFrameRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initRenderer();
    __initAlgorithm();
}

CSequenceFrameRenderer::~CSequenceFrameRenderer()
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
}

void CSequenceFrameRenderer::__initRenderer()
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

void CSequenceFrameRenderer::__initAlgorithm()
{
    m_pScreenQuad = CScreenQuad::getOrCreate();

    std::string TexPath = "Textures/Clouds/cloud1.png";
    CSequenceFramePlayer CloudSequencePlayer0(TexPath, 32, 16);
    if (!CloudSequencePlayer0.initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    }

    TexPath = "Textures/Clouds/cloud2.png";
    CSequenceFramePlayer CloudSequencePlayer1(TexPath, 21, 15);
    if (!CloudSequencePlayer1.initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    }

    TexPath = "Textures/Clouds/cloud3.png";
    CSequenceFramePlayer CloudSequencePlayer2(TexPath, 31, 16);
    if (!CloudSequencePlayer2.initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    }

    TexPath = "Textures/Clouds/cloud4.png";
    CSequenceFramePlayer CloudSequencePlayer3(TexPath, 28, 12);
    if (!CloudSequencePlayer3.initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    }

    TexPath = "Textures/Clouds/cloud5.png";
    CSequenceFramePlayer CloudSequencePlayer4(TexPath, 10, 12);
    if (!CloudSequencePlayer4.initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    }

    m_pSequencePlayerManager = std::make_unique<CSequencePlayerManager>();
    m_pSequencePlayerManager->pushBack(CloudSequencePlayer0);
    m_pSequencePlayerManager->pushBack(CloudSequencePlayer1);
    m_pSequencePlayerManager->pushBack(CloudSequencePlayer2);
    m_pSequencePlayerManager->pushBack(CloudSequencePlayer3);
    m_pSequencePlayerManager->pushBack(CloudSequencePlayer4);

    m_pSequencePlayerManager->initSequenceState();
    m_LastFrameTime = __getCurrentTime();
}

void CSequenceFrameRenderer::renderBlendingSnow()
{
    m_CurrentTime = __getCurrentTime();
    double Dt = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime = m_CurrentTime;

    glClearColor(0.1f,0.1f,0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);

    EGLint Width, Height;
    eglQuerySurface(m_Display, m_Surface, EGL_WIDTH, &Width);
    eglQuerySurface(m_Display, m_Surface, EGL_HEIGHT, &Height);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_pSequencePlayerManager->updateFrameAndUV(Width, Height, Dt);
    m_pSequencePlayerManager->updateSequenceState(Dt);
    m_pSequencePlayerManager->setPlayingSpeed(20.0f);
    static int PlayersNum = m_pSequencePlayerManager->getSequencePlayerLength();
    static std::vector<glm::vec2> ScreenUVScale(PlayersNum, glm::vec2(1.0f, 1.0f));

    for (int i = 0; i < PlayersNum; i++)
    {
        ScreenUVScale[i].y = ScreenUVScale[i].x / m_pSequencePlayerManager->getImageAspectRatioAt(i);
        m_pSequencePlayerManager->setImageAspectRatioAt(i, ScreenUVScale[i]);
    }
    m_pSequencePlayerManager->draw(m_pScreenQuad);

    auto SwapResult = eglSwapBuffers(m_Display, m_Surface);
    assert(SwapResult == EGL_TRUE);
}

double CSequenceFrameRenderer::__getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}