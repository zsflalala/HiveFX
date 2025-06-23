#include "RaindropEraserRenderer.h"
#include <GLES3/gl3.h>
#include <cassert>
#include <algorithm>
#include "Common.h"
#include "TimeUtils.h"
#include "ScreenQuad.h"
#include "ShaderProgram.h"
#include "JsonReader.h"
#include "SequenceFramePlayer.h"
#include "JsonReader.h"

using namespace hiveVG;

CRaindropEraserRenderer::CRaindropEraserRenderer()
{
    __initAlgorithm();
}

CRaindropEraserRenderer::CRaindropEraserRenderer(int vScreenWidth, int vScreenHeight) : m_Width(vScreenWidth), m_Height(vScreenHeight)
{
    __initAlgorithm();
}

CRaindropEraserRenderer::~CRaindropEraserRenderer()
{
    if (m_pScreenQuad)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
    if (m_pRaindropPlayer)
    {
        delete m_pRaindropPlayer;
        m_pRaindropPlayer = nullptr;
    }
}

void CRaindropEraserRenderer::render(ERenderChannel vRenderChannel)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.1f,0.1f,0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    __fadeEraseMask(static_cast<float>(DeltaTime) * 0.1f);
    m_pRaindropPlayer->updateMultiChannelFrame(DeltaTime, vRenderChannel);
    m_pRaindropPlayer->drawRaindropEraser(m_pScreenQuad, m_EraseMaskTex);
}

void CRaindropEraserRenderer::eraseAtPosition(float vPointX, float vPointY)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_EraseFBO);
    m_pEraseBrushShader->useProgram();
    m_pEraseBrushShader->setUniform("uBrushCenter", glm::vec2(vPointX, vPointY));
    m_pEraseBrushShader->setUniform("uBrushSize", m_BrushSize);
    m_pScreenQuad->bindAndDraw();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CRaindropEraserRenderer::resetLastPoint()
{
    m_HasLastPoint = false;
}

