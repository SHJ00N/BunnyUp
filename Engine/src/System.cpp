#include "System.h"
#include "D3DManager.h"

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
		D3DManager::CreateInstance();
		hr = D3DManager::GetInstance().CreateDeviceResources();
		if (FAILED(hr))
		{
			return hr;
		}

		hr = D3DManager::GetInstance().CreateWindowResources(m_pWindowClass->GetHWND());
		if(FAILED(hr))
		{
			return hr;
		}

		// Initialize input
		m_pInput = std::make_unique<Input>();
		m_pInput->Initialize();

		// Initialize renderer
		m_pRenderer = std::make_unique<Renderer>();
		hr = m_pRenderer->Initialize();

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

				m_pRenderer->Update();

				D3DManager::GetInstance().BeginFrame(0.1f, 0.1f, 0.1f, 1.0f);

				// render the active scene here
				m_pRenderer->Render();

				D3DManager::GetInstance().EndFrame();
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