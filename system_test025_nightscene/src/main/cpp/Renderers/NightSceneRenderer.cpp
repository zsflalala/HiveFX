#include "NightSceneRenderer.h"
#include <GLES3/gl3.h>
#include <cassert>
#include <algorithm>
#include "Common.h"
#include "TimeUtils.h"
#include "ScreenQuad.h"
#include "JsonReader.h"
#include "NightSceneSequencePlayer.h"

using namespace hiveVG;

CNightSceneRenderer::CNightSceneRenderer()
{
    __initAlgorithm();
}

CNightSceneRenderer::~CNightSceneRenderer()
{
    if (m_pScreenQuad)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
    if (m_pNightSceneSequencePlayer)
    {
        delete m_pNightSceneSequencePlayer;
        m_pNightSceneSequencePlayer = nullptr;
    }
}

void CNightSceneRenderer::render()
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.1f,0.1f,0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_pNightSceneSequencePlayer->updateFrameAndUV(DeltaTime);
    m_pNightSceneSequencePlayer->draw(m_pScreenQuad);
}

void CNightSceneRenderer::__initAlgorithm()
{
    m_pScreenQuad = CScreenQuad::getOrCreate();

    std::string ConfigFilePath = "configs/NightSceneConfig.json";
    CJsonReader JsonConfig = CJsonReader(ConfigFilePath);

    Json::Value LightningConfig = JsonConfig.getObject("raining");

    std::string FramePath    = LightningConfig["frames_path"].asString();
    std::string FrameType    = LightningConfig["frames_type"].asString();
    int         FrameCount   = LightningConfig["frames_count"].asInt();
    int         SequenceRows = LightningConfig["ranks"]["rows"].asInt();
    int         SequenceCols = LightningConfig["ranks"]["cols"].asInt();
    std::string PlayMode     = LightningConfig["play_mode"].asString();

    int         PlayFPS      = LightningConfig["fps"].asInt();
    bool        IsLoop       = LightningConfig["loop"].asBool();

    EPictureType::EPictureType PictureType = EPictureType::FromString(FrameType);
    EPlayType::EPlayType PlayType = EPlayType::FromString(PlayMode);

    m_pNightSceneSequencePlayer = new CNightSceneSequencePlayer(FramePath, SequenceRows, SequenceCols, FrameCount, PictureType);
    if(!m_pNightSceneSequencePlayer->initTextureAndShaderProgram())
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return;
    }
    m_pNightSceneSequencePlayer->setFrameRate(PlayFPS);
    m_pNightSceneSequencePlayer->setLoopPlayback(IsLoop);
    if (PlayType == EPlayType::PARTIAL)
    {
        glm::vec2   UVOffset     = glm::vec2(LightningConfig["position"]["x"].asFloat(),
                                             LightningConfig["position"]["y"].asFloat());
        float       Scale        = LightningConfig["scale"].asFloat();
        glm::vec2   MoveSpeed    = glm::vec2(LightningConfig["moving_speed"][0].asFloat(),
                                             LightningConfig["moving_speed"][1].asFloat());

        m_pNightSceneSequencePlayer->setIsMoving(true);
        m_pNightSceneSequencePlayer->setScreenUVOffset(UVOffset);
        m_pNightSceneSequencePlayer->setScreenUVScale(glm::vec2(Scale, Scale));
        m_pNightSceneSequencePlayer->setScreenUVMovingSpeed(MoveSpeed);
    }

    std::string CloudTex = JsonConfig.getString("background");
    m_pNightSceneSequencePlayer->initBackground(CloudTex);
    m_LastFrameTime = CTimeUtils::getCurrentTime();
}

void CNightSceneRenderer::setRenderChannel(ERenderChannel vChannel)
{
    m_pNightSceneSequencePlayer->setCurrentChannel(static_cast<std::uint8_t>(vChannel));
}
