#include "ImGuiClass.h"
#include "Log.h"

#include "Renderer.h"
#include "System.h"

namespace Engine
{
	ImGuiClass::ImGuiClass()
	{
	}

	ImGuiClass::~ImGuiClass()
	{
	}

	void ImGuiClass::Initialize(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* context)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(device, context);
	}

	void ImGuiClass::BeginFrame()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiClass::EndFrame()
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiClass::Shutdown()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiClass::RenderUI()
	{
		renderLogWindow();
		renderTransformWindow(System::GetInstance().GetRenderer());
	}

	void ImGuiClass::renderLogWindow()
	{
		ImGui::Begin("Log");

		if (ImGui::Button("Clear"))
		{
			LogManager::GetInstance().Clear();
		}

		ImGui::Separator();

		const auto& logs = LogManager::GetInstance().GetEntries();

		for (const auto& entry : logs)
		{
			ImVec4 color;

			switch (entry.level)
			{
			case LogLevel::Info:    color = ImVec4(1, 1, 1, 1); break;
			case LogLevel::Warning: color = ImVec4(1, 1, 0, 1); break;
			case LogLevel::Error:   color = ImVec4(1, 0, 0, 1); break;
			}

			ImGui::TextColored(color, "%s", entry.message.c_str());
		}

		// scroll
		if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			ImGui::SetScrollHereY(1.0f);

		ImGui::End();
	}

	void ImGuiClass::renderTransformWindow(Renderer* renderer)
	{
		ImGui::Begin("Transform");

		ImGui::SliderFloat("Rotation Y", &renderer->Rotation, 0.0f, 360.0f);
		ImGui::SliderFloat("Scale", &renderer->Scale, 0.01f, 1.0f);

		ImGui::Checkbox("Auto Rotate", &renderer->AutoRotate);
		ImGui::SliderFloat("Speed", &renderer->RotationSpeed, 0.1f, 10.0f);

		ImGui::End();
	}
}