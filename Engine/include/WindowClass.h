#pragma once

#include <Windows.h>
#include <string>

namespace Engine
{
	class WindowClass
	{
	public:
		WindowClass();
		~WindowClass();

		HWND GetHWND() const { return m_hWnd; }
		RECT GetWindowRect() const { return m_windowRect; }

		HRESULT Initialize();
		void Shutdown();

		static LRESULT CALLBACK StaticWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	private:
		HINSTANCE m_hInstance;
		HWND m_hWnd;
		std::wstring m_windowClassName;
		RECT m_windowRect;
	};
}