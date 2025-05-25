#pragma once

#include <string>
#include "Common.h"

namespace hiveVG
{
    class CScreenQuad;
    class CNightSceneSequencePlayer;

    class CNightSceneRenderer
    {
    public:
        CNightSceneRenderer();
        ~CNightSceneRenderer();

        void render();
        void setRenderChannel(ERenderChannel vChannel);

    private:
        void __initAlgorithm();

        double       m_LastFrameTime = 0.0f;
        double       m_CurrentTime   = 0.0f;

        CScreenQuad*          m_pScreenQuad = nullptr;
        CNightSceneSequencePlayer* m_pNightSceneSequencePlayer = nullptr;
    };
}
