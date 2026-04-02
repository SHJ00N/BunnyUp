#include "Window.h"
#include "System.h"

namespace Engine
{
	Window::Window() : m_hInstance(NULL), m_hWnd(NULL)
	{
		m_windowClassName = L"BunnyEngine";
	}

	Window::~Window()
	{
	}

	HWND Window::GetHWND() const
	{
		return m_hWnd;
	}

	HRESULT Window::Initialize()
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
		RECT rc = { 0, 0, 640, 480 };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

		// Create the window
		m_hWnd = CreateWindow(
			m_windowClassName.c_str(),		// class name
			m_windowClassName.c_str(),		// window title
			WS_OVERLAPPEDWINDOW,		// style
			CW_USEDEFAULT, CW_USEDEFAULT,		// position
			(rc.right - rc.left), (rc.bottom - rc.top),		// size
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

	LRESULT CALLBACK Window::StaticWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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