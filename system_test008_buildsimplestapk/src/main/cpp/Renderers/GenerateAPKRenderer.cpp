#include "GenerateAPKRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "Common.h"
#include "ScreenQuad.h"
#include "JsonReader.h"
#include "TimeUtils.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"

using namespace hiveVG;

CGenerateAPKRenderer::CGenerateAPKRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CGenerateAPKRenderer::~CGenerateAPKRenderer()
{
    if (m_pScreenQuad)             delete m_pScreenQuad;
    if (m_pBackFramePlayer)        delete m_pBackFramePlayer;
}

void CGenerateAPKRenderer::__initAlgorithm()
{
    std::string FileName   = "configs/MainAppConfig.json";
    CJsonReader JsonReader = CJsonReader(FileName);
    Json::Value BackConfig     = JsonReader.getObject("Background");

    std::string BackgroundPath = BackConfig["frames_path"].asString();
    m_pBackFramePlayer   = new CSingleTexturePlayer(BackgroundPath);
    m_pBackFramePlayer->initTextureAndShaderProgram();
    m_pScreenQuad   = CScreenQuad::getOrCreate();
    m_LastFrameTime = CTimeUtils::getCurrentTime();
}

void CGenerateAPKRenderer::renderScene(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.345f,0.345f,0.345f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_pBackFramePlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();
}

