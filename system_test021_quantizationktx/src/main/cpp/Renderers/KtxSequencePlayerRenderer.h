#pragma once

#include <EGL/egl.h>
#include <glm/glm.hpp>
#include "Common.h"

struct android_app;

namespace hiveVG
{
    class CScreenQuad;
    class CSingleTexturePlayer;
    class CSequenceFramePlayer;

    class CASTCSequencePlayerRenderer
    {
    public:
        CASTCSequencePlayerRenderer();
        ~CASTCSequencePlayerRenderer();

        void renderScene(int vWindowWidth, int vWindowHeight);

    private:
        void   __initAlgorithm();

        template<typename T>
        void __deleteSafely(T*& vPointer);

        double                     m_LastFrameTime        = 0.0f;
        double                     m_CurrentTime          = 0.0f;
        glm::vec2                  m_UVOffset             = glm::vec2(0.0f, 0.0f);
        EPlayType::EPlayType       m_PlayMode             = EPlayType::FULLSCREEN;
        EPictureType::EPictureType m_PictureType          = EPictureType::PNG;
        float                      m_PlayScale            = 1.0f;
        CScreenQuad*               m_pScreenQuad          = nullptr;
        CSequenceFramePlayer*      m_pTestPlayer          = nullptr;
        CSingleTexturePlayer*      m_pSingleFramePlayer   = nullptr;
    };

    template<typename T>
    void CASTCSequencePlayerRenderer::__deleteSafely(T*& vPointer)
    {
        if (vPointer != nullptr)
        {
            delete vPointer;
            vPointer = nullptr;
        }
    }
}