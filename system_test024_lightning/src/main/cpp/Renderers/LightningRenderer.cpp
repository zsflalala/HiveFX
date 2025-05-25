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

//    m_pLightningPlayer->updateFrameAndUV(DeltaTime);
    m_pLightningPlayer->updateQuantizationFrame(DeltaTime);
    m_pLightningPlayer->draw(m_pScreenQuad);
}

void CLightningRenderer::__initAlgorithm()
{
    m_pScreenQuad = CScreenQuad::getOrCreate();

    std::string ConfigFilePath = "configs/LightningConfig.json";
    CJsonReader JsonConfig = CJsonReader(ConfigFilePath);

    Json::Value LightningConfig = JsonConfig.getObject("lightning");

    std::string LightningFramePath        = LightningConfig["frames_path"].asString();
    std::string LightningFrameMaskPath    = LightningConfig["frames_mask_path"].asString();
    std::string LightningFrameType        = LightningConfig["frames_type"].asString();
    int         LightningFrameCount       = LightningConfig["frames_count"].asInt();
    int         LightningOneTextureFrames = LightningConfig["one_texture_frames"].asInt();
    int         LightningSequenceRows     = LightningConfig["ranks"]["rows"].asInt();
    int         LightningSequenceCols     = LightningConfig["ranks"]["cols"].asInt();
    std::string LightningPlayMode         = LightningConfig["play_mode"].asString();
    float       LightningPlayFPS          = LightningConfig["fps"].asFloat();
    bool        LightningIsLoop           = LightningConfig["loop"].asBool();
    bool        LightningInFront          = LightningConfig["lightning_front"].asBool();
    EPictureType::EPictureType PictureType = EPictureType::FromString(LightningFrameType);
    EPlayType::EPlayType PlayType = EPlayType::FromString(LightningPlayMode);

    m_pLightningPlayer = new CLightningSequencePlayer(LightningFramePath, LightningFrameCount, LightningOneTextureFrames, LightningPlayFPS, PictureType);
//    m_pLightningPlayer = new CLightningSequencePlayer(LightningFramePath, LightningSequenceRows, LightningSequenceCols, LightningFrameCount, PictureType);
    if(!m_pLightningPlayer->initTextureAndShaderProgram())
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "LightningPlayer initialization failed.");
        return;
    }
    m_pLightningPlayer->setFrameRate(LightningPlayFPS);
    m_pLightningPlayer->setLoopPlayback(LightningIsLoop);
    m_pLightningPlayer->setLightningMode(LightningInFront);
//    m_pLightningPlayer->setRotationAngle(50.0f);
    if (PlayType == EPlayType::PARTIAL)
    {
        glm::vec2   LightningUVOffset     = glm::vec2(LightningConfig["position"]["x"].asFloat(),
                                             LightningConfig["position"]["y"].asFloat());
        float       LightningScale        = LightningConfig["scale"].asFloat();

        m_pLightningPlayer->setScreenUVOffset(LightningUVOffset);
        m_pLightningPlayer->setScreenUVScale(glm::vec2(LightningScale, LightningScale));
    }

    std::string CloudTex = JsonConfig.getString("cloud");
    m_pLightningPlayer->initBackground(CloudTex);

    m_LastFrameTime = CTimeUtils::getCurrentTime();
}