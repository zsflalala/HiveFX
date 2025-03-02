#include "TextureBlender.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "Texture2D.h"

namespace hiveVG {
    CTextureBlender::CTextureBlender() : m_BlendingMode(EBlendingMode::NORMAL), m_isInit(false), m_isDstTex1bound(false), m_DstFBO(0), m_SrcFBO(0)
    {
        m_pScreenQuad = CScreenQuad::getOrCreate();

        // TODO: 用其他方式管理混合 shader。当使用到某 blend 算法时再生成
        m_pBlitVertShaderFile = "shaders/blitTex2Screen.vert";
        m_pBlendShaderFileList.push_back("shaders/blendAlpha.frag");
        m_pBlendShaderFileList.push_back("shaders/blendMultiply.frag");
        m_pBlendShaderFileList.push_back("shaders/blendLighten.frag");
        m_pBlendShaderFileList.push_back("shaders/blendLinearDodge.frag");
        m_pBlendShaderFileList.push_back("shaders/blendLighterColor.frag");

//        for(int i = 0; i < m_pBlendShaderFileList.size(); i++)
//        {
//            m_pBlendShaderPrograms.push_back(nullptr);
//        }
    }

    CTextureBlender::~CTextureBlender()
    {
        // TODO: ScreenQuad 单例如何释放
        //if (m_pScreenQuad) delete m_pScreenQuad;
        if (m_pDstTexture0) delete m_pDstTexture0;
        if (m_pDstTexture1) delete m_pDstTexture1;
        if (m_pSrcTexture) delete m_pSrcTexture;
        if (m_pBlitShaderProgram) delete m_pBlitShaderProgram;
        for (auto ShaderProgram : m_pBlendShaderPrograms)
            if(ShaderProgram) delete ShaderProgram;
        m_pBlendShaderPrograms.clear();
    }

    bool CTextureBlender::init(AAssetManager* vAssetManager, int vWidth, int vHeight)
    {
        if(m_isInit)
            return true;

        m_isInit = __createFBO() && __createTexture(vWidth, vHeight)
                && __bindTex2FBO(m_SrcFBO, m_pSrcTexture)
                && __bindTex2FBO(m_DstFBO, m_pDstTexture0)
                && __compilerShaders(vAssetManager);
        return m_isInit;
    }

    void CTextureBlender::updateResolution(int vWidth, int vHeight)
    {
        // TODO
    }

