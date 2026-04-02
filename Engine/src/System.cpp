#include "System.h"

namespace Engine
{
	HRESULT System::Initialize()
	{
		HRESULT hr = S_OK;

		// Initialize the window
		m_window = std::make_unique<Window>();
		m_window->Initialize();
		if (FAILED(hr))
		{
			return hr;
		}
		
		return S_OK;
	}

	HRESULT System::Run()
	{
		MSG msg;
		msg.message = WM_NULL;

		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				// Update and render the game here
			}
		}

		return S_OK;
	}

	LRESULT System::MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
			default:
			{
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
	}
}