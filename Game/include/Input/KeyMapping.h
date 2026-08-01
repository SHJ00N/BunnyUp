#pragma once

#include <EngineSystem.h>

namespace Game
{
    enum class Input
    {
        MoveForward,
        MoveBackward,
        MoveLeft,
        MoveRight,

        Run,
        Jump,
        Dash,
        Attack,

        Start
    };

    class KeyMapping
    {
    public:
        void BindKey(Input input, DirectX::Keyboard::Keys key);

        bool IsKeyPressed(Input input) const;
        bool IsKeyReleased(Input input) const;
        bool IsKeyDown(Input input) const;

    private:
        std::unordered_map<Input, DirectX::Keyboard::Keys> m_keyBindings;
    };
}
