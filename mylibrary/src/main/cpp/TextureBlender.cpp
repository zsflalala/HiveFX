#include "TextureBlender.h"
#include "ShaderProgram.h"
#include "ScreenQuad.h"
#include "Texture2D.h"

using namespace hiveVG;

CTextureBlender::CTextureBlender() : m_BlendingMode(EBlendingMode::NORMAL), m_IsInit(false), m_IsDstTex1Bound(false), m_DstFBO(0), m_SrcFBO(0)
{
    m_pScreenQuad = CScreenQuad::getOrCreate();
    // TODO: 用其他方式管理混合 shader。当使用到某 blend 算法时再生成
    m_BlitVertShaderFile = BlitTex2ScreenVert;
    m_BlendShaderFileList.emplace_back(BlendAlphaFrag);
    m_BlendShaderFileList.emplace_back(BlendAlphaFrag);
    m_BlendShaderFileList.emplace_back(BlendLightenFrag);
    m_BlendShaderFileList.emplace_back(BlendLinearDodgeFrag);
    m_BlendShaderFileList.emplace_back(BlendLighterColorFrag);
}

CTextureBlender::~CTextureBlender()
{
    // TODO: ScreenQuad 单例如何释放
    //if (m_pScreenQuad) delete m_pScreenQuad;
    if (m_pDstTexture0) delete m_pDstTexture0;
    if (m_pDstTexture1) delete m_pDstTexture1;
    if (m_pSrcTexture) delete m_pSrcTexture;
    if (m_pBlitShaderProgram) delete m_pBlitShaderProgram;
    for (auto ShaderProgram : m_BlendShaderPrograms)
        if(ShaderProgram) delete ShaderProgram;
    m_BlendShaderPrograms.clear();
}

bool CTextureBlender::init(AAssetManager* vAssetManager, int vWidth, int vHeight)
{
    if(m_IsInit) return true;
    m_IsInit = __createFBO() && __createTexture(vWidth, vHeight)
               && __bindTex2FBO(m_SrcFBO, m_pSrcTexture)
               && __bindTex2FBO(m_DstFBO, m_pDstTexture0)
               && __compilerShaders(vAssetManager);
    return m_IsInit;
}

void CTextureBlender::updateResolution(int vWidth, int vHeight)
{
    // TODO
}

void CTextureBlender::drawAndBlend(const std::function<void()>& vDrawCall)
{
    if(!m_IsInit)
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
    assert(static_cast<std::size_t>(m_BlendingMode) < m_BlendShaderPrograms.size());
    __blend(m_BlendShaderPrograms[static_cast<std::size_t>(m_BlendingMode)]);
}

void CTextureBlender::blitToScreen(CTexture2D *vTexture)
{
    if(!m_IsInit)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE_BLENDER_TAG, "Initializing is not complete!");
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    m_pBlitShaderProgram->useProgram();
    m_pBlitShaderProgram->setUniform("fboTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    if(vTexture == nullptr)
        if(m_IsDstTex1Bound)
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

void CTextureBlender::blitSrcToScreen()
{
    blitToScreen(m_pSrcTexture);
}

bool CTextureBlender::__updateTexSize(GLuint &vFboId, CTexture2D* vTexture, int vWidth, int vHeight)
{
    delete vTexture;
    int Channels = 4;
    vTexture = CTexture2D::createEmptyTexture(vWidth, vHeight, Channels);

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
    int Channels = 4;
    m_pSrcTexture  = CTexture2D::createEmptyTexture(vWidth, vHeight, Channels);
    m_pDstTexture0 = CTexture2D::createEmptyTexture(vWidth, vHeight, Channels);
    m_pDstTexture1 = CTexture2D::createEmptyTexture(vWidth, vHeight, Channels);
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

    if (m_DstFBO == 0 || m_SrcFBO == 0)
    {
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

bool CTextureBlender::__compilerShaders(AAssetManager* vAssetManager)
{

    m_pBlitShaderProgram = CShaderProgram::createProgram(vAssetManager, BlitTex2ScreenVert, BlitTex2ScreenFrag);
    m_BlendShaderPrograms.push_back(CShaderProgram::createProgram(vAssetManager, BlitTex2ScreenVert, BlendAlphaFrag));
    m_BlendShaderPrograms.push_back(CShaderProgram::createProgram(vAssetManager, BlitTex2ScreenVert, BlendMultiplyFrag));
    m_BlendShaderPrograms.push_back(CShaderProgram::createProgram(vAssetManager, BlitTex2ScreenVert, BlendLightenFrag));
    m_BlendShaderPrograms.push_back(CShaderProgram::createProgram(vAssetManager, BlitTex2ScreenVert, BlendLinearDodgeFrag));
    m_BlendShaderPrograms.push_back(CShaderProgram::createProgram(vAssetManager, BlitTex2ScreenVert, BlendLighterColorFrag));
    // m_BlendShaderPrograms.push_back(CShaderProgram::createProgram(vAssetManager, BlitTex2ScreenVert, BlendOverlayFrag));

    if(!m_pBlitShaderProgram)
        return false;
    for (auto &pBlendShaderProgram: m_BlendShaderPrograms)
    {
        if (!pBlendShaderProgram) return false;
    }

    return true;
}

void CTextureBlender::__blend(CShaderProgram* vShaderProgram)
{
    if(!vShaderProgram)
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE_BLENDER_TAG, "Blend Shader isn't exist.");
        return;
    }
    assert(vShaderProgram);
    CTexture2D* pDstTex = m_IsDstTex1Bound ? m_pDstTexture1 : m_pDstTexture0;
    if(m_IsDstTex1Bound)
        __bindTex2FBO(m_DstFBO, m_pDstTexture0);
    else
        __bindTex2FBO(m_DstFBO, m_pDstTexture1);
    m_IsDstTex1Bound = !m_IsDstTex1Bound;

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