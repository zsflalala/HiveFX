#include "Renderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <GLES3/gl3.h>
#include <cassert>
#include <algorithm>
#include "Common.h"
#include "Renders/CombinedBigRainFrameRenderer.h"
#include "Renders/CombinedSmallRainFrameRenderer.h"
#include "Renders/CombinedBigSnowFrameRenderer.h"
#include "Renders/CombinedSmallSnowFrameRenderer.h"
#include "Renders/CombinedSnowCoverFrameRenderer.h"
#include "Renders/CloudRendererBillBoard.h"

using namespace hiveVG;

CRenderer::CRenderer(android_app *vApp): m_pApp(vApp)
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
    if (m_pCombinedBigRainRender)         delete m_pCombinedBigRainRender;
    if (m_pCombinedSmallRainRender)       delete m_pCombinedSmallRainRender;
    if (m_pCombinedBigSnowRender)         delete m_pCombinedBigSnowRender;
    if (m_pCombinedSmallSnowRender)       delete m_pCombinedSmallSnowRender;
    if (m_pCombinedSnowCoverRender)       delete m_pCombinedSnowCoverRender;
    if (m_pCombinedCloudRender)           delete m_pCombinedCloudRender;
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
            EGL_NONE
    };

    auto Display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    eglInitialize(Display, nullptr, nullptr);

    EGLint NumConfigs;
    eglChooseConfig(Display, Attributes, nullptr, 0, &NumConfigs);

    std::unique_ptr<EGLConfig[]> pSupportedConfigs(new EGLConfig[NumConfigs]);
    eglChooseConfig(Display, Attributes, pSupportedConfigs.get(), NumConfigs, &NumConfigs);

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

    EGLint Format;
    eglGetConfigAttrib(Display, pConfig, EGL_NATIVE_VISUAL_ID, &Format);
    EGLSurface Surface = eglCreateWindowSurface(Display, pConfig, m_pApp->window, nullptr);

    EGLint ContextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
    EGLContext Context = eglCreateContext(Display, pConfig, nullptr, ContextAttribs);

    auto MadeCurrent = eglMakeCurrent(Display, Surface, Surface, Context);
    assert(MadeCurrent);

    m_Display = Display;
    m_Surface = Surface;
    m_Context = Context;
}

void CRenderer::renderScene()
{
    __updateRenderArea();
    //大雪
    if (m_pCombinedBigSnowRender == nullptr)
        m_pCombinedBigSnowRender = new CCombinedBigSnowFrameRenderer(m_pApp);
        m_pCombinedBigSnowRender->renderScene(m_WindowWidth, m_WindowHeight);
    //小雪
   /* if (m_pCombinedSmallSnowRender == nullptr)
        m_pCombinedSmallSnowRender = new CCombinedSmallSnowFrameRenderer(m_pApp);
    m_pCombinedSmallSnowRender->renderScene(m_WindowWidth, m_WindowHeight);*/
    //大雨
   /* if (m_pCombinedBigRainRender == nullptr)
        m_pCombinedBigRainRender = new CCombinedBigRainFrameRenderer(m_pApp);
    m_pCombinedBigRainRender->renderScene(m_WindowWidth, m_WindowHeight);*/
    //小雨
    /*if (m_pCombinedSmallRainRender == nullptr)
        m_pCombinedSmallRainRender = new CCombinedSmallRainFrameRenderer(m_pApp);
    m_pCombinedSmallRainRender->renderScene(m_WindowWidth, m_WindowHeight);*/
    //积雪
    /*if (m_pCombinedSnowCoverRender == nullptr)
        m_pCombinedSnowCoverRender = new CCombinedSnowCoverFrameRenderer(m_pApp);
    m_pCombinedSnowCoverRender->renderScene(m_WindowWidth, m_WindowHeight);*/
   //云
    /*if (m_pCombinedCloudRender == nullptr)
        m_pCombinedCloudRender = new CCloudRendererBillBoard(m_pApp);
    m_pCombinedCloudRender->renderScene(m_WindowWidth, m_WindowHeight);*/

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

                if (PointerX < m_WindowWidth / 2.0)
                {
                    m_RenderType = ERenderType::SNOW;
                    m_EnableRenderType = ERenderType::BIG_SNOW_BACK;
                    LOG_INFO(TAG_KEYWORD::RENDERER_TAG, "大雪");
                }
                else
                {
                    m_RenderType = ERenderType::RAIN;
                    m_EnableRenderType = ERenderType::BIG_RAIN_BACK;
                    LOG_INFO(TAG_KEYWORD::RENDERER_TAG, "大雨");
                }
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