#include "TestSnowRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <android/asset_manager.h>
#include <json/json.h>
#include "Common.h"
#include "TimeUtils.h"
#include "ScreenQuad.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"
#include "BillBoardManager.h"
#include "JsonReader.h"

using namespace hiveVG;

CTestSnowRenderer::CTestSnowRenderer()
{
    __initAlgorithm();
}

CTestSnowRenderer::~CTestSnowRenderer()
{
}

void CTestSnowRenderer::__initAlgorithm()
{
    m_LastFrameTime = CTimeUtils::getCurrentTime();
}

void CTestSnowRenderer::renderScene(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.345f, 0.345f, 0.345f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}