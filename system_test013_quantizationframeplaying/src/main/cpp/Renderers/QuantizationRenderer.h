#pragma once

#include <EGL/egl.h>
#include "Common.h"

namespace hiveVG
{
    class CTexture2D;
    class CShaderProgram;
    class CScreenQuad;
    class CSingleTexturePlayer;

    class CQuantizationRenderer
    {
    public:
        explicit CQuantizationRenderer();
        ~CQuantizationRenderer();

        void renderScene();
        bool initTextureAndShaderProgram();

    private:
        template<typename T>
        void __deleteSafely(T*& vPointer);

        int                   m_OneTextureFrames        = 1;
        int                   m_CurrentChannel          = 0;
        int                   m_TextureCount            = 0;
        int                   m_PreloadTexture          = 5;
        int                   m_CurrentTexture          = 0;
        float                 m_FramePerSecond          = 24.0f;
        double                m_LastFrameTime           = 0.0f;
        double                m_CurrentTime             = 0.0f;
        double                m_AccumFrameTime          = 0.0f;
        std::string           m_TexPath;
        CScreenQuad*          m_pScreenQuad             = nullptr;
        CShaderProgram*       m_pSequenceShaderProgram  = nullptr;
        CSingleTexturePlayer* m_pBackgroundPlayer       = nullptr;
        std::vector<CTexture2D*> m_SeqTextures;
    };

    template<typename T>
    void CQuantizationRenderer::__deleteSafely(T*& vPointer)
    {
        if (vPointer != nullptr)
        {
            delete vPointer;
            vPointer = nullptr;
        }
    }
}