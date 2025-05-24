#include "LightningRenderer.h"
#include <GLES3/gl3.h>
#include <cassert>
#include <algorithm>
#include "Common.h"
#include "TimeUtils.h"
#include "ScreenQuad.h"
#include "SingleTexturePlayer.h"
#include "JsonReader.h"
#include "LightningSequencePlayer.h"

using namespace hiveVG;

CLightningRenderer::CLightningRenderer()
{
    __initAlgorithm();
}

CLightningRenderer::~CLightningRenderer()
{
    if (m_pScreenQuad)
    {
        CScreenQuad::destroy();
        m_pScreenQuad = nullptr;
    }
    if (m_pLightningPlayer)
    {
        delete m_pLightningPlayer;
        m_pLightningPlayer = nullptr;
    }
}

void CLightningRenderer::render()
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.1f,0.1f,0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_pLightningPlayer->updateFrameAndUV(DeltaTime);
    m_pLightningPlayer->draw(m_pScreenQuad);
}

void CLightningRenderer::__initAlgorithm()
{
    m_pScreenQuad = CScreenQuad::getOrCreate();

    std::string ConfigFilePath = "configs/LightningConfig.json";
    CJsonReader JsonConfig = CJsonReader(ConfigFilePath);

    Json::Value LightningConfig = JsonConfig.getObject("lightning");

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

    m_pLightningPlayer = new CLightningSequencePlayer(FramePath, SequenceRows, SequenceCols, FrameCount, PictureType);
    if(!m_pLightningPlayer->initTextureAndShaderProgram())
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return;
    }
    m_pLightningPlayer->setFrameRate(PlayFPS);
    m_pLightningPlayer->setLoopPlayback(IsLoop);
    if (PlayType == EPlayType::PARTIAL)
    {
        glm::vec2   UVOffset     = glm::vec2(LightningConfig["position"]["x"].asFloat(),
                                             LightningConfig["position"]["y"].asFloat());
        float       Scale        = LightningConfig["scale"].asFloat();
        glm::vec2   MoveSpeed    = glm::vec2(LightningConfig["moving_speed"][0].asFloat(),
                                             LightningConfig["moving_speed"][1].asFloat());

        m_pLightningPlayer->setIsMoving(true);
        m_pLightningPlayer->setScreenUVOffset(UVOffset);
        m_pLightningPlayer->setScreenUVScale(glm::vec2(Scale, Scale));
        m_pLightningPlayer->setScreenUVMovingSpeed(MoveSpeed);
    }

    std::string CloudTex = JsonConfig.getString("cloud");
    m_pLightningPlayer->initBackground(CloudTex);

    m_LastFrameTime = CTimeUtils::getCurrentTime();
}