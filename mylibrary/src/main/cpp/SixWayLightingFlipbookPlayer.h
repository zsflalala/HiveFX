#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "Common.h"
#include "SequenceFramePlayer.h"
namespace hiveVG
{
    class CTexture2D;
    class CShaderProgram;
    class CBoard;
    struct SSequenceState;



    class CSixWayLightingFlipbookPlayer : public CSequenceFramePlayer{
    public:
        CSixWayLightingFlipbookPlayer(const std::string &vTextureRootPath, int vSequenceRows, int vSequenceCols, int vTextureCount, EPictureType::EPictureType vPictureType = EPictureType::PNG);
        CSixWayLightingFlipbookPlayer(const std::string &vTextureRootPath, int vSequenceRows, int vSequenceCols, int vTextureCount, bool vUseCompressedPNG = false);
        ~CSixWayLightingFlipbookPlayer() override;


        bool initTextureAndShaderProgram(bool vIsCompressed = false,bool vFlipYAxis = false) override;
        void updateFrameIndex(double vDeltaTime);
        void updateFrameAndUV(int vWindowWidth, int vWindowHeight, double vDeltaTime) override{
            updateFrameIndex(vDeltaTime);
        }
        void draw(CBoard* vBoard);
        void draw(CScreenQuad* vQuad) override{
            draw(reinterpret_cast<CBoard*>(vQuad));
        }
    private:
        void __initSequenceParams() override;

        struct SSixWayLightingTexture{
            CTexture2D* m_pSixWayLightingTextureA;
            CTexture2D* m_pSixWayLightingTextureB;
            CTexture2D* m_pSixWayLightingTextureAlbedo;
        };

        std::shared_ptr<CTexture2D> m_pSixWayLightingTextureA;
        std::shared_ptr<CTexture2D> m_pSixWayLightingTextureB;
        std::shared_ptr<CTexture2D> m_pSixWayLightingTextureAlbedo;

        float           m_CurrentFrameIndexFraction = 0.0f;

        std::vector<SSixWayLightingTexture*> m_SixWayLightingTextures;
        CShaderProgram*          m_pShaderProgram = nullptr;
    };
}