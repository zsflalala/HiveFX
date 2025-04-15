#include "CloudRendererSixWayLighting.h"
#include "Common.h"
#include "TimeUtils.h"
#include "JsonReader.h"
#include "ScreenQuad.h"
#include "Board.h"
#include "BillBoardManager.h"
#include "SixWayLightingFlipbookPlayer.h"

using namespace hiveVG;

CCloudRendererSixWayLighting::CCloudRendererSixWayLighting(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CCloudRendererSixWayLighting::~CCloudRendererSixWayLighting()
{
    if (m_pBoard) delete m_pBoard;
}

void CCloudRendererSixWayLighting::__initAlgorithm()
{
    m_pBoard = CBoard::getOrCreate();
    std::string  FileName = "configs/MainAppConfig.json";
    CJsonReader JsonReader = CJsonReader(FileName);
    Json::Value SixWayLightCloudConfig = JsonReader.getObject("SixWayLightCloud1");
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


  //  std::string CloudFramePathA = SixWayLightCloudConfig["frames_path"][0].asString();
  //  std::string CloudFramePathB = SixWayLightCloudConfig["frames_path"][1].asString();
 //   std::string CloudFramePathAlbedo = SixWayLightCloudConfig["frames_path"][2].asString();
    CloudFramePath = SixWayLightCloudConfig["frames_path"].asString();
    m_PictureType= EPictureType::FromString(SixWayLightCloudConfig["frames_type"].asString());
    CloudFrameCount = SixWayLightCloudConfig["frames_count"].asInt();
    CloudRows = SixWayLightCloudConfig["sequenceRows"].asInt();
    CloudCols = SixWayLightCloudConfig["sequenceCols"].asInt();
    CloudIsLoop  =SixWayLightCloudConfig["loop"].asBool();
    CloudPlayFPS = SixWayLightCloudConfig["fps"].asInt();
    auto* SixWayLightCloud1 = new CSixWayLightingFlipbookPlayer(CloudFramePath, CloudRows, CloudCols, 1,m_PictureType);
    SixWayLightCloud1->initTextureAndShaderProgram(false, true);

/*
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
    Cloud5Scene->initTextureAndShaderProgram();*/

    m_pCloudManager = std::make_unique<CBillBoardManager>();
  //  m_pCloudManager->pushBack(Cloud2Scene);
  //  m_pCloudManager->pushBack(Cloud4Scene);
    m_pCloudManager->pushBack(SixWayLightCloud1);
  //  m_pCloudManager->pushBack(Cloud5Scene);
    m_pCloudManager->initSequenceState();

    m_LastFrameTime = CTimeUtils::getCurrentTime();
}

void CCloudRendererSixWayLighting::renderScene(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.133f,0.509f,0.95f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_pCloudManager->updateFrameAndUV(vWindowWidth, vWindowHeight, DeltaTime);
    m_pCloudManager->setSequenceState(0, true);
    m_pCloudManager->setFrameRate(12.0f);
    static int PlayersNum = m_pCloudManager->getSequencePlayerLength();
    static std::vector<glm::vec2> ScreenUVScale(PlayersNum, glm::vec2(1.0f, 1.0f));
    for (int i = 0; i < PlayersNum; i++)
    {
      //  ScreenUVScale[i].y = ScreenUVScale[i].x / m_pCloudManager->getImageAspectRatioAt(i);
     //   m_pCloudManager->setImageAspectRatioAt(i, ScreenUVScale[i]);

    }
    m_pCloudManager->draw(m_pBoard);
}