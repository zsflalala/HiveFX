#pragma once

#include <EGL/egl.h>
#include "Common.h"

namespace hiveVG
{
    class CTexture2D;
    class CShaderProgram;
    class CScreenQuad;

    class CWeatherSeqRenderer
    {
    public:
        explicit CWeatherSeqRenderer();
        ~CWeatherSeqRenderer();

        void renderScene();
        bool initTextureAndShaderProgram();

    private:
        int                   m_CurrentTexture          = 0;
        float                 m_FramePerSecond          = 24.0f;
        double                m_LastFrameTime           = 0.0f;
        double                m_CurrentTime             = 0.0f;
        double                m_AccumFrameTime          = 0.0f;
        CScreenQuad*          m_pScreenQuad             = nullptr;
        CShaderProgram*       m_pSequenceShaderProgram  = nullptr;
        std::vector<CTexture2D*> m_SeqTextures;
    };
}
