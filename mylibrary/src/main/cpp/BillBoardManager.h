#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "SequenceFramePlayer.h"

namespace hiveVG
{
    class CScreenQuad;

    struct SSequenceState
    {
        bool      _IsAlive;
        float     _PlannedLivingTime;
        float     _PlannedDeadTime;
        float     _AlreadyLivingTime;
        float     _AlreadyDeadTime;
        float     _MovingDirection;
        float     _MovingSpeed;
        glm::vec2 _UVOffset;
        float     _UVScale;
    };

    class CBillBoardManager
    {
    public:
        ~CBillBoardManager();

        void  pushBack(CSequenceFramePlayer* vSequenceFramePlayer);
        void  initSequenceState();
        void  updateFrameAndUV(int vWindowWidth, int vWindowHeight, double vDeltaTime);
        void  draw(CScreenQuad* vQuad);
        int   getSequencePlayerLength() { return m_SequencePlayers.size(); }
        float getImageAspectRatioAt(int i) { return static_cast<float>(m_SequencePlayers[i]->getSingleTextureWidth()) / static_cast<float>(m_SequencePlayers[i]->getSingleTextureHeight()); }
        void  updateSequenceState(float vDeltaTime);

        void  setLoop(bool vLoop);
        void  setFrameRate(int vFrameRate);
        void  setRotationAngle(float vRotationAngle);
        void  setScreenUvOffset(glm::vec2& vUVOffset);
        void  setImageAspectRatioAt(int i, glm::vec2& vScale) { m_SequencePlayers[i]->setScreenUVScale(vScale); }

    private:
        SSequenceState __initSequenceParams();

        double                             m_LastFrameTime = 0.0f;
        std::vector<SSequenceState>        m_SequenceState;
        std::vector<CSequenceFramePlayer*> m_SequencePlayers;
    };
}