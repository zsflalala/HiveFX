#include "SnowSceneRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "Common.h"
#include "TimeUtils.h"
#include "ScreenQuad.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"

using namespace hiveVG;

CSnowSceneRenderer::CSnowSceneRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CSnowSceneRenderer::~CSnowSceneRenderer()
{
    if (m_pScreenQuad)              delete m_pScreenQuad;
    if (m_pSnowSceneSeqFramePlayer) delete m_pSnowSceneSeqFramePlayer;
}

void CSnowSceneRenderer::__initAlgorithm()
{
    int TextureCount = 128;
    EPictureType::EPictureType PictureType = EPictureType::PNG;
    int SequenceRows = 1, SequenceCols = 1;
    m_pScreenQuad = CScreenQuad::getOrCreate();
    m_pSnowSceneSeqFramePlayer = new CSequenceFramePlayer("textures/SnowCover", SequenceRows, SequenceCols, TextureCount, PictureType);
    if(!m_pSnowSceneSeqFramePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SnowScene initialization falied.");
        return ;
    };
    m_pSnowSceneSeqFramePlayer->setFrameRate(5);
    m_pSnowSceneSeqFramePlayer->setLoopPlayback(false);
}

void CSnowSceneRenderer::renderScene(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.1f,0.1f,0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_pSnowSceneSeqFramePlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
    m_pSnowSceneSeqFramePlayer->draw(m_pScreenQuad);
}