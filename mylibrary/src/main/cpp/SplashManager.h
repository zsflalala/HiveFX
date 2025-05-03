#include <vector>
#include "SequenceFramePlayer.h"
#include "Common.h"

namespace hiveVG
{
    class CScreenQuad;
    class CTextureBlender;
    class CSequenceFramePlayer;
    struct SSequenceState;

    class CSplashManager
    {
    public:
        ~CSplashManager();

        void  pushBack(CSequenceFramePlayer* vSequenceFramePlayer);
        void  initSequenceState(const std::string& vImagePath, const float& vScale);
        bool  initBlender(int vWidth, int vHeight);
        void  updateFrameAndUV(double vDeltaTime);
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
        void  setIsQuantization(bool vIsQuantization){m_IsQuantization=vIsQuantization;}
    private:
        SSequenceState __initSequenceParams();
        void __calculatePosition(const std::string& vImagePath, const float& vScale);

        float                              m_SplashScale   = 0.0f;
        double                             m_LastFrameTime = 0.0f;
        bool                               m_IsBlend       = false;
        bool                               m_IsQuantization= false;
        CTextureBlender*                   m_pTexBlender   = nullptr;
        std::vector<SSequenceState>        m_SequenceState;
        std::vector<CSequenceFramePlayer*> m_SequencePlayers;
        std::vector<glm::vec2>             m_SplashPositions;
    };
}