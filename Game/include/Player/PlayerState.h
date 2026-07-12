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
        
        void Update(float deltaTime, PlayerController& playerController, const PlayerInputState& inputState) 
        {
            m_elapsedTime += deltaTime;
            OnUpdate(deltaTime, playerController, inputState);
        };
        
        virtual bool HandleInput(PlayerController& playerController, const PlayerInputState& inputState) = 0;
        virtual void Enter(PlayerController& playerController) { }
        virtual void Exit(PlayerController& playerController) { }

    protected:
        virtual void OnUpdate(float deltaTime, PlayerController& playerController, const PlayerInputState& inputState) { }
        
        float m_elapsedTime = 0.0f;
        void ResetTimer() { m_elapsedTime = 0.0f; }
        bool Elapsed(float time) const { return m_elapsedTime >= time; }
    };
}