#pragma once

#include <Windows.h>
#include <Keyboard.h>
#include <Mouse.h>
#include <memory>

#include "Singleton.h"

namespace Engine
{
	using namespace DirectX;
	
	enum class MouseButton
	{
		Left,
		Right,
		Wheel
	};

	class InputManager : public Singleton<InputManager>
	{
	public:
		InputManager();
		~InputManager();

		// initialize the input system
		void Initialize(HWND hwnd);	
		// update the input system
		void Update();

		// keyboard
		bool IsKeyDown(Keyboard::Keys key) const;		// current key state
		bool IsKeyPressed(Keyboard::Keys key) const;		// key down event
		bool IsKeyReleased(Keyboard::Keys key) const;	// key up event

		// mouse
		bool IsMouseDown(MouseButton button) const;
		bool IsMousePressed(MouseButton button) const;
		bool IsMouseReleased(MouseButton button) const;

		int GetMouseX() const { return m_mouseState.x; }
		int GetMouseY() const { return m_mouseState.y; }

		int GetMouseDeltaX() const { return m_deltaX; }
		int GetMouseDeltaY() const { return m_deltaY; }

	private:
		// keyboard
		std::unique_ptr<Keyboard> m_keyboard;
		Keyboard::KeyboardStateTracker m_keyboardTracker;
		Keyboard::State m_keyboardState;

		// mouse
		std::unique_ptr<Mouse> m_mouse;
		Mouse::ButtonStateTracker m_mouseTracker;
		Mouse::State m_mouseState;

		int m_prevMouseX;
		int m_prevMouseY;
		int m_deltaX;
		int m_deltaY;
	};
}