#include "WindowClass.h"
#include "System.h"

namespace Engine
{
	WindowClass::WindowClass() : m_hInstance(NULL), m_hWnd(NULL), m_windowRect(0)
	{
		m_windowClassName = L"BunnyEngine";
	}

	WindowClass::~WindowClass()
	{
	}

	HRESULT WindowClass::Initialize()
	{
		if (m_hInstance == NULL)
		{
			m_hInstance = (HINSTANCE)GetModuleHandle(NULL);
		}

		// Register the window class.
		WNDCLASSEXW wcex;
		wcex.cbSize = sizeof(WNDCLASSEXW);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = StaticWindowProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = m_hInstance;
		wcex.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = m_windowClassName.c_str();
		wcex.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

		if (!RegisterClassExW(&wcex))
		{
			DWORD dwError = GetLastError();
			if (dwError != ERROR_CLASS_ALREADY_EXISTS)
				return HRESULT_FROM_WIN32(dwError);
		}

		// Set window size
		int defaultWidth = 640;
		int defaultHeight = 480;
		SetRect(&m_windowRect, 0, 0, defaultWidth, defaultHeight);
		AdjustWindowRect(&m_windowRect, WS_OVERLAPPEDWINDOW, FALSE);

		// Create the window
		m_hWnd = CreateWindow(
			m_windowClassName.c_str(),		// class name
			m_windowClassName.c_str(),		// window title
			WS_OVERLAPPEDWINDOW,		// style
			CW_USEDEFAULT, CW_USEDEFAULT,		// position
			(m_windowRect.right - m_windowRect.left), (m_windowRect.bottom - m_windowRect.top),		// size
			NULL, NULL, m_hInstance, NULL		// parent, menu, instance, param
		);

		if (m_hWnd == NULL)
		{
			DWORD dwError = GetLastError();
			return HRESULT_FROM_WIN32(dwError);
		}

		ShowWindow(m_hWnd, SW_SHOW);
		UpdateWindow(m_hWnd);

		return S_OK;
	}

	LRESULT CALLBACK WindowClass::StaticWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
			case WM_CLOSE:
			{
				PostQuitMessage(0);
				return 0;
			}

			case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 0;
			}

			default:
			{
				return System::GetInstance().MessageHandler(hWnd, message, wParam, lParam);
			}
		}
	}
}