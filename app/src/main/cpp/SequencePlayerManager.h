#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "SequenceFramePlayer.h"

namespace hiveVG
{
    class CScreenQuad;

    struct SSequenceState
    {
        bool  _IsAlive;

        float _PlannedLivingTime;
        float _PlannedDeadTime;
        float _AlreadyLivingTime;
        float _AlreadyDeadTime;
        float _MovingDirection;
        float _MovingSpeed;
        glm::vec2 _UVOffset;
        float     _UVScale;
    };

    class CSequencePlayerManager
    {
    public:
        void pushBack(CSequenceFramePlayer& vSequenceFramePlayer);
        void initSequenceState();
        void updateFrameAndUV(int vWindowWidth, int vWindowHeight, double vDt);
        void draw(CScreenQuad* vQuad);
        int   getSequencePlayerLength() { return m_SequencePlayers.size(); }
        float getImageAspectRatioAt(int i) { return static_cast<float>(m_SequencePlayers[i].getSingleWidth()) / static_cast<float>(m_SequencePlayers[i].getSingleHeight()); }
        void  updateSequenceState(double vDt);

        void setLoop(bool vLoop);
        void setPlayingSpeed(int vPlayingSpeed);
        void setRotationAngle(float vRotationAngle);
        void setScreenUvOffset(glm::vec2 vUVOffset);
        void setImageAspectRatioAt(int i, glm::vec2 vScale) { m_SequencePlayers[i].setScreenUVScale(vScale); }

    private:
        SSequenceState __getAnInitState();

        std::vector<CSequenceFramePlayer> m_SequencePlayers;
        std::vector<SSequenceState>       m_SequenceState;
        double m_LastFrameTime = 0.0f;
    };
}