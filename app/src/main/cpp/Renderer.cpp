#include "Renderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <GLES3/gl3.h>
#include <cassert>
#include <algorithm>
#include "Renderers/SnowRenderer.h"
#include "Renderers/RainRendererAsync.h"
#include "Renderers/SnowRendererAsync.h"
#include "Renderers/CloudRendererBillBoard.h"
#include "Renderers/SnowSceneRenderer.h"
#include "Renderers/RainRenderer.h"
#include "Common.h"

using namespace hiveVG;

CRenderer::CRenderer(android_app *vApp) : m_pApp(vApp)
{
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
    if (m_pRainAsync)
        delete m_pRainAsync;
    if (m_pSnow)
        delete m_pSnow;
    if (m_pCloudScene)
        delete m_pCloudScene;
}

void CRenderer::__initRenderer()
{
    constexpr EGLint Attributes[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 24,
        EGL_NONE};

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
            if (eglGetConfigAttrib(Display, Config, EGL_RED_SIZE, &Red) && eglGetConfigAttrib(Display, Config, EGL_GREEN_SIZE, &Green) && eglGetConfigAttrib(Display, Config, EGL_BLUE_SIZE, &Blue) && eglGetConfigAttrib(Display, Config, EGL_DEPTH_SIZE, &Depth))
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

void CRenderer::renderScene()
{
    __updateRenderArea();

    if (m_RenderType == ERenderType::SNOW)
    {
        /*if (m_pSnow == nullptr) m_pSnow = new CSnowRenderer(m_pApp);
        m_pSnow->handleInput(m_EnableRenderType, m_IsPointerDown);
        m_pSnow->renderScene(m_WindowWidth, m_WindowHeight);*/
        if (m_pSnowAsync == nullptr)
            m_pSnowAsync = new CSnowRendererAsync(m_pApp);
        m_pSnowAsync->handleInput(m_EnableRenderType, m_IsPointerDown);
        m_pSnowAsync->renderScene();
    }
    else if (m_RenderType == ERenderType::RAIN)
    {
        /* if (m_pRain == nullptr) m_pRain = new CRainRenderer(m_pApp);
         m_pRain->handleInput(m_EnableRenderType, m_IsPointerDown);
         m_pRain->renderScene(m_WindowWidth, m_WindowHeight);*/
        if (m_pRainAsync == nullptr)
            m_pRainAsync = new CRainRendererAsync(m_pApp);
        m_pRainAsync->handleInput(m_EnableRenderType, m_IsPointerDown);
        m_pRainAsync->renderScene();
    }
    else if (m_RenderType == ERenderType::CLOUD)
    {
        if (m_pCloudScene == nullptr)
            m_pCloudScene = new CCloudRendererBillBoard(m_pApp);
        m_pCloudScene->renderScene(m_WindowWidth, m_WindowHeight);
    }
    else if (m_RenderType == ERenderType::SNOW_SCENE)
    {
        if (m_pSnowScene == nullptr)
            m_pSnowScene = new CSnowSceneRenderer(m_pApp);
        m_pSnowScene->renderScene(m_WindowWidth, m_WindowHeight);
    }

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
        m_WindowWidth = Width;
        m_WindowHeight = Height;
        glViewport(0, 0, m_WindowWidth, m_WindowHeight);
    }
}

