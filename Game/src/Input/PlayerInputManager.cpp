#include "Input/PlayerInputManager.h"

namespace Game
{
    void PlayerInputManager::Initialize()
    {
        m_keyMapping.BindKey(Input::MoveForward, DirectX::Keyboard::Keys::Up);
        m_keyMapping.BindKey(Input::MoveBackward, DirectX::Keyboard::Keys::Down);
        m_keyMapping.BindKey(Input::MoveLeft, DirectX::Keyboard::Keys::Left);
        m_keyMapping.BindKey(Input::MoveRight, DirectX::Keyboard::Keys::Right);

        m_keyMapping.BindKey(Input::Run, DirectX::Keyboard::Keys::LeftShift);
        m_keyMapping.BindKey(Input::Jump, DirectX::Keyboard::Keys::C);
        m_keyMapping.BindKey(Input::Dash, DirectX::Keyboard::Keys::Z);
        m_keyMapping.BindKey(Input::Attack, DirectX::Keyboard::Keys::X);

        m_keyMapping.BindKey(Input::Start, DirectX::Keyboard::Keys::Enter);
    }
    void PlayerInputManager::Update(float dt)
    {
        m_inputState.moveForward = m_keyMapping.IsKeyDown(Input::MoveForward);
        m_inputState.moveBackward = m_keyMapping.IsKeyDown(Input::MoveBackward);
        m_inputState.moveLeft = m_keyMapping.IsKeyDown(Input::MoveLeft);
        m_inputState.moveRight = m_keyMapping.IsKeyDown(Input::MoveRight);

        m_inputState.run = m_keyMapping.IsKeyDown(Input::Run);
        m_inputState.jump = m_keyMapping.IsKeyPressed(Input::Jump);
        m_inputState.dash = m_keyMapping.IsKeyPressed(Input::Dash);
        m_inputState.attack = m_keyMapping.IsKeyPressed(Input::Attack);

        m_inputState.start = m_keyMapping.IsKeyPressed(Input::Start);
    }
    void PlayerInputManager::OnAwake()
    {
        Singleton::OnAwake();
        Initialize();
    }
}