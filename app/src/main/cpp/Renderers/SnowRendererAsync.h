#pragma once

#include <EGL/egl.h>
#include "Common.h"

struct android_app;

namespace hiveVG
{
    class  CScreenQuad;
    class  CSingleTexturePlayer;
    class  CAsyncSequenceFramePlayer;

    class CSnowRendererAsync
    {
    public:
        CSnowRendererAsync(android_app *vApp);
        ~CSnowRendererAsync();

        void renderScene();
        void handleInput(ERenderType vInputType, bool vIsPointerDown);

    private:
        void __initAlgorithm();
        template<typename T>
        void __deleteSafely(T*& vPointer);

        int                         m_TextureCount         = 64;
        EPictureType::EPictureType  m_PictureType          = EPictureType::PNG;
        bool                        m_PreviousPointerState = false;
        bool                        m_EnableBigSnowFore    = false;
        bool                        m_EnableBigSnowBack    = false;
        bool                        m_EnableSmallSnowFore  = false;
        bool                        m_EnableSmallSnowBack  = false;
        android_app*                m_pApp                 = nullptr;
        CScreenQuad*                m_pScreenQuad          = nullptr;
        CAsyncSequenceFramePlayer*  m_pSmallSnowForePlayer = nullptr;
        CAsyncSequenceFramePlayer*  m_pSmallSnowBackPlayer = nullptr;
        CAsyncSequenceFramePlayer*  m_pBigSnowForePlayer   = nullptr;
        CAsyncSequenceFramePlayer*  m_pBigSnowBackPlayer   = nullptr;
        CSingleTexturePlayer*       m_pSingleFramePlayer   = nullptr;
    };

    template<typename T>
    void CSnowRendererAsync::__deleteSafely(T*& vPointer)
    {
        if (vPointer != nullptr)
        {
            delete vPointer;
            vPointer = nullptr;
        }
    }
}