#include "ImGuiClass.h"
#include "Log.h"

#include "Renderer.h"
#include "SceneManager.h"

namespace Engine
{
	ImGuiClass::ImGuiClass() : m_selectedGameObject(nullptr)
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
		renderSceneHierarchyWindow();
		renderInspectorWindow();
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

	void ImGuiClass::renderSceneHierarchyWindow()
	{
		ImGui::Begin("Hierarchy");

		auto scene = SceneManager::GetInstance().GetActiveScene();
		if (scene)
		{
			renderGameObjectNode(scene->GetRoot());
		}
		
		ImGui::End();
	}

	void ImGuiClass::renderGameObjectNode(GameObject* gameObject)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		if (gameObject->GetChildren().empty())
			flags |= ImGuiTreeNodeFlags_Leaf;

		bool nodeOpen = ImGui::TreeNodeEx(gameObject->GetName().c_str(), flags);
		if (ImGui::IsItemClicked())
		{
			m_selectedGameObject = gameObject; // Set selected game object
		}

		if (nodeOpen)
		{
			for (const auto& child : gameObject->GetChildren())
			{
				renderGameObjectNode(child.get());
			}
			ImGui::TreePop();
		}
	}

	void ImGuiClass::renderInspectorWindow()
	{
		ImGui::Begin("Inspector");

		if (m_selectedGameObject)
		{
			ImGui::Text("Name: %s", m_selectedGameObject->GetName().c_str());

			// Transform information
			ImGui::Separator();
			ImGui::Text("Transform");

			// Set up local variables to hold transform data for ImGui editing
			Vector3 position = m_selectedGameObject->transform.GetLocalPosition();
			Vector3 rotation = m_selectedGameObject->transform.GetLocalRotation();
			Vector3 scale = m_selectedGameObject->transform.GetLocalScale();

			ImGui::DragFloat3("Position", &position.x, 0.1f);
			ImGui::DragFloat3("Rotation", &rotation.x, 0.1f);
			ImGui::DragFloat3("Scale", &scale.x, 0.01f);

			m_selectedGameObject->transform.SetLocalPosition(position);
			m_selectedGameObject->transform.SetLocalRotation(rotation);
			m_selectedGameObject->transform.SetLocalScale(scale);
			
			// Display components
			ImGui::Separator();
			ImGui::Text("Components:");
			for (const auto& component : m_selectedGameObject->GetComponents())
			{
				if (ImGui::TreeNode(typeid(*component).name()))
				{
					component->OnImGui();
					ImGui::TreePop();
				}
			}
		}
		ImGui::End();
	}
}