#pragma once

#include "Singleton.h"
#include "WindowClass.h"
#include "D3DClass.h"
#include "Input.h"
#include "MathHelper.h"

#include <memory>

namespace Engine
{
	class System : public Singleton<System>
	{
	public:
		HRESULT Initialize();
		HRESULT Run();
		LRESULT MessageHandler(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	
	private:
		std::unique_ptr<WindowClass> m_pWindowClass;
		std::unique_ptr<D3DClass> m_pD3DClass;
		std::unique_ptr<Input> m_pInput;
	};
}