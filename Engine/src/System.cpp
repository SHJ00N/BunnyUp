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

		// Initialize input
		m_pInput = std::make_unique<Input>();
		m_pInput->Initialize();
		
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
				m_pInput->Update();

				// K 키가 눌렸을 때
				if (m_pInput->IsKeyPressed(DirectX::Keyboard::Keys::K))
				{
					PostQuitMessage(0); // 메시지 루프 종료
					return 0;
				}
			}
		}

		return S_OK;
	}

	LRESULT System::MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
			case WM_KEYDOWN:
			case WM_KEYUP:
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			{
				DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
				break;
			}
			default:
			{
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}

		return 0;
	}
}