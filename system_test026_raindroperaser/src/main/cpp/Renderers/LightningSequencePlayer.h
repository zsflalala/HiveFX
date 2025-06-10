#pragma once

#include <random>
#include "SequenceFramePlayer.h"

namespace hiveVG
{
    class CTexture2D;

    class CLightningSequencePlayer : public CSequenceFramePlayer
    {
    public:
        CLightningSequencePlayer(const std::string& vTextureRootPath, int vSequenceRows, int vSequenceCols, int vTextureCount, EPictureType::EPictureType vPictureType = EPictureType::PNG);
        CLightningSequencePlayer(const std::string &vTextureRootPath, int vTextureCount, int vOneTextureFrames, float vFrameSeconds, EPictureType::EPictureType vPictureType);
        ~CLightningSequencePlayer();

        void setLightningMode(bool vLightningInFront) { m_LightningInFront = vLightningInFront;};

        void initBackground(const std::string& vTexturePath);
        bool initTextureAndShaderProgram() override;
        void updateFrameAndUV(double vDeltaTime) override;
        void updateQuantizationFrame(double vDeltaTime) override;
        void draw(CScreenQuad *vQuad) override;

    private:
        void __randomizeLightningParameters();
        void __resetPlayback();

        CTexture2D* m_pStaticCloud = nullptr;
        bool        m_LightningInFront = false;
        bool        m_IsWaiting        = false;
        double      m_WaitTime         = 0.0;
        double      m_TargetWaitTime   = 0.0;

        std::mt19937 m_Rng{std::random_device{}()};
        std::uniform_real_distribution<float> m_ScaleDist{0.6f, 1.4f};
        std::uniform_real_distribution<float> m_WaitDist{1.0f, 3.0f};
        std::uniform_int_distribution<int>    m_BoolDist{0, 1};
    };
}