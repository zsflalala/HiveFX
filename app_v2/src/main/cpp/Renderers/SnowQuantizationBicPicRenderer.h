#pragma once

#include <EGL/egl.h>
#include <glm/glm.hpp>
#include "Common.h"

namespace hiveVG
{
    class CScreenQuad;
    class CTexture2D;
    class CShaderProgram;
    class CSingleTexturePlayer;
    class CSequenceFramePlayer;

    class CSnowQuantizationBicPicRenderer
    {
    public:
        CSnowQuantizationBicPicRenderer(std::string& vConfigPath);
        ~CSnowQuantizationBicPicRenderer();

        void renderScene();

    private:
        void   __initAlgorithm();

        template<typename T>
        void __deleteSafely(T*& vPointer);

        int                        m_SeqRows                 = 0;
        int                        m_SeqCols                 = 0;
        int                        m_CurrentFrame            = 0;
        int                        m_TextureCount            = 0;
        int                        m_OneTextureFrames        = 1;
        double                     m_LastFrameTime           = 0.0f;
        double                     m_CurrentTime             = 0.0f;
        double                     m_AccumFrameTime          = 0.0f;
        float                      m_FramePerSecond          = 24.0f;
        std::string                m_TexPath;
        std::string                m_ConfigPath;
        EPictureType::EPictureType m_PictureType             = EPictureType::PNG;
        EPlayMode::EPlayMode       m_PlayMode                = EPlayMode::DEPTH;
        CScreenQuad*               m_pScreenQuad             = nullptr;
        CSequenceFramePlayer*      m_pBackFramePlayer        = nullptr;
        CSingleTexturePlayer*      m_pForeFramePlayer        = nullptr;
    };

    template<typename T>
    void CSnowQuantizationBicPicRenderer::__deleteSafely(T*& vPointer)
    {
        if (vPointer != nullptr)
        {
            delete vPointer;
            vPointer = nullptr;
        }
    }
}