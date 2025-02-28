#include "FullScreenSequenceBlendRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "Common.h"
#include "ScreenQuad.h"
#include "Texture2D.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"
#include "TextureBlender.h"
#include "ShaderProgram.h"
#include "BillBoardManager.h"

using namespace hiveVG;

CFullScreenSequenceBlendRenderer::CFullScreenSequenceBlendRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CFullScreenSequenceBlendRenderer::~CFullScreenSequenceBlendRenderer()
{
    if (m_pScreenQuad) delete m_pScreenQuad;
    if (m_pTexBlender) delete m_pTexBlender;
    if (m_pBackground) delete m_pBackground;
    if (m_pMediumShot) delete m_pMediumShot;
    if (m_pForeSequFraPlayer) delete m_pForeSequFraPlayer;
    if (m_pBackSequFraPlayer) delete m_pBackSequFraPlayer;
}

void CFullScreenSequenceBlendRenderer::render(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = __getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.1f,0.2f,0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    std::function<void()> SequFrameDrawCallFunc;

    // 第四层：背景序列帧
    if (m_showLayer4)
    {
        SequFrameDrawCallFunc = std::bind(&CFullScreenSequenceBlendRenderer::__SequenceFrameDrawCallFunc,
                                                                this, m_pBackSequFraPlayer, vWindowWidth, vWindowHeight, DeltaTime);
        m_pTexBlender->drawAndBlend(SequFrameDrawCallFunc);
    }

    // 第三层：中景图片
    if(m_showLayer3)
    {
        std::function<void()> SingleTexDrawCallFunc = [this]()
        {
            m_pMediumShot->updateFrame();
            m_pScreenQuad->bindAndDraw();
        };
        m_pTexBlender->drawAndBlend(SingleTexDrawCallFunc);
    }

    if(m_showLayer2)
    {
        SequFrameDrawCallFunc = std::bind(&CFullScreenSequenceBlendRenderer::__BillBoardDrawCallFunc,
                                          this, vWindowWidth, vWindowHeight, DeltaTime);
        m_pTexBlender->drawAndBlend(SequFrameDrawCallFunc);
    }

    // 第一层：前景序列帧
    if(m_showLayer1)
    {
        SequFrameDrawCallFunc = std::bind(&CFullScreenSequenceBlendRenderer::__SequenceFrameDrawCallFunc,
                                          this, m_pForeSequFraPlayer, vWindowWidth, vWindowHeight, DeltaTime);
        m_pTexBlender->drawAndBlend(SequFrameDrawCallFunc);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_pTexBlender->blitToScreen();
    //m_pTexBlender->blitSrcToScreen();
}

void CFullScreenSequenceBlendRenderer::__initAlgorithm()
{
    m_pScreenQuad = CScreenQuad::getOrCreate();

    m_pBackground = CTexture2D::loadTexture(m_pApp->activity->assetManager, "Textures/park.png");

    m_pMediumShot = new CSingleTexturePlayer("Textures/Background.png");
    m_pMediumShot->initTextureAndShaderProgram(m_pApp->activity->assetManager);

    m_pForeSequFraPlayer = new CSequenceFramePlayer("Textures/Rain", 1, 1, 64, EPictureType::PNG);
    if(!m_pForeSequFraPlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "Foreground sequence player initialization falied.");
        return ;
    }

    m_pBackSequFraPlayer = new CSequenceFramePlayer("Textures/Snow", 1, 1, 64, EPictureType::PNG);
    if(!m_pBackSequFraPlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "Background sequence player initialization falied.");
        return ;
    }

    __initBillBoardManager();

    m_pTexBlender = new CTextureBlender();
    int Width, Height;
    assert(m_pApp->window != nullptr);
    if (m_pApp->window != nullptr) {
        Width = ANativeWindow_getWidth(m_pApp->window);
        Height = ANativeWindow_getHeight(m_pApp->window);
    }

    m_pTexBlender->init(m_pApp->activity->assetManager,Width, Height);

    m_LastFrameTime = __getCurrentTime();
}

double CFullScreenSequenceBlendRenderer::__getCurrentTime() {
    struct timeval tv{};
    gettimeofday(&tv, nullptr);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void CFullScreenSequenceBlendRenderer::__SequenceFrameDrawCallFunc(CSequenceFramePlayer* vSequFraPlayer, int vWindowWidth, int vWindowHeight, float vDeltaTime)
{
    vSequFraPlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, vDeltaTime);
    vSequFraPlayer->draw(m_pScreenQuad);
}

void CFullScreenSequenceBlendRenderer::__initBillBoardManager()
{
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
}

void CFullScreenSequenceBlendRenderer::__BillBoardDrawCallFunc(int vWindowWidth, int vWindowHeight, float vDeltaTime)
{
    m_pCloudManager->updateFrameAndUV(vWindowWidth, vWindowHeight, vDeltaTime);
    m_pCloudManager->updateSequenceState(vDeltaTime);
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

void CFullScreenSequenceBlendRenderer::changeLayerStatus(int vIndex)
{
    switch (vIndex)
    {
        case(1):
            m_showLayer1 = !m_showLayer1;
            break;
        case(2):
            m_showLayer2 = !m_showLayer2;
            break;
        case(3):
            m_showLayer3 = !m_showLayer3;
            break;
        case(4):
            m_showLayer4 = !m_showLayer4;
            break;
    }
}
