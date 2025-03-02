#include "TestSlideWindowRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <android/asset_manager.h>
#include <json/json.h>
#include "Common.h"
#include "ScreenQuad.h"
#include "JsonReader.h"
#include "SlideWindow.h"

namespace hiveVG
{

    CTestSlideWindowRenderer::CTestSlideWindowRenderer(android_app *vApp) : m_pApp(vApp)
    {
        __initAlgorithm();
    }

    CTestSlideWindowRenderer::~CTestSlideWindowRenderer()
    {
        if (m_pScreenQuad)      delete m_pScreenQuad;
        if (m_pSlideWindow)     delete m_pSlideWindow;
    }

    void CTestSlideWindowRenderer::__initAlgorithm()
    {
        std::string FileName = "configs/SlideWindowConfig.json";
        CJsonReader JsonReader = CJsonReader(m_pApp->activity->assetManager, FileName);
        Json::Value SlideConfig = JsonReader.getObject("slide_config");
        std::string PicturePath = SlideConfig["picture_path"].asString();
        float SlideSpeed = SlideConfig["slide_speed"].asFloat();
        std::string SlideDirection = SlideConfig["slide_direction"].asString();

        m_pScreenQuad = CScreenQuad::getOrCreate();
        m_pSlideWindow = new CSlideWindow(PicturePath, SlideSpeed, SlideDirection);
        m_pSlideWindow->createProgram(m_pApp->activity->assetManager);
        m_pSlideWindow->loadTextures(m_pApp->activity->assetManager);

        m_LastFrameTime = __getCurrentTime();
    }

    double CTestSlideWindowRenderer::__getCurrentTime()
    {
        struct timeval tv{};
        gettimeofday(&tv, nullptr);
        return tv.tv_sec + tv.tv_usec / 1000000.0;
    }

    void CTestSlideWindowRenderer::renderScene(int vWindowWidth, int vWindowHeight)
    {
        m_CurrentTime    = __getCurrentTime();
        double DeltaTime = m_CurrentTime - m_LastFrameTime;
        m_LastFrameTime  = m_CurrentTime;

        glClearColor(0.1f,0.1f,0.1f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        m_pSlideWindow->updateFrame(vWindowWidth, vWindowHeight, DeltaTime * 100.0f, m_pScreenQuad);
//        m_pSlideWindow->draw(m_pScreenQuad);
    }
}