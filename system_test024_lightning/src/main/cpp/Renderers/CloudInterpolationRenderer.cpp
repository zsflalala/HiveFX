#include "CloudInterpolationRenderer.h"
#include "LightningRenderer.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <GLES3/gl3.h>
#include <cassert>
#include <algorithm>
#include "Common.h"
#include "TimeUtils.h"
#include "ScreenQuad.h"
#include "SequenceFramePlayer.h"
#include "JsonReader.h"

using namespace hiveVG;

CCloudInterpolationRenderer::CCloudInterpolationRenderer(android_app *vApp) : m_pApp(vApp)
{
    __initAlgorithm();
}

CCloudInterpolationRenderer::~CCloudInterpolationRenderer()
{
    if (m_pScreenQuad)
        delete m_pScreenQuad;
    if(m_pCloudPlayer)
        delete m_pCloudPlayer;
}

void CCloudInterpolationRenderer::render(int vWindowWidth, int vWindowHeight)
{
    m_CurrentTime    = CTimeUtils::getCurrentTime();
    double DeltaTime = m_CurrentTime - m_LastFrameTime;
    m_LastFrameTime  = m_CurrentTime;

    glClearColor(0.1f,0.1f,0.1f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    LOG_INFO(TAG_KEYWORD::RENDERER_TAG, "%lf", DeltaTime);

    m_pCloudPlayer->updateInterpolationFrame(DeltaTime);
    m_pCloudPlayer->drawInterpolation(m_pScreenQuad);
//    m_pCloudPlayer->updateFrameAndUV(DeltaTime);
//    m_pCloudPlayer->draw(m_pScreenQuad);
}

void CCloudInterpolationRenderer::__initAlgorithm()
{
    m_pScreenQuad = CScreenQuad::getOrCreate();

    std::string ConfigFilePath = "configs/CloudInterpolation.json";
    CJsonReader JsonConfig = CJsonReader(ConfigFilePath);

    Json::Value LightningConfig = JsonConfig.getObject("cloud");

    std::string FramePath    = LightningConfig["frames_path"].asString();
    std::string FrameType    = LightningConfig["frames_type"].asString();
    int         FrameCount   = LightningConfig["frames_count"].asInt();
    int         SequenceRows = LightningConfig["ranks"]["rows"].asInt();
    int         SequenceCols = LightningConfig["ranks"]["cols"].asInt();
    std::string PlayMode     = LightningConfig["play_mode"].asString();

    float       PlayFPS      = LightningConfig["fps"].asFloat();
    bool        IsLoop       = LightningConfig["loop"].asBool();

    EPictureType::EPictureType PictureType = EPictureType::FromString(FrameType);
    EPlayType::EPlayType PlayType = EPlayType::FromString(PlayMode);

    m_pCloudPlayer = new CSequenceFramePlayer(FramePath, SequenceRows, SequenceCols, FrameCount, PictureType);
    if(!m_pCloudPlayer->initTextureAndShaderProgram(SingleTexPlayVert, SeqTexPlayInterpolation))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return;
    }
    m_pCloudPlayer->setFrameRate(PlayFPS);
    m_pCloudPlayer->setLoopPlayback(IsLoop);
    if (PlayType == EPlayType::PARTIAL)
    {
        glm::vec2   UVOffset     = glm::vec2(LightningConfig["position"]["x"].asFloat(),
                                             LightningConfig["position"]["y"].asFloat());
        float       Scale        = LightningConfig["scale"].asFloat();
        glm::vec2   MoveSpeed    = glm::vec2(LightningConfig["moving_speed"][0].asFloat(),
                                             LightningConfig["moving_speed"][1].asFloat());

        m_pCloudPlayer->setIsMoving(true);
        m_pCloudPlayer->setScreenUVOffset(UVOffset);
        m_pCloudPlayer->setScreenUVScale(glm::vec2(Scale, Scale));
        m_pCloudPlayer->setScreenUVMovingSpeed(MoveSpeed);
    }

    m_LastFrameTime = CTimeUtils::getCurrentTime();
}
