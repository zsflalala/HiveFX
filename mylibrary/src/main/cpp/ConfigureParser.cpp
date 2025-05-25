#include "ConfigureParser.h"
#include "JsonReader.h"
#include "SequenceFramePlayer.h"

using namespace hiveVG;

CSequenceFramePlayer* CConfigureParser::createSequenceFramePlayer(const CJsonReader *vJsonReader)
{
    std::string FramePath    = vJsonReader->getString("frames_path");
    std::string FrameType    = vJsonReader->getString("frames_type");
    int         FrameCount   = vJsonReader->getInt("frames_count");

    Json::Value Ranks        = vJsonReader->getArray("ranks");
    int         SequenceRows = Ranks["rows"].asInt();
    int         SequenceCols = Ranks["cols"].asInt();

    std::string PlayMode     = vJsonReader->getString("play_mode");

    int         PlayFPS      = vJsonReader->getInt("fps");
    bool        IsLoop       = vJsonReader->getBool("loop");

    EPictureType::EPictureType PictureType = EPictureType::FromString(FrameType);
    EPlayType::EPlayType PlayType = EPlayType::FromString(PlayMode);

    CSequenceFramePlayer* pSequenceFramePlayer = new CSequenceFramePlayer(FramePath, SequenceRows, SequenceCols, FrameCount, PictureType);
    if(!pSequenceFramePlayer->initTexture())
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return nullptr;
    }
    pSequenceFramePlayer->setFrameRate(PlayFPS);
    pSequenceFramePlayer->setLoopPlayback(IsLoop);
    if (PlayType == EPlayType::PARTIAL)
    {
        Json::Value Position  = vJsonReader->getArray("position");
        glm::vec2   UVOffset  = glm::vec2(Position["x"].asFloat(),
                                          Position["y"].asFloat());
        float       Scale     = vJsonReader->getFloat("scale");

        Json::Value MovingSpeedArray = vJsonReader->getArray("moving_speed");
        glm::vec2   MoveSpeed = glm::vec2(MovingSpeedArray[0].asFloat(),
                                          MovingSpeedArray[1].asFloat());

        pSequenceFramePlayer->setIsMoving(true);
        pSequenceFramePlayer->setScreenUVOffset(UVOffset);
        pSequenceFramePlayer->setScreenUVScale(glm::vec2(Scale, Scale));
        pSequenceFramePlayer->setScreenUVMovingSpeed(MoveSpeed);
    }
    return pSequenceFramePlayer;
}

CSequenceFramePlayer *CConfigureParser::createSequencePlayerForHuawei(const CJsonReader *vJsonReader, std::string& voVert, std::string& voFrag)
{
    std::string FramePath      = vJsonReader->getString("frames_path");
    std::string FrameType = vJsonReader->getString("frames_type");
    int TextureCount      = vJsonReader->getInt("frames_count");
    int OneTextureFrames  = vJsonReader->getInt("one_texture_frames");
    float FramePerSecond  = vJsonReader->getFloat("fps");
    std::string VertexShader = vJsonReader->getString("vertex_shader");
    std::string FragShader   = vJsonReader->getString("fragment_shader");
    EPictureType::EPictureType PictureType = EPictureType::FromString(FrameType);

    CSequenceFramePlayer* pSequenceFramePlayer = new CSequenceFramePlayer(FramePath, TextureCount, OneTextureFrames, FramePerSecond, PictureType);
    if(!pSequenceFramePlayer->initTextureAndShaderProgram(VertexShader, FragShader))
    {
        LOG_ERROR(hiveVG::TAG_KEYWORD::SEQFRAME_RENDERER_TAG, "SequencePlay initialization falied.");
        return nullptr;
    }
    return pSequenceFramePlayer;
}