void CRenderer::handleInput()
{
    auto *pInputBuffer = android_app_swap_input_buffers(m_pApp);
    if (!pInputBuffer)
        return;

    for (auto i = 0; i < pInputBuffer->motionEventsCount; i++)
    {
        auto &MotionEvent = pInputBuffer->motionEvents[i];
        auto Action = MotionEvent.action;

        auto PointerIndex = (Action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

        auto &Pointer = MotionEvent.pointers[PointerIndex];
        auto PointerX = GameActivityPointerAxes_getX(&Pointer);
        auto PointerY = GameActivityPointerAxes_getY(&Pointer);

        switch (Action & AMOTION_EVENT_ACTION_MASK)
        {
        case AMOTION_EVENT_ACTION_DOWN:
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
            m_IsPointerDown = true;
            if (PointerY > m_WindowHeight / 2.0)
            {
                if (PointerX < m_WindowWidth / 3.0)
                {
                    m_RenderType = ERenderType::SNOW;
                    if (PointerY < m_WindowHeight * 3.0 / 4.0)
                    {
                        if (PointerX < m_WindowWidth / 6.0)
                        {
                            m_EnableRenderType = ERenderType::SMALL_SNOW_FORE;
                            LOG_INFO(TAG_KEYWORD::RENDERER_TAG, "小雪前景");
                        }
                        else
                        {
                            m_EnableRenderType = ERenderType::SMALL_SNOW_BACK;
                            LOG_INFO(TAG_KEYWORD::RENDERER_TAG, "小雪背景");
                        }
                    }
                    else
                    {
                        if (PointerX < m_WindowWidth / 6.0)
                        {
                            m_EnableRenderType = ERenderType::BIG_SNOW_FORE;
                            LOG_INFO(TAG_KEYWORD::RENDERER_TAG, "大雪前景");
                        }
                        else
                        {
                            m_EnableRenderType = ERenderType::BIG_SNOW_BACK;
                            LOG_INFO(TAG_KEYWORD::RENDERER_TAG, "大雪背景");
                        }
                    }
                }
                else if (PointerX < m_WindowWidth * 2.0 / 3.0)
                {
                    m_RenderType = ERenderType::RAIN;
                    if (PointerY < m_WindowHeight * 3.0 / 4.0)
                    {
                        if (PointerX < m_WindowWidth / 2.0)
                        {
                            m_EnableRenderType = ERenderType::SMALL_RAIN_FORE;
                            LOG_INFO(TAG_KEYWORD::RENDERER_TAG, "小雨前景");
                        }
                        else
                        {
                            m_EnableRenderType = ERenderType::SMALL_RAIN_BACK;
                            LOG_INFO(TAG_KEYWORD::RENDERER_TAG, "小雨背景");
                        }
                    }
                    else
                    {
                        if (PointerX < m_WindowWidth / 2.0)
                        {
                            m_EnableRenderType = ERenderType::BIG_RAIN_FORE;
                            LOG_INFO(TAG_KEYWORD::RENDERER_TAG, "大雨前景");
                        }
                        else
                        {
                            m_EnableRenderType = ERenderType::BIG_RAIN_BACK;
                            LOG_INFO(TAG_KEYWORD::RENDERER_TAG, "大雨背景");
                        }
                    }
                }
                else
                    m_RenderType = ERenderType::CLOUD;
            }
            else
                m_RenderType = ERenderType::SNOW_SCENE;
            LOG_INFO(hiveVG::TAG_KEYWORD::RENDERER_TAG, "Pointer(s): (%d, %f, %f) Pointer Down", Pointer.id, PointerX, PointerY);
            break;

        case AMOTION_EVENT_ACTION_CANCEL:
        case AMOTION_EVENT_ACTION_UP:
        case AMOTION_EVENT_ACTION_POINTER_UP:
            m_IsPointerDown = false;
            m_EnableRenderType = ERenderType::NONE;
            LOG_INFO(hiveVG::TAG_KEYWORD::RENDERER_TAG, "Pointer(s): (%d, %f, %f) Pointer Up", Pointer.id, PointerX, PointerY);
            break;

        case AMOTION_EVENT_ACTION_MOVE:
            for (auto Index = 0; Index < MotionEvent.pointerCount; Index++)
            {
                Pointer = MotionEvent.pointers[Index];
                PointerX = GameActivityPointerAxes_getX(&Pointer);
                PointerY = GameActivityPointerAxes_getY(&Pointer);
                LOG_INFO(hiveVG::TAG_KEYWORD::RENDERER_TAG, "Pointer(s): (%d, %f, %f) Pointer Move", Pointer.id, PointerX, PointerY);

                if (Index != (MotionEvent.pointerCount - 1))
                    LOG_INFO(hiveVG::TAG_KEYWORD::RENDERER_TAG, ",");
            }
            break;
        default:
            LOG_INFO(hiveVG::TAG_KEYWORD::RENDERER_TAG, "Unknown MotionEvent Action: %d", Action);
        }
    }
    android_app_clear_motion_events(pInputBuffer);
}
