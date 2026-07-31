#include "pch.h"
#include "System.h"

#include "D3DManager.h"
#include "LogManager.h"
#include "RenderStateManager.h"
#include "ResourceManager.h"
#include "SamplerStateManager.h"
#include "SceneManager.h"
#include "SceneFactory.h"
#include "TimeClass.h"
#include "EventBus.h"
#include "InputManager.h"
#include "ConstantBufferManager.h"
#include "DebugRenderer.h"
#include "RandomClass.h"

extern void LoadGameResources();
extern void RegistractionGameScenes();

namespace Engine
{
	System::~System()
	{
	}

	HRESULT System::Initialize()
	{
		HRESULT hr = S_OK;

		Random::Init();

		// Create system instances
		LogManager::CreateInstance();
		EventBus::CreateInstance();

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

		// Initialize constant buffer manager
		ConstantBufferManager::CreateInstance();
		hr = ConstantBufferManager::GetInstance().Initialize();
		if (FAILED(hr))
		{
			return hr;
		}

		// Initialize DebugRenderer instance
		DebugRenderer::CreateInstance();
		hr = DebugRenderer::GetInstance().Initialize(&D3DManager::GetInstance(), &ConstantBufferManager::GetInstance());
		if (FAILED(hr))
		{
			return hr;
		}

		// Initialize resource manager and load default resources
		ResourceManager::CreateInstance();
		ResourceManager::GetInstance().LoadDefaultResources();
		LoadGameResources();

		// Initialize RenderPipline instance
		m_pRenderPipeline = std::make_unique<RenderPipeline>();
		m_pRenderPipeline->Initialize(&ConstantBufferManager::GetInstance());

		// Initialize input
		InputManager::CreateInstance();
		InputManager::GetInstance().Initialize(m_pWindowClass->GetHWND());

		// Initialize imgui
		m_pImGuiClass = std::make_unique<ImGuiClass>();
		m_pImGuiClass->Initialize(m_pWindowClass->GetHWND(), D3DManager::GetInstance().GetDevice(), D3DManager::GetInstance().GetDeviceContext());

		// Initalize scene management
		SceneFactory::CreateInstance();	// Create SceneFactory instance
		RegistractionGameScenes();

		SceneManager::CreateInstance();
		SceneManager::GetInstance().Initialize();

		return hr;
	}

	HRESULT System::Run()
	{
		MSG msg;
		msg.message = WM_NULL;

		TimeClass::Start();

		while (true)
		{
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					return S_OK;
				
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			
			// Update time
			TimeClass::Update();
			// Update Input
			InputManager::GetInstance().Update();
			// press ESC to quit
			if (InputManager::GetInstance().IsKeyPressed(DirectX::Keyboard::Keys::Escape))
			{
				PostQuitMessage(0);
			}
			if (InputManager::GetInstance().IsKeyPressed(DirectX::Keyboard::Keys::Enter))
			{
				static int sceneState = 0;
				sceneState++;
				SceneManager::GetInstance().LoadScene(sceneState % SceneFactory::GetInstance().GetSceneCount());
			}
			if (InputManager::GetInstance().IsKeyPressed(DirectX::Keyboard::Keys::F3))
			{
				if (InputManager::GetInstance().IsEditorMode())
				{
					InputManager::GetInstance().OffEditorMode();
				} 
				else
				{
					InputManager::GetInstance().OnEditorMode();
				}
			}

			// clear debug drawing list
			DebugRenderer::GetInstance().Clear();

			// Update scene
			// Fixed Update
			while (TimeClass::ShouldPerformFixedUpdate())
			{
				SceneManager::GetInstance().CurrentSceneFixedUpdate(TimeClass::GetFixedDeltaTime());
				TimeClass::ConsumeFixedUpdateTime();
			}
			// Scaled delta time update
			SceneManager::GetInstance().CurrentSceneUpdate(TimeClass::GetDeltaTime());
			// delete destroyed object
			SceneManager::GetInstance().CurrentSceneObjectDestroy();
			// Render
			auto rect = m_pWindowClass->GetWindowRect();
			ConstantBufferPerScreen data = { static_cast<unsigned int>((rect.right - rect.left)), static_cast<unsigned int>((rect.bottom - rect.top)) };
			ConstantBufferManager::GetInstance().UpdatePerScreen(data);
			render();
		}

		return S_OK;
	}

	void System::render()
	{
		if (InputManager::GetInstance().IsEditorMode())
		{
			m_pImGuiClass->BeginFrame();
		}
		// Build render graph and execute render passes based on render graph
		m_pRenderPipeline->Render(SceneManager::GetInstance().GetCurrentScene());
		// Render the UI
		if (InputManager::GetInstance().IsEditorMode())
		{
			DebugRenderer::GetInstance().Render(SceneManager::GetInstance().GetCurrentScene());
			m_pImGuiClass->RenderUI();
			m_pImGuiClass->EndFrame();
		}
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
		SceneFactory::DestroyInstance();
		SceneManager::DestroyInstance();
		ResourceManager::DestroyInstance();
		LogManager::DestroyInstance();
		RenderStateManager::DestroyInstance();
		SamplerStateManager::DestroyInstance();
		InputManager::DestroyInstance();
		DebugRenderer::DestroyInstance();
		ConstantBufferManager::DestroyInstance();

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
			case WM_SYSKEYDOWN:
			{
				if ((lParam & 0x40000000) == 0)
					DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
				break;
			}
			case WM_KEYUP:
			case WM_SYSKEYUP:
			{
				DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
				break;
			}
			case WM_INPUT:
			case WM_MOUSEWHEEL:
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			case WM_XBUTTONDOWN:
			case WM_XBUTTONUP:
			{
				Mouse::ProcessMessage(message, wParam, lParam);
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