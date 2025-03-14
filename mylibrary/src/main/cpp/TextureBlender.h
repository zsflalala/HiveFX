#pragma once

#include <functional>
#include <string>
#include <vector>
#include <GLES3/gl3.h>
#include "Common.h"

namespace hiveVG
{
    class CShaderProgram;
    class CScreenQuad;
    class CTexture2D;

    class CTextureBlender
    {
    public:
        CTextureBlender();
        ~CTextureBlender();

        bool init(int vWidth, int vHeight);
        void setBlendingMode(EBlendingMode::EBlendingMode vMode) { m_BlendingMode = vMode; }
        void updateResolution(int vWidth, int vHeight);
        void drawAndBlend(const std::function<void()>& vDrawCall);
        void blit(bool vIsBlitToScreen = true, CTexture2D *vTexture = nullptr);
        void blitSrc(bool vIsBlitToScreen = true);
        void blitTex(CTexture2D *vSrcTex);
        bool isInit() {return m_IsInit;}

        GLuint getDstFBO() const { return m_DstFBO; }
        GLuint getSrcFBO() const { return m_SrcFBO; }

    private:
        bool __createFBO();
        bool __createTexture(int vWidth, int vHeight);
        static bool __bindTex2FBO(GLuint& vFboId, CTexture2D* vTexture);
        static bool __compilerShaders();
        void __blend();
        bool __updateTexSize(GLuint& vFboId, CTexture2D* vTextrue, int vWidth, int vHeight);

        EBlendingMode::EBlendingMode m_BlendingMode;
        bool   m_IsInit;
        bool   m_IsBlend;
        bool   m_IsDstTex1Bound;
        GLuint m_DstFBO;
        GLuint m_SrcFBO;

        CTexture2D*     m_pDstTexture0 = nullptr;
        CTexture2D*     m_pDstTexture1 = nullptr;
        CTexture2D*     m_pSrcTexture  = nullptr;
        CScreenQuad*    m_pScreenQuad  = nullptr;

        static CShaderProgram*              m_pBlitShaderProgram;
        static std::vector<CShaderProgram*> m_BlendShaderPrograms;
        std::string                  m_BlitVertShaderFile;
        std::vector<std::string>     m_BlendShaderFileList;
    };
}

