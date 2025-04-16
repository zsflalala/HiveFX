#include "CTestInterpolationPlaying.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "Common.h"
#include "TimeUtils.h"
#include "ScreenQuad.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"
#include "BillBoardManager.h"
#include "JsonReader.h"
#include "AsyncSequenceFramePlayer.h"

using namespace hiveVG;

CTestInterpolationPlaying::CTestInterpolationPlaying(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CTestInterpolationPlaying::~CTestInterpolationPlaying()
{
    if (m_pScreenQuad)              delete m_pScreenQuad;
    if (m_pTestPlayer)              delete m_pTestPlayer;
}

void CTestInterpolationPlaying::__initAlgorithm()
{
    m_pSingleFramePlayer   = new CSingleTexturePlayer("textures/snowScene.png");
    m_pSingleFramePlayer->initTextureAndShaderProgram();
    m_pScreenQuad = CScreenQuad::getOrCreate();
}

void CTestInterpolationPlaying::renderScene(int vWindowWidth, int vWindowHeight)
{
    glClearColor(0.345f,0.345f,0.345f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(m_pTestPlayer == nullptr)
    {
        m_pTestPlayer = new CAsyncSequenceFramePlayer("textures/BigSnow_fore_64", m_TextureCount, m_PictureType);
        m_pTestPlayer->initTextureAndShaderProgram();
        sleep(1);
    }
    m_pTestPlayer->updateFramesWithInterpolation();
    m_pScreenQuad->bindAndDraw();
}