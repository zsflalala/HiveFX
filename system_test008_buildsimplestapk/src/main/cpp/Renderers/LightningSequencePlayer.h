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
        bool initTextureAndShaderProgram() ;
        void updateFrameAndUV(double vDeltaTime) ;
        void updateQuantizationFrame(double vDeltaTime) ;
        void draw(CScreenQuad *vQuad);

    private:
        void __randomizeLightningParameters();
        void __resetPlayback();

        CTexture2D* m_pStaticCloud = nullptr;
        bool        m_LightningInFront = false;
        bool        m_IsWaiting        = false;
        double      m_WaitTime         = 0.0;
        double      m_TargetWaitTime   = 0.0;

        int m_CurrentCloudTexture = 0;
        int m_NextCloudTexture = 1;
        int m_CurrentCloudChannel = 0;
        float m_CloudInterpFactor = 0.0f;
        double m_AccumCloudTime = 0.0;
        int m_OneCloudTexFrames = 4;
        float m_CloudFPS = 10.0f;
        std::vector<CTexture2D *>  m_SeqCloudTextures;
        CSequenceFramePlayer* m_pCloudPlayer             = nullptr;
        int m_CloudSingleTexWidth = 0;
        int m_CloudSingleTexHeight = 0;
        std::mt19937 m_Rng{std::random_device{}()};
        std::uniform_real_distribution<float> m_ScaleDist{0.6f, 1.4f};
        std::uniform_real_distribution<float> m_WaitDist{1.0f, 3.0f};
        std::uniform_int_distribution<int>    m_BoolDist{0, 1};

        void updateCloudSequence(double vDeltaTime);
    };
}