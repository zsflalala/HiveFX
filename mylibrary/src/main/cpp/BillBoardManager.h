#pragma once

#include <vector>
#include "SequenceFramePlayer.h"
#include "Common.h"

namespace hiveVG
{
    class CScreenQuad;
    class CTextureBlender;
    class CSequenceFramePlayer;
    struct SSequenceState;

    class CBillBoardManager
    {
    public:
        ~CBillBoardManager();

        void  pushBack(CSequenceFramePlayer* vSequenceFramePlayer);
        void  initSequenceState();
        bool  initBlender(int vWidth, int vHeight);
        void  updateFrameAndUV(int vWindowWidth, int vWindowHeight, double vDeltaTime);
        void  draw(CScreenQuad* vQuad);
        int   getSequencePlayerLength() { return m_SequencePlayers.size(); }
        float getImageAspectRatioAt(int i) { return static_cast<float>(m_SequencePlayers[i]->getSingleTextureWidth()) / static_cast<float>(m_SequencePlayers[i]->getSingleTextureHeight()); }
        void  updateSequenceState(float vDeltaTime);
        void  setBlendStatus(bool vStatus);
        void  blit(bool vIsBlitToScreen = true);

        void  setLoop(bool vLoop);
        void  setFrameRate(int vFrameRate);
        void  setRotationAngle(float vRotationAngle);
        void  setScreenUvOffset(glm::vec2& vUVOffset);
        void  setImageAspectRatioAt(int i, glm::vec2& vScale) { m_SequencePlayers[i]->setScreenUVScale(vScale); }
        void  setBlender(CTextureBlender* vTexBlender) { m_pTexBlender = vTexBlender; }

    private:
        SSequenceState __initSequenceParams();

        double                             m_LastFrameTime = 0.0f;
        bool                               m_IsBlend       = false;
        CTextureBlender*                   m_pTexBlender = nullptr;
        std::vector<SSequenceState>        m_SequenceState;
        std::vector<CSequenceFramePlayer*> m_SequencePlayers;
    };
}