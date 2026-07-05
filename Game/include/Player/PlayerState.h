#pragma once

#include <EngineSystem.h>

namespace Game
{
    struct PlayerInputState;
    class PlayerController;
    class PlayerState
    {
    public:
        virtual ~PlayerState() = default;
        virtual bool HandleInput(PlayerController& playerController, const PlayerInputState& inputState) = 0;

        virtual void Enter(PlayerController& playerController) { };
        virtual void Update(float deltaTime, PlayerController& playerController, const PlayerInputState& inputState) { };
        virtual void Exit(PlayerController& playerController) { };
    };
}