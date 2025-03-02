#pragma once

#include <functional>
#include <string>
#include <vector>
#include <GLES3/gl3.h>
#include <android/asset_manager.h>
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
        bool init(AAssetManager* vAssetManager, int vWidth, int vHeight);
        void setBlendingMode(EBlendingMode vMode) { m_BlendingMode = vMode; }
        void updateResolution(int vWidth, int vHeight);
        void drawAndBlend(std::function<void()> vDrawCall);
        void blitToScreen(CTexture2D *vTexture = nullptr);
        void blitSrcToScreen();

    private:
        bool __createFBO();
        bool __createTexture(int vWidth, int vHeight);
        bool __bindTex2FBO(GLuint& vFboId, CTexture2D* vTexture);
        bool __compilerShaders(AAssetManager* vAssetManager);
        CShaderProgram* __compilerShader(AAssetManager* vAssetManager, const std::string& vVertFile, const std::string& vFragFile);
        void __blend(CShaderProgram* vShaderProgram);
        bool __updateTexSize(GLuint& vFboId, CTexture2D* vTextrue, int vWidth, int vHeight);

        EBlendingMode   m_BlendingMode;
        bool            m_IsInit;
        bool            m_IsDstTex1bound;
        GLuint          m_DstFBO;
        GLuint          m_SrcFBO;

        CTexture2D*     m_pDstTexture0 = nullptr;
        CTexture2D*     m_pDstTexture1 = nullptr;
        CTexture2D*     m_pSrcTexture  = nullptr;
        CScreenQuad*    m_pScreenQuad  = nullptr;
        CShaderProgram* m_pBlitShaderProgram = nullptr;

        std::vector<CShaderProgram*> m_BlendShaderPrograms;
        std::string                  m_BlitVertShaderFile;
        std::vector<std::string>     m_BlendShaderFileList;
    };
}