void CRaindropEraserRenderer::recreateFramebufferAndTextures(int vWindowWidth, int vWindowHeight)
{
    if (m_EraseMaskTex) glDeleteTextures(1, &m_EraseMaskTex);
    if (m_EraseFBO)     glDeleteFramebuffers(1, &m_EraseFBO);
    if (m_EraseMaskTexCopied) glDeleteTextures(1, &m_EraseMaskTexCopied);
    if (m_EraseFBOCopied)     glDeleteFramebuffers(1, &m_EraseFBOCopied);

    m_Width = vWindowWidth;
    m_Height = vWindowHeight;

    std::vector<uint8_t> whiteMask(vWindowWidth * vWindowHeight, 255);
    glGenTextures(1, &m_EraseMaskTex);
    glBindTexture(GL_TEXTURE_2D, m_EraseMaskTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, m_Width, m_Height, 0, GL_RED, GL_UNSIGNED_BYTE, whiteMask.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenFramebuffers(1, &m_EraseFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_EraseFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_EraseMaskTex, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenTextures(1, &m_EraseMaskTexCopied);
    glBindTexture(GL_TEXTURE_2D, m_EraseMaskTexCopied);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, m_Width, m_Height, 0, GL_RED, GL_UNSIGNED_BYTE, whiteMask.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenFramebuffers(1, &m_EraseFBOCopied);
    glBindFramebuffer(GL_FRAMEBUFFER, m_EraseFBOCopied);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_EraseMaskTexCopied, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CRaindropEraserRenderer::__initAlgorithm()
{
    m_pScreenQuad = CScreenQuad::getOrCreate();

    std::string ConfigFilePath = "configs/RaindropEraser.json";
    CJsonReader JsonConfig = CJsonReader(ConfigFilePath);

    Json::Value BigRaindropConfig = JsonConfig.getObject("BigRaindrop");
    std::string BigRaindropFramePath = BigRaindropConfig["frames_path"].asString();
    std::string BigRaindropFrameType = BigRaindropConfig["frames_type"].asString();
    int         BigRaindropFrameCount = BigRaindropConfig["frames_count"].asInt();
    int         BigRaindropTextureFrames = BigRaindropConfig["one_texture_frames"].asInt();
    float       BigRaindropPlayFPS    = BigRaindropConfig["fps"].asFloat();
    std::string BigRaindropVertexShader = BigRaindropConfig["vertex_shader"].asString();
    std::string BigRaindropFragShader   = BigRaindropConfig["fragment_shader"].asString();
    std::string EraseFragShader   = BigRaindropConfig["erase_fragment_shader"].asString();
    std::string FadeFragShader    = BigRaindropConfig["fade_fragment_shader"].asString();
    std::string CopyFragShader    = BigRaindropConfig["copy_fragment_shader"].asString();
    EPictureType::EPictureType BigRaindropPicType = EPictureType::FromString(BigRaindropFrameType);

    m_pRaindropPlayer = new CSequenceFramePlayer(BigRaindropFramePath, BigRaindropFrameCount, BigRaindropTextureFrames, BigRaindropPlayFPS, BigRaindropPicType);
    m_pRaindropPlayer->initTextureAndShaderProgram(BigRaindropVertexShader, BigRaindropFragShader);

    __createEraseFramebuffer();
    m_pEraseBrushShader = CShaderProgram::createProgram(BigRaindropVertexShader, EraseFragShader);
    m_pFadeShader       = CShaderProgram::createProgram(BigRaindropVertexShader, FadeFragShader);
    m_pCopyShader       = CShaderProgram::createProgram(BigRaindropVertexShader, CopyFragShader);

    m_LastFrameTime = CTimeUtils::getCurrentTime();
}

void CRaindropEraserRenderer::__createEraseFramebuffer()
{
    std::vector<uint8_t> whiteMask(m_Width * m_Height, 255);
    glGenTextures(1, &m_EraseMaskTex);
    glBindTexture(GL_TEXTURE_2D, m_EraseMaskTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, m_Width, m_Height, 0, GL_RED, GL_UNSIGNED_BYTE, whiteMask.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // 🔧 防止左右重复
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // 🔧 防止上下重复
    glGenFramebuffers(1, &m_EraseFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_EraseFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_EraseMaskTex, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenTextures(1, &m_EraseMaskTexCopied);
    glBindTexture(GL_TEXTURE_2D, m_EraseMaskTexCopied);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, m_Width, m_Height, 0, GL_RED, GL_UNSIGNED_BYTE, whiteMask.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenFramebuffers(1, &m_EraseFBOCopied);
    glBindFramebuffer(GL_FRAMEBUFFER, m_EraseFBOCopied);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_EraseMaskTexCopied, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void CRaindropEraserRenderer::__fadeEraseMask(float vDeltaTime)
{
    // TODO: why this method would cause 镜像笔触
//    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_EraseFBO);
//    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_EraseMaskTex, 0);
//    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_EraseFBOCopied);
//    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_EraseMaskTexCopied, 0);
//    glBlitFramebuffer(
//            0, 0, m_Width, m_Height,  // src rect
//            0, 0, m_Width, m_Height,  // dst rect
//            GL_COLOR_BUFFER_BIT,      // what to copy
//            GL_NEAREST                // filter
//    );
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//    glBindFramebuffer(GL_FRAMEBUFFER, m_EraseFBO);
//    glViewport(0, 0, m_Width, m_Height);
//    m_pFadeShader->useProgram();
//    m_pFadeShader->setUniform("uFadeStrength", 0.02f);
//    m_pFadeShader->setUniform("uEraseMask", 0);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, m_EraseMaskTexCopied);
//    m_pScreenQuad->bindAndDraw();
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, m_EraseFBOCopied);
    glViewport(0, 0, m_Width, m_Height);
    m_pCopyShader->useProgram();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_EraseMaskTex);
    m_pCopyShader->setUniform("uSourceTex", 0);
    m_pScreenQuad->bindAndDraw();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, m_EraseFBO);
    glViewport(0, 0, m_Width, m_Height);
    m_pFadeShader->useProgram();
    m_pFadeShader->setUniform("uFadeStrength", vDeltaTime);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_EraseMaskTexCopied);
    m_pFadeShader->setUniform("uEraseMask", 0);
    m_pScreenQuad->bindAndDraw();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}