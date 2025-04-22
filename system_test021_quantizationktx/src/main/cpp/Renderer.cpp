#include "Renderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <EGL/egl.h>
#include <GLES3/gl32.h>
#include <cassert>
#include <algorithm>
#include "Renderers/KtxSequencePlayerRenderer.h"
#include "Renderers/QuantizationBicPicRenderer.h"
#include "Renderers/QuantizationSeqRenderer.h"
#include "Renderers/ScrollRainCompressedRenderer.h"
#include "Common.h"

using namespace hiveVG;

CRenderer::CRenderer(android_app *vApp): m_pApp(vApp)
{
    CAppContext::setAssetManager(vApp->activity->assetManager);
    __initRenderer();
}

CRenderer::~CRenderer()
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
    if (m_pTestRenderer)                    delete m_pTestRenderer;
    if (m_pQuantizationBigPicRenderer)      delete m_pQuantizationBigPicRenderer;
    if (m_pQuantizationSeqRenderer)         delete m_pQuantizationSeqRenderer;
    if (m_pSCrollRainRenderer)              delete m_pSCrollRainRenderer;
}

void CRenderer::__initRenderer()
{
    constexpr EGLint Attributes[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
            EGL_BLUE_SIZE,      8,
            EGL_GREEN_SIZE,     8,
            EGL_RED_SIZE,       8,
            EGL_ALPHA_SIZE,     8,
            EGL_DEPTH_SIZE,     24,
            EGL_STENCIL_SIZE,   8,
            EGL_NONE
    };

    m_Display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (m_Display == EGL_NO_DISPLAY) {
        LOG_ERROR(hiveVG::TAG_KEYWORD::RENDERER_TAG, "Failed to get EGL display");
        return;
    }

    if (!eglInitialize(m_Display, nullptr, nullptr)) {
        LOG_ERROR(hiveVG::TAG_KEYWORD::RENDERER_TAG, "EGL initialization failed");
        return;
    }

    EGLint NumConfigs = 0;
    eglChooseConfig(m_Display, Attributes, nullptr, 0, &NumConfigs);
    std::unique_ptr<EGLConfig[]> pSupportedConfigs(new EGLConfig[NumConfigs]);
    eglChooseConfig(m_Display, Attributes, pSupportedConfigs.get(), NumConfigs, &NumConfigs);

    auto pConfig = *std::find_if(
            pSupportedConfigs.get(),
            pSupportedConfigs.get() + NumConfigs,
            [this](const EGLConfig &Config) {
                EGLint Red, Green, Blue, Depth;
                if (eglGetConfigAttrib(m_Display, Config, EGL_RED_SIZE, &Red) &&
                    eglGetConfigAttrib(m_Display, Config, EGL_GREEN_SIZE, &Green) &&
                    eglGetConfigAttrib(m_Display, Config, EGL_BLUE_SIZE, &Blue) &&
                    eglGetConfigAttrib(m_Display, Config, EGL_DEPTH_SIZE, &Depth))
                {
                    LOG_INFO(hiveVG::TAG_KEYWORD::RENDERER_TAG,
                             "Config: R%d G%d B%d D%d", Red, Green, Blue, Depth);
                    return Red == 8 && Green == 8 && Blue == 8 && Depth == 24;
                }
                return false;
            });

    EGLint Format;
    eglGetConfigAttrib(m_Display, pConfig, EGL_NATIVE_VISUAL_ID, &Format);
    ANativeWindow_setBuffersGeometry(m_pApp->window, 0, 0, Format);
    m_Surface = eglCreateWindowSurface(m_Display, pConfig, m_pApp->window, nullptr);
    if (m_Surface == EGL_NO_SURFACE) {
        LOG_ERROR(hiveVG::TAG_KEYWORD::RENDERER_TAG, "Failed to create EGL surface");
        return;
    }

    const EGLint ContextAttribs[] = {
            EGL_CONTEXT_MAJOR_VERSION, 3,
            EGL_CONTEXT_MINOR_VERSION, 2,
            EGL_NONE
    };

    m_Context = eglCreateContext(m_Display, pConfig, nullptr, ContextAttribs);
    if (m_Context == EGL_NO_CONTEXT) {
        LOG_ERROR(hiveVG::TAG_KEYWORD::RENDERER_TAG,
                  "OpenGL ES 3.2 is NOT supported on this device");
        return;
    }

    if (!eglMakeCurrent(m_Display, m_Surface, m_Surface, m_Context)) {
        LOG_ERROR(hiveVG::TAG_KEYWORD::RENDERER_TAG, "Failed to make context current");
        return;
    }

    const char* glVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    LOG_INFO(hiveVG::TAG_KEYWORD::RENDERER_TAG,
             "Successfully initialized OpenGL ES %s", glVersion);
}

void CRenderer::renderScene()
{
    __updateRenderArea();

 /*   if (m_pTestRenderer == nullptr) m_pTestRenderer = new CASTCSequenceRendererRenderer(m_pApp);
    m_pTestRenderer->renderScene(m_WindowWidth, m_WindowHeight);*/

  /*  if (m_pQuantizationBigPicRenderer == nullptr)
        m_pQuantizationBigPicRenderer = new CQuantizationBicPicRenderer();
    m_pQuantizationBigPicRenderer->renderScene();*/
  /*  if (m_pQuantizationSeqRenderer == nullptr)
        m_pQuantizationSeqRenderer = new CQuantizationSeqRenderer();
    m_pQuantizationSeqRenderer->renderScene();*/

    if(!m_pSCrollRainRenderer) m_pSCrollRainRenderer = new CScrollRainCompressedRenderer(m_pApp);
    m_pSCrollRainRenderer->render();

    auto SwapResult = eglSwapBuffers(m_Display, m_Surface);
    assert(SwapResult == EGL_TRUE);
}

void CRenderer::__updateRenderArea()
{
    EGLint Width, Height;
    eglQuerySurface(m_Display, m_Surface, EGL_WIDTH, &Width);
    eglQuerySurface(m_Display, m_Surface, EGL_HEIGHT, &Height);

    if (Width != m_WindowWidth || Height != m_WindowHeight)
    {
        m_WindowWidth  = Width;
        m_WindowHeight = Height;
        glViewport(0, 0, m_WindowWidth, m_WindowHeight);
    }
}