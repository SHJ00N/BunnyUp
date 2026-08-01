#pragma once

#include <EngineSystem.h>
#include "Singleton.h"
#include "KeyMapping.h"

namespace Game
{
    struct PlayerInputState
    {
        bool moveForward = false;
        bool moveBackward = false;
        bool moveLeft = false;
        bool moveRight = false;
        bool run = false;
        bool jump = false;
        bool dash = false;
        bool attack = false;

        bool start = false;
    };

    class PlayerInputManager : public Singleton<PlayerInputManager>
    {
    public:
        void Initialize();
        void Update(float dt) override;
        const PlayerInputState& GetInputState() const { return m_inputState; }

    protected:
        void OnAwake() override;

    private:
        KeyMapping m_keyMapping;
        PlayerInputState m_inputState;
    };
}