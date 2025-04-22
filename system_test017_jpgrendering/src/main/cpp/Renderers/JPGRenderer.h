#pragma once

#include <EGL/egl.h>
#include "Common.h"

namespace hiveVG
{
    class CScreenQuad;
    class CSingleTexturePlayer;
    class CSequenceFramePlayer;

    class CJPGRenderer
    {
    public:
        explicit CJPGRenderer();
        ~CJPGRenderer();

        void renderScene(int vWindowWidth, int vWindowHeight);

    private:
        void __initAlgorithm();
        template<typename T>
        void __deleteSafely(T*& vPointer);

        double                m_LastFrameTime           = 0.0f;
        double                m_CurrentTime             = 0.0f;
        CScreenQuad*          m_pScreenQuad             = nullptr;
        CSequenceFramePlayer* m_pForeJPGPlayer          = nullptr;
        CSequenceFramePlayer* m_pBackJPGPlayer          = nullptr;
        CSingleTexturePlayer* m_pBackgroundJPGPlayer    = nullptr;
        EPictureType::EPictureType m_PictureType        = EPictureType::JPG;
    };

    template<typename T>
    void CJPGRenderer::__deleteSafely(T*& vPointer)
    {
        if (vPointer != nullptr)
        {
            delete vPointer;
            vPointer = nullptr;
        }
    }
}
