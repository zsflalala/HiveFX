#include "ScrollRainRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <GLES3/gl3.h>
#include <cassert>
#include "SlideWindow.h"
#include "SingleTexturePlayer.h"
#include "ScreenQuad.h"
#include "TimeUtils.h"
#include "JsonReader.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"
#include "SplashManager.h"

using namespace hiveVG;

CScrollRainRenderer::CScrollRainRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CScrollRainRenderer::~CScrollRainRenderer()
{
    if (m_pScreenQuad)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
    __deleteSafely(m_pSlideWindow);
    __deleteSafely(m_pBackFramePlayer);
    __deleteSafely(m_pSplashPlayer);
}

void CScrollRainRenderer::renderScene()
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.345f,0.345f,0.345f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_pBackFramePlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();

    glBlendFunc(GL_ONE, GL_ONE);
    m_pSlideWindow->updateFrameAndDraw(m_WindowWidth, m_WindowHeight, DeltaTime * 100.0f, m_pScreenQuad);
    m_pSplashManager->updateFrameAndUV(DeltaTime);
    m_pSplashManager->updateSequenceState(static_cast<float>(DeltaTime));
    m_pSplashManager->draw(m_pScreenQuad);
}

void CScrollRainRenderer::__initAlgorithm()
{
    std::string FileName = "configs/ScrollRainConfig.json";
    CJsonReader JsonReader = CJsonReader(FileName);
    Json::Value SlideConfig = JsonReader.getObject("Slide");
    std::string PicturePath = SlideConfig["picture_path"].asString();
    std::string PictureType = SlideConfig["picture_type"].asString();
    float SlideSpeed = SlideConfig["slide_speed"].asFloat();
    std::string SlideDirection = SlideConfig["slide_direction"].asString();
    bool IsCompressed = SlideConfig["is_compressed"].asBool();

    Json::Value BackConfig     = JsonReader.getObject("Background");
    std::string BackgroundPath = BackConfig["frames_path"].asString();

    Json::Value SplashConfig = JsonReader.getObject("Splash");
    std::string SplashPath = SplashConfig["frames_path"].asString();
    std::string SplashType = SplashConfig["frames_type"].asString();
    int         SplashFrameCount = SplashConfig["frames_count"].asInt();
    std::string SplashPlayMode   = SplashConfig["play_mode"].asString();
    int         SplashPlayFPS    = SplashConfig["fps"].asInt();
    float  SplashPlayScale = SplashConfig["scale"].asFloat();
    int    SplashSeqRows = 1;
    int    SplashSeqCols = 1;
    EPictureType::EPictureType SplashPicType = EPictureType::FromString(SplashType);

    m_pScreenQuad = CScreenQuad::getOrCreate();
    m_pSlideWindow = new CSlideWindow(PicturePath, SlideSpeed, SlideDirection, EPictureType::FromString(PictureType), IsCompressed);
    m_pSlideWindow->initTextureAndShaderProgram();

    m_pBackFramePlayer = new CSingleTexturePlayer(BackgroundPath);
    m_pBackFramePlayer->initTextureAndShaderProgram();

    m_pSplashPlayer = new CSequenceFramePlayer(SplashPath, SplashSeqRows, SplashSeqCols, SplashFrameCount, SplashPicType);
    m_pSplashPlayer->initTextureAndShaderProgram();
    m_pSplashPlayer->setFrameRate(SplashPlayFPS);
    m_pSplashPlayer->setScreenUVScale(glm::vec2(SplashPlayScale, SplashPlayScale));
    m_pSplashManager = std::make_unique<CSplashManager>();
    int SplashNum = 7;
    for (int i = 0; i < SplashNum; i++)
    {
        m_pSplashManager->pushBack(m_pSplashPlayer->clone());
    }
    m_pSplashManager->initSequenceState(BackgroundPath, SplashPlayScale);

    m_LastFrameTime = CTimeUtils::getCurrentTime();
    assert(m_pApp->window != nullptr);
    if (m_pApp->window)
    {
        m_WindowWidth  = ANativeWindow_getWidth(m_pApp->window);
        m_WindowHeight = ANativeWindow_getHeight(m_pApp->window);
    }
}