#include "SnowRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <android/asset_manager.h>
#include <json/json.h>
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
    if (m_pScreenQuad)              delete m_pScreenQuad;
    if (m_pSmallSnowForePlayer)     delete m_pSmallSnowForePlayer;
    if (m_pSmallSnowBackPlayer)     delete m_pSmallSnowBackPlayer;
    if (m_pBigSnowForePlayer)       delete m_pBigSnowForePlayer;
    if (m_pBigSnowBackPlayer)       delete m_pBigSnowBackPlayer;
    if (m_pSnowSceneSeqFramePlayer) delete m_pSnowSceneSeqFramePlayer;
}

void CSnowRenderer::__initAlgorithm()
{
    std::string fileName = "configs/BasicFramePlayerConfig.json";

    AAsset* asset = AAssetManager_open(m_pApp->activity->assetManager, fileName.c_str(), AASSET_MODE_BUFFER);
    if (!asset) {
        LOG_ERROR(hiveVG::TAG_KEYWORD::MAIN_TAG, "file is not open");
        return ;
    }
    off_t assetLength = AAsset_getLength(asset);
    std::vector<char> buffer(assetLength);
    AAsset_read(asset, buffer.data(), assetLength);
    AAsset_close(asset);
    std::string jsonContent = std::string(buffer.begin(), buffer.end());

    Json::Value root;
    Json::CharReaderBuilder reader;
    std::string errs;

    std::istringstream sstream(jsonContent);
    if (!Json::parseFromStream(reader, sstream, &root, &errs)) {
        LOG_ERROR(hiveVG::TAG_KEYWORD::MAIN_TAG, "json file parse failed, %s", errs.c_str());
        return;
    }
    LOG_INFO(hiveVG::TAG_KEYWORD::MAIN_TAG, "Name:  %s", root["name"].asString().c_str());
    LOG_INFO(hiveVG::TAG_KEYWORD::MAIN_TAG, "Age:   %s", root["age"].asString().c_str());
    LOG_INFO(hiveVG::TAG_KEYWORD::MAIN_TAG, "Email: %s", root["email"].asString().c_str());

    m_pScreenQuad = CScreenQuad::getOrCreate();

    EPictureType PictureType = EPictureType::PNG;
    int SequenceRows = 1, SequenceCols = 1, TextureCount = 128;

    std::string  TexRootPath = "Textures/SmallSnow_fore";
    m_pSmallSnowForePlayer = new CSequenceFramePlayer(TexRootPath, SequenceRows, SequenceCols, TextureCount, PictureType);
    if(!m_pSmallSnowForePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    };
    m_pSmallSnowForePlayer->setFrameRate(60.0f);

    TexRootPath = "Textures/SmallSnow_back";
    m_pSmallSnowBackPlayer = new CSequenceFramePlayer(TexRootPath, SequenceRows, SequenceCols, TextureCount, PictureType);
    if(!m_pSmallSnowBackPlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    };
    m_pSmallSnowBackPlayer->setFrameRate(60.0f);

    TexRootPath = "Textures/BigSnow_fore";
    m_pBigSnowForePlayer = new CSequenceFramePlayer(TexRootPath, SequenceRows, SequenceCols, TextureCount, PictureType);
    if(!m_pBigSnowForePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    };
    m_pBigSnowForePlayer->setFrameRate(60.0f);

    TexRootPath = "Textures/BigSnow_back";
    m_pBigSnowBackPlayer = new CSequenceFramePlayer(TexRootPath, SequenceRows, SequenceCols, TextureCount, PictureType);
    if(!m_pBigSnowBackPlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return ;
    };
    m_pBigSnowBackPlayer->setFrameRate(60.0f);

    m_pSnowSceneSeqFramePlayer = new CSequenceFramePlayer("Textures/SnowCover", SequenceRows, SequenceCols, TextureCount, PictureType);
    if(!m_pSnowSceneSeqFramePlayer->initTextureAndShaderProgram(m_pApp->activity->assetManager))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SnowScene initialization falied.");
        return ;
    };
    m_pSnowSceneSeqFramePlayer->setFrameRate(5);

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

    if (m_EnableSmallSnowBack)
    {
        m_pSmallSnowBackPlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
        m_pSmallSnowBackPlayer->draw(m_pScreenQuad);
    }
    if (m_EnableBigSnowBack)
    {
        m_pBigSnowBackPlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
        m_pBigSnowBackPlayer->draw(m_pScreenQuad);
    }
    m_pSnowSceneSeqFramePlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
    m_pSnowSceneSeqFramePlayer->draw(m_pScreenQuad);
    if (m_EnableSmallSnowFore)
    {
        m_pSmallSnowForePlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
        m_pSmallSnowForePlayer->draw(m_pScreenQuad);
    }
    if (m_EnableBigSnowFore)
    {
        m_pBigSnowForePlayer->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
        m_pBigSnowForePlayer->draw(m_pScreenQuad);
    }
}

void CSnowRenderer::handleInput(ERenderType vInputType, bool vIsPointerDown)
{
    if (vInputType == ERenderType::SMALL_SNOW_FORE)
    {
        if (vIsPointerDown && !m_PreviousPointerState) m_EnableSmallSnowFore = !m_EnableSmallSnowFore;
    }
    else if (vInputType == ERenderType::SMALL_SNOW_BACK)
    {
        if (vIsPointerDown && !m_PreviousPointerState) m_EnableSmallSnowBack = !m_EnableSmallSnowBack;
    }
    else if (vInputType == ERenderType::BIG_SNOW_FORE)
    {
        if (vIsPointerDown && !m_PreviousPointerState) m_EnableBigSnowFore   = !m_EnableBigSnowFore;
    }
    else if (vInputType == ERenderType::BIG_SNOW_BACK)
    {
        if (vIsPointerDown && !m_PreviousPointerState) m_EnableBigSnowBack   = !m_EnableBigSnowBack;
    }
    m_PreviousPointerState = vIsPointerDown;
}

double CSnowRenderer::__getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}