#include "System.h"

#include "D3DManager.h"
#include "LogManager.h"
#include "RenderStateManager.h"
#include "ResourceManager.h"
#include "SamplerStateManager.h"
#include "SceneManager.h"
#include "TimeClass.h"
namespace Engine
{
	System::~System()
	{
	}

	HRESULT System::Initialize()
	{
		HRESULT hr = S_OK;
		
		// Create singleton instances
		LogManager::CreateInstance();

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

		// initialize DirectX StateManager
		RenderStateManager::CreateInstance();
		hr = RenderStateManager::GetInstance().Initialize();
		if (FAILED(hr))
		{
			return hr;
		}

		SamplerStateManager::CreateInstance();
		hr = SamplerStateManager::GetInstance().Initialize();
		if (FAILED(hr))
		{
			return hr;
		}

		// Initialize resource manager and load default resources
		ResourceManager::CreateInstance();
		ResourceManager::GetInstance().LoadDefaultResources();

		// Initialize input
		m_pInput = std::make_unique<Input>();
		m_pInput->Initialize();

		// Initialize renderer
		m_pRenderer = std::make_unique<Renderer>();
		hr = m_pRenderer->Initialize();

		// Initialize imgui
		m_pImGuiClass = std::make_unique<ImGuiClass>();
		m_pImGuiClass->Initialize(m_pWindowClass->GetHWND(), D3DManager::GetInstance().GetDevice(), D3DManager::GetInstance().GetDeviceContext());

		// Initalize scene manager
		SceneManager::CreateInstance();
		SceneManager::GetInstance().Initialize();

		return hr;
	}

	HRESULT System::Run()
	{
		MSG msg;
		msg.message = WM_NULL;

		TimeClass::Start();

		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				// Update time
				TimeClass::Update();

				// Update Input
				m_pInput->Update();
				// press ESC to quit
				if (m_pInput->IsKeyPressed(DirectX::Keyboard::Keys::Escape))
				{
					PostQuitMessage(0);
				}

				// Update scene
				// Fixed Update
				while (TimeClass::ShouldPerformFixedUpdate())
				{
					SceneManager::GetInstance().ActiveSceneFixedUpdate(TimeClass::GetFixedDeltaTime());
					TimeClass::ConsumeFixedUpdateTime();
				}
				// Scaled delta time update
				SceneManager::GetInstance().ActiveSceneUpdate(TimeClass::GetDeltaTime());

				// Render
				render();
			}
		}

		return S_OK;
	}

	void System::render()
	{
		// Clear the back buffer and depth stencil
		D3DManager::GetInstance().BeginFrame(0.1f, 0.1f, 0.1f, 1.0f);
		m_pImGuiClass->BeginFrame();
		// Render the active scene
		SceneManager::GetInstance().ActiveSceneRender(*m_pRenderer);
		// Render the UI
		m_pImGuiClass->RenderUI();
		m_pImGuiClass->EndFrame();
		// Present the back buffer to the screen
		D3DManager::GetInstance().EndFrame();
	}

	void System::Shutdown()
	{
		// Clean up resources and singleton instances
		SceneManager::GetInstance().Clear();
		ResourceManager::GetInstance().Clear();
		LogManager::GetInstance().Clear();
		
		// Destroy singleton instances
		SceneManager::DestroyInstance();
		ResourceManager::DestroyInstance();
		LogManager::DestroyInstance();
		RenderStateManager::DestroyInstance();
		SamplerStateManager::DestroyInstance();

		// Shutdown direct3D and related resources
		m_pImGuiClass->Shutdown();
		D3DManager::GetInstance().Shutdown();
		D3DManager::DestroyInstance();
		
		// Shutdown window
		m_pWindowClass->Shutdown();
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