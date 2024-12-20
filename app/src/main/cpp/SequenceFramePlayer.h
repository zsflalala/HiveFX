#pragma once

#include <glm/glm.hpp>
#include <string>
#include <android/asset_manager.h>

namespace hiveVG
{
    class CTexture2D;
    class CShaderProgram;

    class CSequenceFramePlayer
    {
    public:
        CSequenceFramePlayer(const std::string& vTexturePath, int vSequenceRows, int vSequenceCols);
        ~CSequenceFramePlayer() = default;

        void setLoopPlayback(bool vLoopTag) { m_IsLoop = vLoopTag; }
        void setScreenUVScale(const glm::vec2& vScreenScale) { m_ScreenUVScale = vScreenScale; }
        void setScreenUVOffset(const glm::vec2& vScreenOffset) { m_ScreenUVOffset = vScreenOffset; }
        void setFramePerSecond(int vFramePerSecond) { m_FramePerSecond = static_cast<float>(vFramePerSecond); }
        void setValidFrames(int vValidFrames) { m_ValidFrames = vValidFrames; }
        void setRotationAngle(float vAngle) { m_RotationAngle = vAngle; }

        [[nodiscard]] bool getLoopState()   const { return m_IsLoop; }
        [[nodiscard]] bool getFinishState() const { return m_IsFinished; }

        bool initTextureAndShaderProgram(AAssetManager* vAssetManager);
        void updateFrameAndUV(int vWindowWidth, int vWindowHeight);

    private:
        double __getCurrentTime();

        int               m_SequenceRows;
        int               m_SequenceCols;
        int               m_ValidFrames;
        float             m_FramePerSecond = 60.0f;
        bool              m_IsLoop         = true;
        bool              m_IsFinished     = false;
        float             m_RotationAngle  = 0.0f;
        int               m_CurrentFrame   = 0;
        double            m_LastFrameTime;
        std::string       m_TexturePath;
        glm::vec2         m_ScreenUVScale  = glm::vec2(1.0f, 1.0f);
        glm::vec2         m_ScreenUVOffset = glm::vec2(0.0f, 0.0f);

        CTexture2D*	      m_pSequenceTexture       = nullptr;
        CShaderProgram*   m_pSequenceShaderProgram = nullptr;
    };
}
