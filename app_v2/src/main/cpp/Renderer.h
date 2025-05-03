#pragma once

#include <EGL/egl.h>
#include "Common.h"

struct android_app;

namespace hiveVG
{
    class CSnowQuantizationSeqRenderer;
    class CSnowQuantizationBicPicRenderer;
    class CRainQuantizationSeqRenderer;
    class CRainQuantizationBicPicRenderer;
    class CScrollRainRenderer;

    class CRenderer
    {
    public:
        CRenderer(android_app *vApp);
        ~CRenderer();

        void renderScene();
        void handleInput();

    private:
        void __initRenderer();
        void __updateRenderArea();

        template<typename T>
        void __deleteSafely(T*& vPointer);

        ERenderType   m_RenderType       = ERenderType::SNOW_QBC;
        int           m_WindowWidth     = -1;
        int           m_WindowHeight    = -1;
        bool          m_IsPointerDown   = false;
        std::string   m_RainQBC         = "configs/RainQuantizationBigPicChannelConfig.json";
        std::string   m_RainQBD         = "configs/RainQuantizationBigPicDepthConfig.json";
        std::string   m_RainQ32         = "configs/RainQuantizationConfig.json";
        std::string   m_SnowQBC         = "configs/SnowQuantizationBigPicChannelConfig.json";
        std::string   m_SnowQBD         = "configs/SnowQuantizationBigPicDepthConfig.json";
        std::string   m_SnowQ32         = "configs/SnowQuantizationConfig.json";
        std::string   m_ScrollRain      = "configs/ScrollRainConfig.json";

        android_app*  m_pApp            = nullptr;
        EGLDisplay    m_Display         = EGL_NO_DISPLAY;
        EGLSurface    m_Surface         = EGL_NO_SURFACE;
        EGLContext    m_Context         = EGL_NO_CONTEXT;
        CSnowQuantizationBicPicRenderer*   m_pSnowQBCRenderer    = nullptr;
        CSnowQuantizationBicPicRenderer*   m_pSnowQBDRenderer    = nullptr;
        CSnowQuantizationSeqRenderer*      m_pSnowQ32Renderer    = nullptr;
        CRainQuantizationBicPicRenderer*   m_pRainQBCRenderer    = nullptr;
        CRainQuantizationBicPicRenderer*   m_pRainQBDRenderer    = nullptr;
        CRainQuantizationSeqRenderer*      m_pRainQ32Renderer    = nullptr;
        CScrollRainRenderer*               m_pScrollRainRenderer = nullptr;
    };

    template<typename T>
    void CRenderer::__deleteSafely(T*& vPointer)
    {
        if (vPointer != nullptr)
        {
            delete vPointer;
            vPointer = nullptr;
        }
    }
}