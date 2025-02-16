#include "SnowRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "Common.h"
#include "ScreenQuad.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"
#include "BillBoardManager.h"

using namespace hiveVG;

CSnowRenderer::CSnowRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CSnowRenderer::~CSnowRenderer()
{
    if (m_pScreenQuad)     delete m_pScreenQuad;
    if (m_pNearSnowPlayer) delete m_pNearSnowPlayer;
    if (m_pFarSnowPlayer)  delete m_pFarSnowPlayer;
}

void CSnowRenderer::__initAlgorithm()
{
    m_pScreenQuad = CScreenQuad::getOrCreate();

    EPictureType PictureType = EPictureType::PNG;
    std::string  TexRootPath = "Textures/NearSnow";
    int SequenceRows = 1, SequenceCols = 1, TextureCount = 140;
    m_pNearSnowPlayer = new CSequenceFramePlayer(TexRootPath, SequenceRows, SequenceCols, TextureCount, PictureType);
    if(!m_pNearSnowPlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    };
    m_pNearSnowPlayer->setFrameRate(60.0f);

    TexRootPath = "Textures/FarSnow";
    m_pFarSnowPlayer = new CSequenceFramePlayer(TexRootPath, SequenceRows, SequenceCols, TextureCount, PictureType);
    if(!m_pFarSnowPlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    };
    m_pFarSnowPlayer->setFrameRate(60.0f);

    m_pBackgroundPlayer = new CSingleTexturePlayer("Textures/snowScene.png");
    m_pBackgroundPlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager);
    m_LastFrameTime = __getCurrentTime();
}

void CSnowRenderer::renderScene(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = __getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.1f,0.1f,0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_pFarSnowPlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
    m_pFarSnowPlayer->draw(m_pScreenQuad);
    m_pBackgroundPlayer->updateFrame();
    m_pScreenQuad->bindAndDraw();
    m_pNearSnowPlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
    m_pNearSnowPlayer->draw(m_pScreenQuad);
}

double CSnowRenderer::__getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}