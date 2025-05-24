#pragma once

#include "SequenceFramePlayer.h"

namespace hiveVG
{
    class CTexture2D;

    class CLightningSequencePlayer : public CSequenceFramePlayer
    {
    public:
        CLightningSequencePlayer(const std::string& vTextureRootPath, int vSequenceRows, int vSequenceCols, int vTextureCount, EPictureType::EPictureType vPictureType = EPictureType::PNG);
        ~CLightningSequencePlayer();

        void initBackground(const std::string& vTexturePath);
        bool initTextureAndShaderProgram() override;
        void draw(CScreenQuad *vQuad) override;

    private:
        CTexture2D* m_pStaticCloud = nullptr;
    };
}