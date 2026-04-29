#include "InputManager.h"

namespace Engine
{
	InputManager::InputManager() : m_keyboard(nullptr), m_mouse(nullptr)
	{
		m_prevMouseX = 0;
		m_prevMouseY = 0;
		m_deltaX = 0;
		m_deltaY = 0;
	}

	InputManager::~InputManager()
	{
	}

	void InputManager::Initialize(HWND hwnd)
	{
		m_keyboard = std::make_unique<Keyboard>();

		m_mouse = std::make_unique<Mouse>();
		m_mouse->SetWindow(hwnd);
		m_mouse->SetMode(Mouse::MODE_ABSOLUTE);
	}

	void InputManager::Update()
	{
		// Update keyboard
		m_keyboardState = m_keyboard->GetState();
		m_keyboardTracker.Update(m_keyboardState);

		// Update mouse
		m_mouseState = m_mouse->GetState();
		m_mouseTracker.Update(m_mouseState);

		// movement
		if (m_mouseState.positionMode == Mouse::MODE_RELATIVE)
		{
			m_deltaX = m_mouseState.x;
			m_deltaY = m_mouseState.y;
		}
		else
		{
			m_deltaX = m_mouseState.x - m_prevMouseX;
			m_deltaY = m_mouseState.y - m_prevMouseY;

			m_prevMouseX = m_mouseState.x;
			m_prevMouseY = m_mouseState.y;
		}

		// wheel scroll
		m_deltaScroll = m_mouseState.scrollWheelValue - m_prevScroll;
		m_prevScroll = m_mouseState.scrollWheelValue;
	}

#pragma region keyboad
	bool InputManager::IsKeyDown(Keyboard::Keys key) const
	{
		return m_keyboardState.IsKeyDown(key);
	}

	bool InputManager::IsKeyPressed(Keyboard::Keys key) const
	{
		return m_keyboardTracker.IsKeyPressed(key);
	}

	bool InputManager::IsKeyReleased(Keyboard::Keys key) const
	{
		return m_keyboardTracker.IsKeyReleased(key);
	}

#pragma endregion

#pragma region mouse
	bool InputManager::IsMouseDown(MouseButton button) const
	{
		switch (button)
		{
			case MouseButton::Left: 
			{
				return m_mouseState.leftButton;
			}
			case MouseButton::Right:
			{
				return m_mouseState.rightButton;
			}
			case MouseButton::Wheel:
			{
				return m_mouseState.middleButton;
			}
		}
		return false;
	}

	bool InputManager::IsMousePressed(MouseButton button) const
	{
		switch (button)
		{
			case MouseButton::Left:
			{
				return m_mouseTracker.leftButton == Mouse::ButtonStateTracker::PRESSED;
			}
			case MouseButton::Right:
			{
				return m_mouseTracker.rightButton == Mouse::ButtonStateTracker::PRESSED;
			}
			case MouseButton::Wheel:
			{
				return m_mouseTracker.middleButton == Mouse::ButtonStateTracker::PRESSED;
			}
		}
		return false;
	}

	bool InputManager::IsMouseReleased(MouseButton button) const
	{
		switch (button)
		{
			case MouseButton::Left:
			{
				return m_mouseTracker.leftButton == Mouse::ButtonStateTracker::RELEASED;
			}
			case MouseButton::Right:
			{
				return m_mouseTracker.rightButton == Mouse::ButtonStateTracker::RELEASED;
			}
			case MouseButton::Wheel:
			{
				return m_mouseTracker.middleButton == Mouse::ButtonStateTracker::RELEASED;
			}
		}
		return false;
	}

	void InputManager::ResetMouseState()
	{
		m_prevMouseX = m_mouseState.x;
		m_prevMouseY = m_mouseState.y;
		m_deltaX = 0;
		m_deltaY = 0;
	}

#pragma endregion

	void InputManager::OnFPSMode() 
	{ 
		m_isFPSMode = true; 
		m_mouse->SetMode(Mouse::MODE_RELATIVE); 
		ResetMouseState();
	}
	void InputManager::OffFPSMode() 
	{ 
		m_isFPSMode = false; 
		m_mouse->SetMode(Mouse::MODE_ABSOLUTE); 
		ResetMouseState();
	}

	void InputManager::OnEditorMode()
	{
		m_isEditorMode = true;
		OffFPSMode();
	}

	void InputManager::OffEditorMode()
	{
		m_isEditorMode = false;
		OffFPSMode();
	}
}