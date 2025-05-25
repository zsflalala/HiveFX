#pragma once

#include <EGL/egl.h>
#include "Common.h"

namespace hiveVG
{
    class CTexture2D;
    class CShaderProgram;
    class CScreenQuad;
    class CSequenceFramePlayer;
    class CNightSceneSequencePlayer;

    class CRainMultiChannelSeqRenderer
    {
    public:
        CRainMultiChannelSeqRenderer();
        ~CRainMultiChannelSeqRenderer();

        void renderScene(ERenderChannel vRenderChannel);

    private:
        bool __initAlgorithm();

        template<typename T>
        void __deleteSafely(T*& vPointer);

        double                m_LastFrameTime            = 0.0f;
        double                m_CurrentTime              = 0.0f;
        double                m_AccumFrameTime           = 0.0f;
        CScreenQuad*          m_pScreenQuad              = nullptr;

        CSequenceFramePlayer* m_pLightingPlayer          = nullptr;
        CSequenceFramePlayer* m_pCloudPlayer             = nullptr;
        CSequenceFramePlayer* m_pSmallRaindropPlayer     = nullptr;
        CSequenceFramePlayer* m_pBigRaindropPlayer       = nullptr;

        CNightSceneSequencePlayer* m_pRainSeqPlayer           = nullptr;
    };

    template<typename T>
    void CRainMultiChannelSeqRenderer::__deleteSafely(T*& vPointer)
    {
        if (vPointer != nullptr)
        {
            delete vPointer;
            vPointer = nullptr;
        }
    }
}