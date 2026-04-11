#pragma once

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <windows.h>
#include <d3d11.h>

namespace Engine
{
	class ImGuiClass
	{
	public:
		ImGuiClass();
		~ImGuiClass();

		void Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context);
		void BeginFrame();
		void EndFrame();
		void RenderUI();

		void Shutdown();
	};
}