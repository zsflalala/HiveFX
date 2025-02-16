#include "CloudRendererBillBoard.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "Common.h"
#include "ScreenQuad.h"
#include "SequenceFramePlayer.h"
#include "BillBoardManager.h"

using namespace hiveVG;

CCloudRendererBillBoard::CCloudRendererBillBoard(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CCloudRendererBillBoard::~CCloudRendererBillBoard()
{
    if (m_pScreenQuad) delete m_pScreenQuad;
}

void CCloudRendererBillBoard::__initAlgorithm()
{
    m_pScreenQuad = CScreenQuad::getOrCreate();

    std::string TexRootPath = "Textures/Cloud2Scene";
    int SequenceRows = 1, SequenceCols = 1;
    int TextureCount = 180;
    CSequenceFramePlayer* Cloud2Scene = new CSequenceFramePlayer(TexRootPath, SequenceRows, SequenceCols, TextureCount);
    if (!Cloud2Scene->initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    }

    TexRootPath = "Textures/Cloud4Scene";
    TextureCount = 176;
    CSequenceFramePlayer* Cloud4Scene = new CSequenceFramePlayer(TexRootPath, SequenceRows, SequenceCols, TextureCount);
    if (!Cloud4Scene->initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    }

    TexRootPath = "Textures/Cloud5Scene";
    TextureCount = 128;
    CSequenceFramePlayer* Cloud5Scene = new CSequenceFramePlayer(TexRootPath, SequenceRows, SequenceCols, TextureCount);
    if (!Cloud5Scene->initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    }

    m_pCloudManager = std::make_unique<CBillBoardManager>();
    m_pCloudManager->pushBack(Cloud2Scene);
    m_pCloudManager->pushBack(Cloud4Scene);
    m_pCloudManager->pushBack(Cloud5Scene);
    m_pCloudManager->initSequenceState();

    m_LastFrameTime = __getCurrentTime();
}

void CCloudRendererBillBoard::renderScene(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = __getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.8f,0.8f,0.8f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_pCloudManager->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
    m_pCloudManager->updateSequenceState(DeltaTime);
    m_pCloudManager->setFrameRate(8.0f);
    static int PlayersNum = m_pCloudManager->getSequencePlayerLength();
    static std::vector<glm::vec2> ScreenUVScale(PlayersNum, glm::vec2(1.0f, 1.0f));
    for (int i = 0; i < PlayersNum; i++)
    {
        ScreenUVScale[i].y = ScreenUVScale[i].x / m_pCloudManager->getImageAspectRatioAt(i);
        m_pCloudManager->setImageAspectRatioAt(i, ScreenUVScale[i]);
    }
    m_pCloudManager->draw(m_pScreenQuad);
}

double CCloudRendererBillBoard::__getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}