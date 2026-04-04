#include "Input.h"

namespace Engine
{
	Input::Input() : m_keyboard(nullptr), m_keyboardTracker(), m_keyboardState()
	{
	}

	Input::~Input()
	{
	}

	void Input::Initialize()
	{
		m_keyboard = std::make_unique<Keyboard>();
	}

	void Input::Update()
	{
		m_keyboardState = m_keyboard->GetState();
		m_keyboardTracker.Update(m_keyboardState);
	}

	bool Input::IsKeyDown(Keyboard::Keys key) const
	{
		return m_keyboardState.IsKeyDown(key);
	}

	bool Input::IsKeyPressed(Keyboard::Keys key) const
	{
		return m_keyboardTracker.IsKeyPressed(key);
	}

	bool Input::IsKeyReleased(Keyboard::Keys key) const
	{
		return m_keyboardTracker.IsKeyReleased(key);
	}
}