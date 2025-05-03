#pragma once

#include <EGL/egl.h>
#include "Common.h"

namespace hiveVG
{
    class CTexture2D;
    class CShaderProgram;
    class CScreenQuad;
    class CSequenceFramePlayer;

    class CSnowQuantizationSeqRenderer
    {
    public:
        CSnowQuantizationSeqRenderer();
        ~CSnowQuantizationSeqRenderer();

        void renderScene();

    private:
        bool __initAlgorithm();

        template<typename T>
        void __deleteSafely(T*& vPointer);

        int                   m_OneTextureFrames        = 1;
        int                   m_TextureCount            = 0;
        int                   m_CurrentTexture          = 0;
        float                 m_FramePerSecond          = 24.0f;
        double                m_LastFrameTime           = 0.0f;
        double                m_CurrentTime             = 0.0f;
        double                m_AccumFrameTime          = 0.0f;
        EPictureType::EPictureType m_PictureType        = EPictureType::EPictureType::PNG;
        std::string           m_TexPath;
        CScreenQuad*          m_pScreenQuad             = nullptr;
        CSequenceFramePlayer* m_pSequencePlayer         = nullptr;
        CSequenceFramePlayer* m_pBackgroundPlayer       = nullptr;
    };

    template<typename T>
    void CSnowQuantizationSeqRenderer::__deleteSafely(T*& vPointer)
    {
        if (vPointer != nullptr)
        {
            delete vPointer;
            vPointer = nullptr;
        }
    }
}