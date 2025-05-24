#pragma once
#include "SequenceFramePlayer.h"

namespace hiveVG
{
    class CTexture2D;

    class CLightningSequencePlayer : public CSequenceFramePlayer
    {
    public:
        CLightningSequencePlayer(const std::string& vTextureRootPath, int vSequenceRows, int vSequenceCols, int vTextureCount, EPictureType::EPictureType vPictureType = EPictureType::PNG);

        CLightningSequencePlayer(const std::string &vTextureRootPath, int vTextureCount,
                                 int vOneTextureFrames, float vFrameSeconds,
                                 EPictureType::EPictureType vPictureType);

        ~CLightningSequencePlayer();

        void initBackground(const std::string& vTexturePath);
        bool initTextureAndShaderProgram();
        void draw(CScreenQuad *vQuad);

    private:
        CTexture2D * m_pStaticCloud = nullptr;
    };

}
