#include "ScrollRainCompressedRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <GLES3/gl3.h>
#include <cassert>
#include "SlideWindow.h"
#include "SingleTexturePlayer.h"
#include "ScreenQuad.h"
#include "TimeUtils.h"
#include "JsonReader.h"

using namespace hiveVG;

CScrollRainCompressedRenderer::CScrollRainCompressedRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CScrollRainCompressedRenderer::~CScrollRainCompressedRenderer()
{
    if (m_pScreenQuad)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
    if (m_pSlideWindow)
    {
        delete m_pSlideWindow;
        m_pSlideWindow = nullptr;
    }
    if (m_pBackgroundPlayer)
    {
        delete m_pBackgroundPlayer;
        m_pBackgroundPlayer = nullptr;
    }
}

void CScrollRainCompressedRenderer::renderScene()
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.345f,0.345f,0.345f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_pBackgroundPlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();
    //glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_ONE, GL_ONE);
    m_pSlideWindow->updateFrameAndDraw(m_WindowWidth, m_WindowHeight, DeltaTime * 100.0f, m_pScreenQuad);
}

void CScrollRainCompressedRenderer::__initAlgorithm()
{
    std::string FileName    = "configs/ScrollRainCompressedConfig.json";
    CJsonReader JsonReader  = CJsonReader(FileName);
    Json::Value SlideConfig = JsonReader.getObject("slide_config");
    std::string PicturePath = SlideConfig["picture_path"].asString();
    std::string PictureType = SlideConfig["picture_type"].asString();
    std::string VertexPath  = SlideConfig["vertex_path"].asString();
    std::string FragmentPath = SlideConfig["fragment_path"].asString();
    float SlideSpeed = SlideConfig["slide_speed"].asFloat();
    std::string SlideDirection = SlideConfig["slide_direction"].asString();
    bool IsCompressed = SlideConfig["is_compressed"].asBool();

    Json::Value BackGroundConfig = JsonReader.getObject("Background");
    std::string ImgPath = BackGroundConfig["frames_path"].asString();

    m_pBackgroundPlayer = new CSingleTexturePlayer(ImgPath);
    m_pBackgroundPlayer->initTextureAndShaderProgram();
    m_pScreenQuad = CScreenQuad::getOrCreate();
    m_pSlideWindow = new CSlideWindow(PicturePath, SlideSpeed, SlideDirection, EPictureType::FromString(PictureType), IsCompressed);
    m_pSlideWindow->initTextureAndShaderProgram(VertexPath,FragmentPath);

    m_LastFrameTime = CTimeUtils::getCurrentTime();
    assert(m_pApp->window != nullptr);
    if (m_pApp->window)
    {
        m_WindowWidth  = ANativeWindow_getWidth(m_pApp->window);
        m_WindowHeight = ANativeWindow_getHeight(m_pApp->window);
    }
}