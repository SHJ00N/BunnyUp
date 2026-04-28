#pragma once

#include <memory>

#include "Singleton.h"
#include "WindowClass.h"
#include "Renderer.h"
#include "ImGuiClass.h"

namespace Engine
{
	class WindowClass;
	class Renderer;
	class ImGuiClass;

	class System : public Singleton<System>
	{
	public:
		~System();

		HRESULT Initialize();
		HRESULT Run();
		LRESULT MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		void Shutdown();
	
	private:
		std::unique_ptr<WindowClass> m_pWindowClass;
		std::unique_ptr<Renderer> m_pRenderer;
		std::unique_ptr<ImGuiClass> m_pImGuiClass;

		void render();
	};
}