    void CTextureBlender::drawAndBlend(std::function<void()> vDrawCall)
    {
        if(!m_isInit)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE_BLENDER_TAG, "Initializing is not complete!");
            return;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, m_SrcFBO);
        glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_BLEND);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        vDrawCall();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        assert(static_cast<std::size_t>(m_BlendingMode) < m_pBlendShaderPrograms.size());

        __blend(m_pBlendShaderPrograms[static_cast<std::size_t>(m_BlendingMode)]);
    }

    void CTextureBlender::blitToScreen(CTexture2D *vTexture)
    {
        if(!m_isInit)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE_BLENDER_TAG, "Initializing is not complete!");
            return;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        m_pBlitShaderProgram->useProgram();
        m_pBlitShaderProgram->setUniform("fboTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        if(vTexture == nullptr)
            if(m_isDstTex1bound)
                m_pDstTexture1->bindTexture();
            else
                m_pDstTexture0->bindTexture();
        else
            vTexture->bindTexture();

        m_pScreenQuad->bindAndDraw();

        if(vTexture == nullptr)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, m_DstFBO);
            glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }
    }

    void CTextureBlender::blitSrcToScreen() {
        blitToScreen(m_pSrcTexture);
    }

    bool CTextureBlender::__updateTexSize(GLuint &vFboId, CTexture2D* vTexture, int vWidth, int vHeight)
    {
        delete vTexture;
        vTexture = CTexture2D::createEmptyTexture(vWidth, vHeight, 4);

        glBindFramebuffer(GL_FRAMEBUFFER, vFboId);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, vTexture->getTextureHandle(), 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE_BLENDER_TAG, "Framebuffer is not complete after texture regeneration!");
            return false;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return true;
    }

    bool CTextureBlender::__createTexture(int vWidth, int vHeight)
    {
        m_pSrcTexture = CTexture2D::createEmptyTexture(vWidth, vHeight, 4);
        m_pDstTexture0 = CTexture2D::createEmptyTexture(vWidth, vHeight, 4);
        m_pDstTexture1 = CTexture2D::createEmptyTexture(vWidth, vHeight, 4);
        if(m_pSrcTexture == nullptr || m_pDstTexture0 == nullptr || m_pDstTexture1 == nullptr)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE_BLENDER_TAG, "Failed to generate textures.");
            return false;
        }
        return true;
    }

    bool CTextureBlender::__createFBO()
    {
        GLuint FboIds[2];
        glGenFramebuffers(2, FboIds);
        m_DstFBO = FboIds[0];
        m_SrcFBO = FboIds[1];

        if (m_DstFBO == 0 || m_SrcFBO == 0) {
            LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE_BLENDER_TAG, "Failed to generate framebuffer object.");
            return false;
        }
        return true;
    }

    bool CTextureBlender::__bindTex2FBO(GLuint &vFboId, CTexture2D *vTexture)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, vFboId);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, vTexture->getTextureHandle(), 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE_BLENDER_TAG, "Framebuffer is not complete.");
            return false;
        }
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return true;
    }

    bool CTextureBlender::__compilerShaders(AAssetManager* vAssetManager) {
        m_pBlitShaderProgram = __compilerShader(vAssetManager, "shaders/blitTex2Screen.vert", "shaders/blitTex2Screen.frag");
        m_pBlendShaderPrograms.push_back(__compilerShader(vAssetManager, "shaders/blitTex2Screen.vert", "shaders/blendAlpha.frag"));
        m_pBlendShaderPrograms.push_back(__compilerShader(vAssetManager, "shaders/blitTex2Screen.vert", "shaders/blendMultiply.frag"));
        m_pBlendShaderPrograms.push_back(__compilerShader(vAssetManager, "shaders/blitTex2Screen.vert", "shaders/blendLighten.frag"));
        m_pBlendShaderPrograms.push_back(__compilerShader(vAssetManager, "shaders/blitTex2Screen.vert", "shaders/blendLinearDodge.frag"));
        m_pBlendShaderPrograms.push_back(__compilerShader(vAssetManager, "shaders/blitTex2Screen.vert", "shaders/blendLighterColor.frag"));

        if(!m_pBlitShaderProgram)
            return false;
        return true;
    }

    CShaderProgram* CTextureBlender::__compilerShader(AAssetManager *vAssetManager, const std::string &vVertFile, const std::string &vFragFile)
    {
        auto Program = CShaderProgram::createProgram(vAssetManager, vVertFile, vFragFile);
        assert(Program != nullptr);
        if(Program == nullptr)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE_BLENDER_TAG, "Shader \"%s\" & \"%s\" build error.", vVertFile.c_str(), vFragFile.c_str());
            return nullptr;
        }
        return Program;
    }

    void CTextureBlender::__blend(CShaderProgram* vShaderProgram)
    {
        assert(vShaderProgram);
        if(!vShaderProgram)
        {
            LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE_BLENDER_TAG, "Blend Shader isn't exist.");
            return;
        }
        CTexture2D* pDstTex = m_isDstTex1bound ? m_pDstTexture1 : m_pDstTexture0;
        if(m_isDstTex1bound)
            __bindTex2FBO(m_DstFBO, m_pDstTexture0);
        else
            __bindTex2FBO(m_DstFBO, m_pDstTexture1);
        m_isDstTex1bound = !m_isDstTex1bound;

        glBindFramebuffer(GL_FRAMEBUFFER, m_DstFBO);
        glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_BLEND);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        vShaderProgram->useProgram();

        vShaderProgram->setUniform("SrcTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        m_pSrcTexture->bindTexture();

        vShaderProgram->setUniform("DstTexture", 1);
        glActiveTexture(GL_TEXTURE1);
        pDstTex->bindTexture();

        m_pScreenQuad->bindAndDraw();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}