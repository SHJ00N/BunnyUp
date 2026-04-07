#include "System.h"

namespace Engine
{
	HRESULT System::Initialize()
	{
		HRESULT hr = S_OK;
		// Initialize the window
		m_pWindowClass = std::make_unique<WindowClass>();
		hr = m_pWindowClass->Initialize();
		if (FAILED(hr))
		{
			return hr;
		}

		// Initialize Direct3D
		m_pD3DClass = std::make_unique<D3DClass>();
		hr = m_pD3DClass->CreateDeviceResources();
		if (FAILED(hr))
		{
			return hr;
		}

		hr = m_pD3DClass->CreateWindowResources(m_pWindowClass->GetHWND());
		if(FAILED(hr))
		{
			return hr;
		}

		// Initialize input
		m_pInput = std::make_unique<Input>();
		m_pInput->Initialize();

		return hr;
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

				// press ESC to quit
				if (m_pInput->IsKeyPressed(DirectX::Keyboard::Keys::Escape))
				{
					PostQuitMessage(0);
				}

				m_pD3DClass->BeginFrame(0.1f, 0.1f, 0.1f, 1.0f);
				m_pD3DClass->EndFrame();
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