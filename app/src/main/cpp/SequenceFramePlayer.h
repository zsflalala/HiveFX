#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <android/asset_manager.h>
#include "Common.h"

namespace hiveVG
{
    class CTexture2D;
    class CShaderProgram;
    class CScreenQuad;

    class CSequenceFramePlayer
    {
    public:
        CSequenceFramePlayer(const std::string& vTextureRootPath, int vSequenceRows, int vSequenceCols, int vTextureCount, EPictureType vPictureType = EPictureType::PNG);
        ~CSequenceFramePlayer();

        void setLoopPlayback(bool vLoopTag)   { m_IsLoop = vLoopTag; }
        void setScreenUVScale(const glm::vec2& vScreenScale)   { m_ScreenUVScale = vScreenScale; }
        void setScreenUVOffset(const glm::vec2& vScreenOffset) { m_ScreenUVOffset = vScreenOffset; }
        void setScreenRandScale(const float& vRandScle)        { m_ScreenRandScale = vRandScle; }
        void setFrameRate(int vFrameRate)     { m_FramePerSecond = static_cast<float>(vFrameRate); }
        void setValidFrames(int vValidFrames) { m_ValidFrames = vValidFrames; }
        void setRotationAngle(float vAngle)   { m_RotationAngle = vAngle; }

        int  getSingleTextureWidth()  { return m_SequeceSingleTextureWidth; }
        int  getSingleTextureHeight() { return m_SequeceSingleTextureHeight; }

        [[nodiscard]] bool getLoopState()   const { return m_IsLoop; }
        [[nodiscard]] bool getFinishState() const { return m_IsFinished; }

        bool initTextureAndShaderProgram(AAssetManager* vAssetManager);
        void updateFrameAndUV(int vWindowWidth, int vWindowHeight, double vDeltaTime);
        void draw(CScreenQuad* vQuad);

    private:
        int               m_SequenceRows;
        int               m_SequenceCols;
        int				  m_SequeceWidth;
        int				  m_SequeceHeight;
        int				  m_SequeceSingleTextureWidth;
        int				  m_SequeceSingleTextureHeight;
        int               m_ValidFrames;
        float             m_FramePerSecond = 24.0f;
        bool              m_IsLoop         = true;
        bool              m_IsFinished     = false;
        float             m_RotationAngle  = 0.0f;
        int               m_CurrentFrame   = 0;
        double            m_AccumFrameTime = 0.0f;
        std::string       m_TextureRootPath;
        int               m_CurrentTexture = 0;
        int               m_TextureCount;
        float			  m_ScreenRandScale = 1.0f;
        EPictureType      m_TextureType     = EPictureType::PNG;
        glm::vec2         m_ScreenUVScale   = glm::vec2(1.0f, 1.0f);
        glm::vec2         m_ScreenUVOffset  = glm::vec2(0.0f, 0.0f);
        glm::vec2		  m_WindowSize      = glm::vec2(0.0f, 0.0f);

        std::vector<CTexture2D*> m_SeqTextures;
        CShaderProgram*          m_pSequenceShaderProgram = nullptr;
    };
}