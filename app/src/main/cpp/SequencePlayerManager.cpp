#include "SequencePlayerManager.h"
#include <random>

using namespace hiveVG;

void CSequencePlayerManager::pushBack(CSequenceFramePlayer* vSequenceFramePlayer)
{
    m_SequencePlayers.push_back(vSequenceFramePlayer);
}

void CSequencePlayerManager::initSequenceState()
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        m_SequenceState.emplace_back(__initSequenceParams());
    }
}

void CSequencePlayerManager::updateFrameAndUV(int vWindowWidth, int vWindowHeight, double vDeltaTime)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        if (m_SequencePlayers[i]->getLoopState() || (!m_SequencePlayers[i]->getLoopState() && !m_SequencePlayers[i]->getFinishState()))
        {
            m_SequencePlayers[i]->updateFrameAndUV(vWindowWidth, vWindowHeight, vDeltaTime);
        }
    }
}

void CSequencePlayerManager::draw(CScreenQuad* vQuad)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        if (!m_SequenceState[i]._IsAlive) continue;
        m_SequencePlayers[i]->draw(vQuad);
    }
}

void CSequencePlayerManager::updateSequenceState(float vDeltaTime)
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
                Player->setScreenRandScale(State._UVScale);
            }
        }
        else
        {
            State._AlreadyLivingTime += vDeltaTime;
            if (State._AlreadyLivingTime > State._PlannedLivingTime)
            {
                State._IsAlive = false;
            }
            State._UVOffset += glm::vec2(State._MovingDirection, 0.0f) * State._MovingSpeed * vDeltaTime;
            Player->setScreenUVOffset(State._UVOffset);
        }
    }
}

void CSequencePlayerManager::setLoop(bool vLoop)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        m_SequencePlayers[i]->setLoopPlayback(vLoop);
    }
}

void CSequencePlayerManager::setFrameRate(int vFrameRate)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        m_SequencePlayers[i]->setFrameRate(vFrameRate);
    }
}

void CSequencePlayerManager::setRotationAngle(float vRotationAngle)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        m_SequencePlayers[i]->setRotationAngle(vRotationAngle);
    }
}

void CSequencePlayerManager::setScreenUvOffset(glm::vec2& vUVOffset)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        m_SequencePlayers[i]->setScreenUVOffset(vUVOffset);
    }
}

SSequenceState CSequencePlayerManager::__initSequenceParams()
{
    std::random_device Rd;
    std::mt19937 Gen(Rd());
    std::uniform_int_distribution<>  IntDistribution(0, 1);
    std::uniform_real_distribution<> FloatDistribution(0.0, 1.0);

    SSequenceState State;
    State._IsAlive = IntDistribution(Gen);

    FloatDistribution.param(std::uniform_real_distribution<>::param_type(30.0f, 40.0f));
    State._PlannedLivingTime = FloatDistribution(Gen);

    FloatDistribution.param(std::uniform_real_distribution<>::param_type(1.0f, 2.0f));
    State._PlannedDeadTime   = FloatDistribution(Gen);
    State._AlreadyDeadTime   = 0;
    State._AlreadyLivingTime = 0;
    State._MovingDirection   = 1;

    FloatDistribution.param(std::uniform_real_distribution<>::param_type(0.2f, 1.0f));
    State._UVScale = FloatDistribution(Gen);

    FloatDistribution.param(std::uniform_real_distribution<>::param_type(-0.5f, 0.5f));
    State._UVOffset = State._MovingDirection > 0 ? glm::vec2(-1.0f - State._UVScale, FloatDistribution(Gen)) : glm::vec2(1.0f + State._UVScale, FloatDistribution(Gen));

    float MovingDistance = 2.0f + 2 * State._UVScale; // 2.0f is from -1.0 ~ 1.0; * 2 is from left to right
    State._MovingSpeed = MovingDistance / State._PlannedLivingTime;
    return State;
}