#pragma once

#include <Keyboard.h>
#include <memory>

namespace Engine
{
	using namespace DirectX;
	
	class Input
	{
	public:
		Input();
		~Input();

		// initialize the input system
		void Initialize();	
		// update the input system
		void Update();

		bool IsKeyDown(Keyboard::Keys key) const;		// current key state
		bool IsKeyPressed(Keyboard::Keys key) const;		// key down event
		bool IsKeyReleased(Keyboard::Keys key) const;	// key up event

	private:
		std::unique_ptr<Keyboard> m_keyboard;
		Keyboard::KeyboardStateTracker m_keyboardTracker;
		Keyboard::State m_keyboardState;
	};
}