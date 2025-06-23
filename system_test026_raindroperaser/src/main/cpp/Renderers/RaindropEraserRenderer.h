#pragma once

#include <EGL/egl.h>
#include "Common.h"

namespace hiveVG
{
    class CScreenQuad;
    class CShaderProgram;
    class CSequenceFramePlayer;

    class CRaindropEraserRenderer
    {
    public:
        CRaindropEraserRenderer();
        CRaindropEraserRenderer(int vScreenWidth, int vScreenHeight);
        ~CRaindropEraserRenderer();

        void render(ERenderChannel vRenderChannel);
        void eraseAtPosition(float vPointX, float vPointY);
        void resetLastPoint();
        void recreateFramebufferAndTextures(int vWindowWidth, int vWindowHeight);

    private:
        void __initAlgorithm();
        void __createEraseFramebuffer();
        void __fadeEraseMask(float vDeltaTime);

        double       m_LastFrameTime = 0.0f;
        double       m_CurrentTime   = 0.0f;
        glm::vec2    m_LastPoint;
        bool         m_HasLastPoint = false;
        float        m_BrushSize = 100.0f;

        int m_Width = 2992, m_Height = 1334;
        GLuint m_EraseFBO = 0;
        GLuint m_EraseMaskTex = 0;
        GLuint m_EraseFBOCopied = 0;
        GLuint m_EraseMaskTexCopied = 0;

        CScreenQuad*          m_pScreenQuad = nullptr;
        CSequenceFramePlayer* m_pRaindropPlayer = nullptr;
        CShaderProgram*       m_pEraseBrushShader = nullptr;
        CShaderProgram*       m_pFadeShader       = nullptr;
        CShaderProgram*       m_pCopyShader       = nullptr;
    };
}