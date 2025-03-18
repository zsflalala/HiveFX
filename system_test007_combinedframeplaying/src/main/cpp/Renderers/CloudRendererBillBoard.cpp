#include "CloudRendererBillBoard.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include "Common.h"
#include "TimeUtils.h"
#include "JsonReader.h"
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
    std::string  FileName = "configs/MainAppConfig.json";
    CJsonReader JsonReader = CJsonReader(FileName);
    Json::Value Cloud2SceneConfig = JsonReader.getObject("Cloud2Scene");
    Json::Value Cloud4SceneConfig = JsonReader.getObject("Cloud4Scene");
    Json::Value Cloud5SceneConfig = JsonReader.getObject("Cloud5Scene");

    std::string CloudFramePath = Cloud2SceneConfig["frames_path"].asString();
    m_PictureType= EPictureType::FromString(Cloud2SceneConfig["frames_type"].asString());
    int  CloudFrameCount = Cloud2SceneConfig["frames_count"].asInt();
    int  CloudRows = Cloud2SceneConfig["sequenceRows"].asInt();
    int  CloudCols = Cloud2SceneConfig["sequenceCols"].asInt();
    bool CloudIsLoop  = Cloud2SceneConfig["loop"].asBool();
    int  CloudPlayFPS = Cloud2SceneConfig["fps"].asInt();
    auto* Cloud2Scene = new CSequenceFramePlayer(CloudFramePath, CloudRows, CloudCols, CloudFrameCount,m_PictureType);
    Cloud2Scene->initTextureAndShaderProgram();

    CloudFramePath = Cloud4SceneConfig["frames_path"].asString();
    m_PictureType= EPictureType::FromString(Cloud4SceneConfig["frames_type"].asString());
    CloudFrameCount = Cloud4SceneConfig["frames_count"].asInt();
    CloudRows = Cloud4SceneConfig["sequenceRows"].asInt();
    CloudCols = Cloud4SceneConfig["sequenceCols"].asInt();
    CloudIsLoop  = Cloud4SceneConfig["loop"].asBool();
    CloudPlayFPS = Cloud4SceneConfig["fps"].asInt();
    auto* Cloud4Scene = new CSequenceFramePlayer(CloudFramePath, CloudRows, CloudCols, CloudFrameCount,m_PictureType);
    Cloud4Scene->initTextureAndShaderProgram();

    CloudFramePath = Cloud5SceneConfig["frames_path"].asString();
    m_PictureType= EPictureType::FromString(Cloud5SceneConfig["frames_type"].asString());
    CloudFrameCount = Cloud5SceneConfig["frames_count"].asInt();
    CloudRows = Cloud5SceneConfig["sequenceRows"].asInt();
    CloudCols = Cloud5SceneConfig["sequenceCols"].asInt();
    CloudIsLoop  = Cloud5SceneConfig["loop"].asBool();
    CloudPlayFPS = Cloud5SceneConfig["fps"].asInt();
    auto* Cloud5Scene = new CSequenceFramePlayer(CloudFramePath, CloudRows, CloudCols, CloudFrameCount,m_PictureType);
    Cloud5Scene->initTextureAndShaderProgram();

    m_pCloudManager = std::make_unique<CBillBoardManager>();
    m_pCloudManager->pushBack(Cloud2Scene);
    m_pCloudManager->pushBack(Cloud4Scene);
    m_pCloudManager->pushBack(Cloud5Scene);
    m_pCloudManager->initSequenceState();

    m_LastFrameTime = CTimeUtils::getCurrentTime();
}

void CCloudRendererBillBoard::renderScene(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
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