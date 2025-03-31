#include "SplashManager.h"
#include <random>
#include "TextureBlender.h"
#include "Common.h"

using namespace hiveVG;

CSplashManager::~CSplashManager()
{
    for (int i = 0;i < m_SequencePlayers.size();i++)
    {
        delete m_SequencePlayers[i];
    }
    if (m_pTexBlender) delete m_pTexBlender;
}

void CSplashManager::pushBack(CSequenceFramePlayer* vSequenceFramePlayer)
{
    m_SequencePlayers.push_back(vSequenceFramePlayer);
}

void CSplashManager::initSequenceState()
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        m_SequenceState.emplace_back(__initSequenceParams());
        m_SequencePlayers[i]->setScreenUVOffset(m_SequenceState[i]._UVOffset);
    }
}

void CSplashManager::updateFrameAndUV(int vWindowWidth, int vWindowHeight, double vDeltaTime)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        if (m_SequencePlayers[i]->getLoopState() || (!m_SequencePlayers[i]->getLoopState() && !m_SequencePlayers[i]->getFinishState()))
        {
            m_SequencePlayers[i]->updateFrameAndUV(vWindowWidth, vWindowHeight, vDeltaTime);
        }
    }
}

void CSplashManager::draw(CScreenQuad* vQuad)
{
    auto DrawCallFunc = [this](CScreenQuad* vQuad, int Index)
    {
        this->m_SequencePlayers[Index]->draw(vQuad);
    };

    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        if (!m_SequenceState[i]._IsAlive)
            continue;
        if(m_IsBlend)
            m_pTexBlender->drawAndBlend(std::bind(DrawCallFunc, vQuad, i));
        else
            m_SequencePlayers[i]->draw(vQuad);
    }
}

void CSplashManager::updateSequenceState(float vDeltaTime)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        auto& Player = m_SequencePlayers[i];
        auto& State = m_SequenceState[i];
        if (!State._IsAlive)
        {
            State._AlreadyDeadTime += vDeltaTime;
            if (State._AlreadyDeadTime > State._PlannedDeadTime)
            {
                State = __initSequenceParams();
                State._IsAlive = true;
                Player->setScreenUVOffset(State._UVOffset);
                Player->setScreenUVScale(glm::vec2(State._UVScale, State._UVScale));
            }
        }
        else
        {
            State._AlreadyLivingTime += vDeltaTime;
            if (State._AlreadyLivingTime > State._PlannedLivingTime)
            {
                State._IsAlive = false;
            }
        }
    }
}

void CSplashManager::setLoop(bool vLoop)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        m_SequencePlayers[i]->setLoopPlayback(vLoop);
    }
}

void CSplashManager::setFrameRate(int vFrameRate)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        m_SequencePlayers[i]->setFrameRate(vFrameRate);
    }
}

void CSplashManager::setRotationAngle(float vRotationAngle)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        m_SequencePlayers[i]->setRotationAngle(vRotationAngle);
    }
}

void CSplashManager::setScreenUvOffset(glm::vec2& vUVOffset)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        m_SequencePlayers[i]->setScreenUVOffset(vUVOffset);
    }
}

SSequenceState CSplashManager::__initSequenceParams()
{
    std::random_device Rd;
    std::mt19937 Gen(Rd());
    std::uniform_int_distribution<>  IntDistribution(0, 1);
    std::uniform_real_distribution<> FloatDistribution(0.0, 1.0);

    SSequenceState State;
    State._IsAlive = IntDistribution(Gen);

    //FloatDistribution.param(std::uniform_real_distribution<>::param_type(3.0f, 4.0f));
    //State._PlannedLivingTime = FloatDistribution(Gen);
    State._PlannedLivingTime = 0.8;

    FloatDistribution.param(std::uniform_real_distribution<>::param_type(1.0f, 2.0f));
    State._PlannedDeadTime   = FloatDistribution(Gen);
    State._AlreadyDeadTime   = 0;
    State._AlreadyLivingTime = 0;
    State._MovingDirection   = 1;

    State._UVScale = 0.1;

    std::vector<glm::vec2> Positions = { glm::vec2(-0.2, 0.2), glm::vec2(-0.3, 0.2), glm::vec2(-0.4, 0.2),
                                         glm::vec2(0.2, 0.2), glm::vec2(0.8, -0.11), glm::vec2(0.9, -0.11),
                                         glm::vec2(-0.72, -0.32), glm::vec2(-0.95, -0.75), glm::vec2(-0.74, 0.19),
                                         glm::vec2(0.43, -0.425) };
    std::uniform_int_distribution<size_t> Dis(0, Positions.size() - 1);
    State._UVOffset = Positions[Dis(Gen)];

    State._MovingSpeed = 0.0;
    return State;
}

void CSplashManager::setBlendStatus(bool vStatus)
{
    if(!m_pTexBlender)
    {
        m_IsBlend = false;
        LOG_ERROR(hiveVG::TAG_KEYWORD::TEXTURE_BLENDER_TAG, "Blending cannot be started by no blender.");
        return;
    }
    m_IsBlend = vStatus;
    if (m_IsBlend)
        LOG_INFO(TAG_KEYWORD::RENDERER_TAG, "Turn on blending.");
    else
        LOG_INFO(TAG_KEYWORD::RENDERER_TAG, "Turn off blending.");
}

void CSplashManager::blit(bool vIsBlitToScreen)
{
    if(m_IsBlend)
    {
        m_pTexBlender->blit(vIsBlitToScreen);
    }
}

bool CSplashManager::initBlender(int vWidth, int vHeight)
{
    m_pTexBlender = new CTextureBlender();
    return m_pTexBlender->init(vWidth, vHeight);
}
