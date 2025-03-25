#include "BlendRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "Common.h"
#include "TimeUtils.h"
#include "ScreenQuad.h"
#include "Texture2D.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"
#include "TextureBlender.h"
#include "ShaderProgram.h"
#include "BillBoardManager.h"

using namespace hiveVG;

CBlendRenderer::CBlendRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CBlendRenderer::~CBlendRenderer()
{
    if (m_pScreenQuad) delete m_pScreenQuad;
    if (m_pTexBlender) delete m_pTexBlender;
    if (m_pMediumShot) delete m_pMediumShot;
    if (m_pForeSequFraPlayer) delete m_pForeSequFraPlayer;
    if (m_pBackSequFraPlayer) delete m_pBackSequFraPlayer;
}

void CBlendRenderer::render(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.1f,0.2f,0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    std::function<void()> SeqFrameDrawCallFunc;
    // 第四层：背景序列帧
    if (m_ShowLayer4)
    {
        SeqFrameDrawCallFunc = [this, vWindowWidth, vWindowHeight, DeltaTime] { __SequenceFrameDrawCallFunc(m_pBackSequFraPlayer, vWindowWidth, vWindowHeight, DeltaTime); };
        m_pTexBlender->drawAndBlend(SeqFrameDrawCallFunc);
    }

    // 第三层：中景图片
    if(m_ShowLayer3)
    {
        std::function<void()> SingleTexDrawCallFunc = [this]()
        {
            m_pMediumShot->updateFrame();
            m_pScreenQuad->bindAndDraw();
        };
        m_pTexBlender->drawAndBlend(SingleTexDrawCallFunc);
    }

    if(m_ShowLayer2)
    {
        SeqFrameDrawCallFunc = [this, vWindowWidth, vWindowHeight, DeltaTime] { __BillBoardDrawCallFunc(vWindowWidth, vWindowHeight, DeltaTime); };
        m_pTexBlender->drawAndBlend(SeqFrameDrawCallFunc);
    }

    // 第一层：前景序列帧
    if(m_ShowLayer1)
    {
        SeqFrameDrawCallFunc = [this, vWindowWidth, vWindowHeight, DeltaTime] { __SequenceFrameDrawCallFunc(m_pForeSequFraPlayer, vWindowWidth, vWindowHeight, DeltaTime); };
        m_pTexBlender->drawAndBlend(SeqFrameDrawCallFunc);
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_pTexBlender->blit();
}

void CBlendRenderer::__initAlgorithm()
{
    m_pScreenQuad = CScreenQuad::getOrCreate();
    m_pMediumShot = new CSingleTexturePlayer("textures/Background.png");
    m_pMediumShot->initTextureAndShaderProgram();

    std::string TexPath = "textures/BigRain_fore";
    m_pForeSequFraPlayer = new CSequenceFramePlayer(TexPath, 1, 1, 64, EPictureType::PNG);
    if(!m_pForeSequFraPlayer->initTextureAndShaderProgram())
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "Foreground sequence player initialization falied.");
        return ;
    }

    TexPath = "textures/BigRain_back";
    m_pBackSequFraPlayer = new CSequenceFramePlayer(TexPath, 1, 1, 64, EPictureType::PNG);
    if(!m_pBackSequFraPlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "Background sequence player initialization falied.");
        return ;
    }

    m_pTexBlender = new CTextureBlender();
    int Width = 0, Height = 0;
    assert(m_pApp->window != nullptr);
    if (m_pApp->window)
    {
        Width = ANativeWindow_getWidth(m_pApp->window);
        Height = ANativeWindow_getHeight(m_pApp->window);
    }

    CTextureBlender::setAssetManager(m_pApp->activity->assetManager);
    m_pTexBlender->init(Width, Height);

    __initBillBoardManager();

    m_LastFrameTime = CTimeUtils::getCurrentTime();
}

void CBlendRenderer::__SequenceFrameDrawCallFunc(CSequenceFramePlayer* vSequFraPlayer, int vWindowWidth, int vWindowHeight, double vDeltaTime)
{
    vSequFraPlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, vDeltaTime);
    vSequFraPlayer->draw(m_pScreenQuad);
}

void CBlendRenderer::__initBillBoardManager()
{
    std::string TexRootPath = "textures/Cloud2Scene";
    int SequenceRows = 1, SequenceCols = 1;
    int TextureCount = 180;
    auto PictureType = EPictureType::PNG;
    auto* Cloud2Scene = new CSequenceFramePlayer(TexRootPath, SequenceRows, SequenceCols, TextureCount, PictureType);
    if (!Cloud2Scene->initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    }

    TexRootPath = "textures/Cloud4Scene";
    TextureCount = 176;
    auto* Cloud4Scene = new CSequenceFramePlayer(TexRootPath, SequenceRows, SequenceCols, TextureCount, PictureType);
    if (!Cloud4Scene->initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    }

    TexRootPath = "textures/Cloud5Scene";
    TextureCount = 128;
    auto* Cloud5Scene = new CSequenceFramePlayer(TexRootPath, SequenceRows, SequenceCols, TextureCount, PictureType);
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
    m_pCloudManager->setBlender(m_pTexBlender);
    m_pCloudManager->setBlendStatus(true);
}

void CBlendRenderer::__BillBoardDrawCallFunc(int vWindowWidth, int vWindowHeight, double vDeltaTime)
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

void CBlendRenderer::changeLayerStatus(int vIndex)
{
    switch (vIndex)
    {
        case(1):
            m_ShowLayer1 = !m_ShowLayer1;
            break;
        case(2):
            m_ShowLayer2 = !m_ShowLayer2;
            break;
        case(3):
            m_ShowLayer3 = !m_ShowLayer3;
            break;
        case(4):
            m_ShowLayer4 = !m_ShowLayer4;
            break;
        default:
            LOG_ERROR(TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "No acceptable index [%d].", vIndex);
            break;
    }
}

void CBlendRenderer::changeBlendMode(int vMode)
{
    assert(vMode >= 0 && vMode < static_cast<int>(EBlendingMode::EBlendingMode::COUNT));
    auto Mode = static_cast<EBlendingMode::EBlendingMode>(vMode);
    m_pTexBlender->setBlendingMode(Mode);
}