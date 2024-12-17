#include "SequencePlayerManager.h"
#include <random>

using namespace hiveVG;

void CSequencePlayerManager::pushBack(CSequenceFramePlayer& vSequenceFramePlayer)
{
    m_SequencePlayers.push_back(vSequenceFramePlayer);
}

void CSequencePlayerManager::initSequenceState()
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        m_SequenceState.emplace_back(__getAnInitState());
    }
}

void CSequencePlayerManager::updateFrameAndUV(int vWindowWidth, int vWindowHeight, double vDt)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        if (m_SequencePlayers[i].getLoopState() || (!m_SequencePlayers[i].getLoopState() && !m_SequencePlayers[i].getFinishState()))
        {
            m_SequencePlayers[i].updateFrameAndUV(vWindowWidth, vWindowHeight, vDt);
        }
    }
}

void CSequencePlayerManager::draw(CScreenQuad* vQuad)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        m_SequencePlayers[i].draw(vQuad);
    }
}

void CSequencePlayerManager::updateSequenceState(double vDt)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        auto& Player = m_SequencePlayers[i];
        auto& State = m_SequenceState[i];
        if (!State._IsAlive)
        {
            State._AlreadyDeadTime += vDt;
            if (State._AlreadyDeadTime > State._PlannedDeadTime)
            {
                State = __getAnInitState();
                State._IsAlive = true;
                Player.setScreenUVOffset(State._UVOffset);
                Player.setScreenRandScale(State._UVScale);
            }
        }
        else
        {
            State._AlreadyLivingTime += vDt;
            State._UVOffset += glm::dvec2(State._MovingDirection, 0.0f) * (double)State._MovingSpeed * vDt;
            if (State._AlreadyLivingTime > State._PlannedLivingTime)
            {
                State._IsAlive = false;
            }
            Player.setScreenUVOffset(State._UVOffset);
        }
    }
}

void CSequencePlayerManager::setLoop(bool vLoop)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        m_SequencePlayers[i].setLoopPlayback(vLoop);
    }
}

void CSequencePlayerManager::setPlayingSpeed(int vPlayingSpeed)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        m_SequencePlayers[i].setFramePerSecond(vPlayingSpeed);
    }
}

void CSequencePlayerManager::setRotationAngle(float vRotationAngle)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        m_SequencePlayers[i].setRotationAngle(vRotationAngle);
    }
}

void CSequencePlayerManager::setScreenUvOffset(glm::vec2 vUVOffset)
{
    for (int i = 0; i < m_SequencePlayers.size(); i++)
    {
        m_SequencePlayers[i].setScreenUVOffset(vUVOffset);
    }
}

SSequenceState CSequencePlayerManager::__getAnInitState()
{
    std::random_device Rd;
    std::mt19937 Gen(Rd());
    std::uniform_int_distribution<>  IntDistribution(0, 1);
    std::uniform_real_distribution<> FloatDistribution(0.0, 1.0);

    SSequenceState State;
    State._IsAlive = IntDistribution(Gen);

    FloatDistribution.param(std::uniform_real_distribution<>::param_type(15.0f, 20.0f));
    State._PlannedLivingTime = FloatDistribution(Gen);

    State._PlannedDeadTime   = 0;
    State._AlreadyDeadTime   = 0;
    State._AlreadyLivingTime = 0;
    State._MovingDirection   = 1;

    FloatDistribution.param(std::uniform_real_distribution<>::param_type(0.2f, 1.0f));
    State._UVScale = FloatDistribution(Gen);

    FloatDistribution.param(std::uniform_real_distribution<>::param_type(-0.5f, 0.5f));
    State._UVOffset = State._MovingDirection > 0 ? glm::vec2(-1.0f - State._UVScale, FloatDistribution(Gen)) : glm::vec2(1.0f + State._UVScale, FloatDistribution(Gen));
    State._MovingSpeed = (2.0f + 2 * State._UVScale) / State._PlannedLivingTime;  // TODO : how this 2.0 generate
    return State;
}