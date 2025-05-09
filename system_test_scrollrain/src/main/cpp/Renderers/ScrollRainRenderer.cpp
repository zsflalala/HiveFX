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
    __deleteSafely(m_pSlideWindowFore);
    __deleteSafely(m_pSlideWindowBack);
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

    m_pSlideWindowBack->updateFrameAndDraw(m_WindowWidth, m_WindowHeight, DeltaTime * 100.0f, m_pScreenQuad);
    m_pBackFramePlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();
    m_pSlideWindowFore->updateFrameAndDraw(m_WindowWidth, m_WindowHeight, DeltaTime * 100.0f, m_pScreenQuad);
    m_pSplashManager->updateFrameAndUV(DeltaTime);
    m_pSplashManager->updateSequenceState(static_cast<float>(DeltaTime));
    m_pSplashManager->draw(m_pScreenQuad);
}

void CScrollRainRenderer::__initAlgorithm()
{
    std::string FileName = "configs/ScrollRainConfig.json";
    CJsonReader JsonReader = CJsonReader(FileName);
    Json::Value SlideConfig = JsonReader.getObject("SlideFore");
    std::string PicturePath = SlideConfig["picture_path"].asString();
    std::string PictureType = SlideConfig["picture_type"].asString();
    std::string VertexPath  = SlideConfig["vertex_path"].asString();
    std::string FragmentPath = SlideConfig["fragment_path"].asString();
    float SlideSpeed = SlideConfig["slide_speed"].asFloat();
    std::string SlideDirection = SlideConfig["slide_direction"].asString();
    bool IsCompressed = SlideConfig["is_compressed"].asBool();

    Json::Value BackConfig     = JsonReader.getObject("Background");
    std::string BackgroundPath = BackConfig["frames_path"].asString();
    std::string KtxBackgroundPath = BackConfig["ktxframes_path"].asString();
    std::string BackgroundType = BackConfig["frames_type"].asString();
    std::string BackgroundVertexPath = BackConfig["vertex_path"].asString();
    std::string BackgroundFragmentPath = BackConfig["fragment_path"].asString();

    Json::Value SplashConfig = JsonReader.getObject("Splash");
    std::string SplashPath = SplashConfig["frames_path"].asString();
    std::string SplashType = SplashConfig["frames_type"].asString();
    int         SplashFrameCount = SplashConfig["frames_count"].asInt();
    std::string SplashPlayMode   = SplashConfig["play_mode"].asString();
    std::string VertexShader   = SplashConfig["vertex_path"].asString();
    std::string FragShader     = SplashConfig["fragment_path"].asString();
    float         SplashPlayFPS    = SplashConfig["fps"].asInt();
    float  SplashPlayScale = SplashConfig["scale"].asFloat();
    int    SplashSeqRows = 1;
    int    SplashSeqCols = 1;
    int    OneTexFrames = SplashConfig["one_texture_frames"].asInt();
    EPictureType::EPictureType SplashPicType = EPictureType::FromString(SplashType);

    m_pScreenQuad = CScreenQuad::getOrCreate();
    m_pSlideWindowFore = new CSlideWindow(PicturePath, SlideSpeed, SlideDirection, EPictureType::FromString(PictureType), IsCompressed);
    m_pSlideWindowFore->initTextureAndShaderProgram(VertexPath,FragmentPath);
    SlideConfig = JsonReader.getObject("SlideBack");
    PicturePath = SlideConfig["picture_path"].asString();
    PictureType = SlideConfig["picture_type"].asString();
    VertexPath  = SlideConfig["vertex_path"].asString();
    FragmentPath = SlideConfig["fragment_path"].asString();
    SlideSpeed = SlideConfig["slide_speed"].asFloat();
    SlideDirection = SlideConfig["slide_direction"].asString();
    IsCompressed = SlideConfig["is_compressed"].asBool();
    m_pSlideWindowBack = new CSlideWindow(PicturePath, SlideSpeed, SlideDirection, EPictureType::FromString(PictureType), IsCompressed);
    m_pSlideWindowBack->initTextureAndShaderProgram(VertexPath,FragmentPath);
    m_pBackFramePlayer = new CSingleTexturePlayer(KtxBackgroundPath,EPictureType::FromString(BackgroundType));
    m_pBackFramePlayer->initTextureAndShaderProgram(BackgroundVertexPath,BackgroundFragmentPath);

    m_pSplashPlayer = new CSequenceFramePlayer(SplashPath,SplashFrameCount,OneTexFrames,SplashPlayFPS,SplashPicType);
    //m_pSplashPlayer = new CSequenceFramePlayer(SplashPath, SplashSeqRows, SplashSeqCols, SplashFrameCount, SplashPicType);
    m_pSplashPlayer->initTextureAndShaderProgram(VertexShader,FragShader);
    m_pSplashPlayer->setFrameRate(SplashPlayFPS);
    m_pSplashPlayer->setScreenUVScale(glm::vec2(SplashPlayScale, SplashPlayScale));
    m_pSplashManager = std::make_unique<CSplashManager>();
    m_pSplashManager->setIsQuantization(SlideConfig["is_compressed"].asBool());
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