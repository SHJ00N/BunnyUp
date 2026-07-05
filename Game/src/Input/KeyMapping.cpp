#include "Input/KeyMapping.h"

namespace Game
{
    void KeyMapping::BindKey(Input input, DirectX::Keyboard::Keys key)
    {
        m_keyBindings[input] = key;
    }

    bool KeyMapping::IsKeyPressed(Input input) const
    {
        auto key = m_keyBindings.find(input);
        if(key == m_keyBindings.end())
        {
            return false;
        }

        return Engine::InputManager::GetInstance().IsKeyPressed(key->second);
    }

    bool KeyMapping::IsKeyReleased(Input input) const
    {
        auto key = m_keyBindings.find(input);
        if (key == m_keyBindings.end())
        {
            return false;
        }

        return Engine::InputManager::GetInstance().IsKeyReleased(key->second);
    }

    bool KeyMapping::IsKeyDown(Input input) const
    {
        auto key = m_keyBindings.find(input);
        if (key == m_keyBindings.end())
        {
            return false;
        }

        return Engine::InputManager::GetInstance().IsKeyDown(key->second);
    }
}