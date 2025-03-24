#include "RendererWithExternalStorage.h"
#include "Common.h"
#include "TimeUtils.h"
#include "ScreenQuad.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"

hiveVG::CRendererWithExternalStorage::CRendererWithExternalStorage(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

hiveVG::CRendererWithExternalStorage::~CRendererWithExternalStorage()
{
    if(m_pBackground) delete m_pBackground;
    if(m_pSmallRain)  delete m_pSmallRain;
}

void hiveVG::CRendererWithExternalStorage::renderScene(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.35f,0.35f,0.35f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if(m_pBackground)
    {
        m_pBackground->updateFrame();
        m_pScreenQuad->bindAndDraw();
    }

    if(m_pSmallRain)
    {
        m_pSmallRain->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
        m_pSmallRain->draw(m_pScreenQuad);
    }
}

void hiveVG::CRendererWithExternalStorage::__initAlgorithm()
{
    m_pScreenQuad = CScreenQuad::getOrCreate();

    auto StoragePath = CAppContext::getStoragePath();

//    m_pBackground = new CSingleTexturePlayer(StoragePath + "textures/Background.png");
//    if(!m_pBackground->initTextureAndShaderProgram())
//    {
//        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "Background initialization falied.");
//        return ;
//    }

    m_pSmallRain = new CSequenceFramePlayer(StoragePath + "textures/SmallRain_fore", 1, 1, 64, EPictureType::EPictureType::PNG);
    if(!m_pSmallRain->initTextureAndShaderProgram())
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    }
}
