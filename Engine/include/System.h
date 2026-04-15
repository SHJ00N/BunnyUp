#pragma once

#include "Singleton.h"
#include "WindowClass.h"
#include "Input.h"
#include "MathHelper.h"
#include "Renderer.h"
#include "ImGuiClass.h"

#include <memory>

namespace Engine
{
	class System : public Singleton<System>
	{
	public:
		HRESULT Initialize();
		HRESULT Run();
		LRESULT MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		Renderer* GetRenderer() const { return m_pRenderer.get(); }

		void Shutdown();
	
	private:
		std::unique_ptr<WindowClass> m_pWindowClass;
		std::unique_ptr<Input> m_pInput;
		std::unique_ptr<Renderer> m_pRenderer;
		std::unique_ptr<ImGuiClass> m_pImGuiClass;
	};
}