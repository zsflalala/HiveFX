#include "RendererWithExternalStorage.h"
#include <iomanip>
#include <sstream>
#include "Common.h"
#include "TimeUtils.h"
#include "ScreenQuad.h"
#include "SingleTexturePlayer.h"
#include "SequenceFramePlayer.h"
#include "../Downloader.h"

hiveVG::CRendererWithExternalStorage::CRendererWithExternalStorage(android_app *vApp) : m_pApp(vApp)
{
    __downloadTexture();
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

    m_pBackground = new CSingleTexturePlayer(StoragePath + "textures/snowScene.png");
    if(!m_pBackground->initTextureAndShaderProgram())
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "Background initialization falied.");
        return ;
    }

    m_pSmallRain = new CSequenceFramePlayer(StoragePath + "textures/BigRain_fore", 1, 1, 64, EPictureType::EPictureType::PNG);
    if(!m_pSmallRain->initTextureAndShaderProgram())
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    }
}

void hiveVG::CRendererWithExternalStorage::__downloadTexture()
{
    CDownloader Downloader;
    std::string Url = "https://gitee.com/sidney-chen/hiveFX-Asset/raw/master/textures/snowScene.png";
    std::string Output = CAppContext::getStoragePath() + "textures/snowScene.png";
    bool IsDownloadsucceed = Downloader.downloadTexture(Url,Output);
    if(!IsDownloadsucceed)
    {
        LOG_WARN(TAG_KEYWORD::RENDERER_TAG, "download failed.");
        assert(0);
    }
    else
    {
        LOG_INFO(TAG_KEYWORD::RENDERER_TAG, "Download %s succeed.", Url.c_str());
    }

    std::string BaseUrl = "https://gitee.com/sidney-chen/hiveFX-Asset/raw/master/textures/BigRain_fore/frame_";
    std::string BaseOutputPath = CAppContext::getStoragePath() + "textures/BigRain_fore/frame_";
    for(int i = 1; i <= 64; i++)
    {
        std::stringstream UrlStringstream;
        UrlStringstream << BaseUrl << std::setw(3) << std::setfill('0') << i << ".png";
        std::stringstream OutputStringstream;
        OutputStringstream << BaseOutputPath << std::setw(3) << std::setfill('0') << i << ".png";
        IsDownloadsucceed = Downloader.downloadTexture(UrlStringstream.str(), OutputStringstream.str());
        if(!IsDownloadsucceed)
        {
            LOG_WARN(TAG_KEYWORD::RENDERER_TAG, "download failed.");
            assert(0);
        }
        else
        {
            LOG_INFO(TAG_KEYWORD::RENDERER_TAG, "Download %s succeed.", UrlStringstream.str().c_str());
        }
    }
}
