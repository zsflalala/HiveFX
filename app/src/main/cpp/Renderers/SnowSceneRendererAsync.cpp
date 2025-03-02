#include "SnowSceneRendererAsync.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "Common.h"
#include "ScreenQuad.h"
#include "SingleTexturePlayer.h"
#include "AsyncSequenceFramePlayer.h"

using namespace hiveVG;

CSnowSceneRendererAsync::CSnowSceneRendererAsync(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CSnowSceneRendererAsync::~CSnowSceneRendererAsync()
{
    if (m_pScreenQuad)          delete m_pScreenQuad;
    if (m_pSnowSceneSeqFramePlayer) delete m_pSnowSceneSeqFramePlayer;
}

void CSnowSceneRendererAsync::__initAlgorithm()
{
    int TextureCount = 128;
    EPictureType::EPictureType PictureType = EPictureType::PNG;
    m_pScreenQuad = CScreenQuad::getOrCreate();

    m_pSnowSceneSeqFramePlayer = new CAsyncSequenceFramePlayer("Textures/SnowCover", TextureCount, PictureType);
    m_pSnowSceneSeqFramePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
    m_pSnowSceneSeqFramePlayer->setFrameRate(5);
    m_pSnowSceneSeqFramePlayer->setLoopPlayback(false);
}

void CSnowSceneRendererAsync::renderScene()
{
    glClearColor(0.1f,0.1f,0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_pSnowSceneSeqFramePlayer->updateFrames();
    m_pScreenQuad->bindAndDraw();

}

void CSnowSceneRendererAsync::handleInput(ERenderType vInputType, bool vIsPointerDown)
{
    if (vInputType == ERenderType::SNOW_SCENE)
    {
        // 在持续摁下时只改变一次 m_EnableSmallSnowFore 的变量值
        if (vIsPointerDown && !m_PreviousPointerState) m_EnableSnowScene = !m_EnableSnowScene;
    }
    m_PreviousPointerState = vIsPointerDown;
}