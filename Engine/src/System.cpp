#include "System.h"

namespace Engine
{
	HRESULT System::Initialize()
	{
		// Initialize the window
		m_pWindowClass = std::make_unique<WindowClass>();
		if (FAILED(m_pWindowClass->Initialize()))
		{
			DWORD dwError = GetLastError();
			return HRESULT_FROM_WIN32(dwError);
		}

		// Initialize Direct3D
		m_pD3DClass = std::make_unique<D3DClass>();
		if(FAILED(m_pD3DClass->CreateDeviceResources()) && 
			FAILED(m_pD3DClass->CreateWindowResources(m_pWindowClass->GetHWND())))
		{
			DWORD dwError = GetLastError();
			return HRESULT_FROM_WIN32(dwError);
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