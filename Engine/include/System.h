#pragma once

#include "Window.h"
#include "Singleton.h"

#include <memory.h>

namespace Engine
{
	class System : public Singleton<System>
	{
	public:
		HRESULT Initialize();
		HRESULT Run();
		LRESULT MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	
	private:
		std::unique_ptr<Window> m_window;
	};
}