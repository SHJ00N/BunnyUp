#pragma once

#include "Player/OnGroundState.h"

namespace Game
{
    class DashState : public OnGroundState
    {
    public:
        bool HandleInput(PlayerController& playerController, const PlayerInputState& inputState) override;
        void Enter(PlayerController& playerController) override;
        void Exit(PlayerController& playerController) override;
    
    protected:
        void OnUpdate(float deltaTime, PlayerController& playerController, const PlayerInputState& inputState) override;

    private:
        Engine::Vector3 m_direction;
    };
}