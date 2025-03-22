#include "BackGroundAPKRenderer.h"
#include "Common.h"
#include "ScreenQuad.h"
#include "JsonReader.h"
#include "SingleTexturePlayer.h"

using namespace hiveVG;

CBackgroundAPKRenderer::CBackgroundAPKRenderer()
{
    __initAlgorithm();
}

CBackgroundAPKRenderer::~CBackgroundAPKRenderer()
{
    if (m_pScreenQuad)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
    if (m_pBackgroundPlayer)
    {
        delete m_pBackgroundPlayer;
        m_pBackgroundPlayer = nullptr;
    }
}

void CBackgroundAPKRenderer::__initAlgorithm()
{
    std::string FileName   = "configs/BackgroundAppConfig.json";
    CJsonReader JsonReader = CJsonReader(FileName);
    Json::Value BackConfig = JsonReader.getObject("Background");
    std::string BackgroundPath = BackConfig["frames_path"].asString();

    m_pBackgroundPlayer = new CSingleTexturePlayer(BackgroundPath);
    m_pBackgroundPlayer->initTextureAndShaderProgram();
    m_pScreenQuad = CScreenQuad::getOrCreate();
}

void CBackgroundAPKRenderer::renderScene()
{
    glClearColor(0.345f,0.345f,0.345f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    m_pBackgroundPlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();
}