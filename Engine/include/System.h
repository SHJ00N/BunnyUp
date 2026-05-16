#pragma once

#include <memory>

#include "Singleton.h"
#include "WindowClass.h"
#include "ImGuiClass.h"
#include "RenderPipeline.h"

namespace Engine
{
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
		std::unique_ptr<ImGuiClass> m_pImGuiClass;
		std::unique_ptr <RenderPipeline> m_pRenderPipeline;

		void render();
	};
}