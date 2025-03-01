#include "RainRendererAsync.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "../Common.h"
#include "../ScreenQuad.h"
#include "../SingleTexturePlayer.h"
#include "../AsyncSequenceFramePlayer.h"

using namespace hiveVG;

CRainRendererAsync::CRainRendererAsync(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CRainRendererAsync::~CRainRendererAsync()
{
    if (m_pScreenQuad)          delete m_pScreenQuad;
    if (m_pAsyncSeqFramePlayer) delete m_pAsyncSeqFramePlayer;
    if (m_pSingleFramePlayer)   delete m_pSingleFramePlayer;
}

void CRainRendererAsync::__initAlgorithm()
{
    int TextureCount = 64;
    EPictureType PictureType = EPictureType::PNG;
    m_pScreenQuad = CScreenQuad::getOrCreate();
    m_pAsyncSeqFramePlayer = new CAsyncSequenceFramePlayer("Textures/BigRain", TextureCount, PictureType);
    m_pAsyncSeqFramePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
    m_pSingleFramePlayer = new CSingleTexturePlayer("Textures/background.png");
    m_pSingleFramePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
}

void CRainRendererAsync::renderScene()
{
    glClearColor(0.3f,0.5f,0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);

    m_pSingleFramePlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_pAsyncSeqFramePlayer->updateFrames();
    m_pScreenQuad->bindAndDraw();
}