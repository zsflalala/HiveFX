#include "SnowSceneRendererAsync.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "../Common.h"
#include "../ScreenQuad.h"
#include "../SingleTexturePlayer.h"
#include "../AsyncSequenceFramePlayer.h"

using namespace hiveVG;

CSnowSceneRendererAsync::CSnowSceneRendererAsync(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CSnowSceneRendererAsync::~CSnowSceneRendererAsync()
{
    if (m_pScreenQuad)          delete m_pScreenQuad;
    if (m_pAsyncSeqFramePlayer) delete m_pAsyncSeqFramePlayer;
}

void CSnowSceneRendererAsync::__initAlgorithm()
{
    int TextureCount = 128;
    EPictureType PictureType = EPictureType::PNG;
    m_pScreenQuad = CScreenQuad::getOrCreate();
    m_pAsyncSeqFramePlayer = new CAsyncSequenceFramePlayer("Textures/SnowScene", TextureCount, PictureType);
    m_pAsyncSeqFramePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
    m_pAsyncSeqFramePlayer->setFrameRate(5);
}

void CSnowSceneRendererAsync::renderScene()
{
    glClearColor(0.3f,0.5f,0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_pAsyncSeqFramePlayer->updateFrames();
    m_pScreenQuad->bindAndDraw();
}