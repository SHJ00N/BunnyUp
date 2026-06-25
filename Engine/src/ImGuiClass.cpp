#include "pch.h"
#include "ImGuiClass.h"
#include "Log.h"
#include "ConstantBufferManager.h"
#include "SceneManager.h"
#include "Camera.h"
#include "TimeClass.h"
#include "PhysicsSystem.h"

namespace Engine
{
	ImGuiClass::ImGuiClass() : m_selectedGameObject(nullptr)
	{
	}

	ImGuiClass::~ImGuiClass()
	{
	}

	void ImGuiClass::InitState()
	{
		m_selectedGameObject = nullptr;
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

		// subscribe event
		m_sceneChangeListenerID = EventBus::GetInstance().Subscribe<SceneChangedEvent>([this](const SceneChangedEvent& e) { InitState(); });
		m_objectDestroyedListenerID = EventBus::GetInstance().Subscribe< ObjectDestroyedEvent>(
			[this](const ObjectDestroyedEvent& e) 
			{ 
				if (m_selectedGameObject && e.object->IsAncestorOf(m_selectedGameObject))
				{
					m_selectedGameObject = nullptr;
				}
			}
		);
		
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

		// unsubscribe event
		EventBus::GetInstance().Unsubscribe<SceneChangedEvent>(m_sceneChangeListenerID);
		EventBus::GetInstance().Unsubscribe<ObjectDestroyedEvent>(m_objectDestroyedListenerID);
	}

	void ImGuiClass::RenderUI()
	{
		renderLogWindow();
		renderSceneHierarchyWindow();
		renderInspectorWindow();
		renderTopBar();
		frameRateCounter();
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

		auto scene = SceneManager::GetInstance().GetCurrentScene();
		if (scene)
		{
			renderGameObjectNode(scene->GetRoot());
		}
		
		ImGui::End();
	}

	void ImGuiClass::renderGameObjectNode(GameObject* gameObject)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;
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
			Vector3 rotation = m_selectedGameObject->transform.GetLocalEulerRotation();
			Vector3 scale = m_selectedGameObject->transform.GetLocalScale();

			if (ImGui::DragFloat3("Position", &position.x, 0.1f))
			{
				m_selectedGameObject->transform.SetLocalPosition(position);
			}

			if (ImGui::DragFloat3("Rotation", &rotation.x, 0.1f))
			{
				m_selectedGameObject->transform.SetLocalRotation(rotation);
			}

			if (ImGui::DragFloat3("Scale", &scale.x, 0.01f, 0.01f, 1000.0f))
			{
				m_selectedGameObject->transform.SetLocalScale(scale);
			}

			// Display components
			ImGui::Separator();
			ImGui::Text("Components:");
			for (const auto& component : m_selectedGameObject->GetComponents())
			{
				if (ImGui::TreeNodeEx(typeid(*component).name(), ImGuiTreeNodeFlags_DefaultOpen))
				{
					component->OnImGui();
					ImGui::TreePop();
				}
			}
		}
		ImGui::End();
	}

	void ImGuiClass::renderTopBar()
	{
		auto scene = SceneManager::GetInstance().GetCurrentScene();
		if (!scene) return;

		const auto& cameras = scene->GetCameras();

		ImGui::Begin("TopBar", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoScrollbar);

		// main camera
		// -------------------------------------------------------------------------------------------------------------
		Camera* mainCamera = scene->GetMainCamera();
		const char* mainPreview = mainCamera ? mainCamera->ownerGameObject->GetName().c_str() : "None";

		ImGui::SetNextItemWidth(100.0f);
		if (ImGui::BeginCombo("Main Camera", mainPreview))
		{
			for (Camera* cam : cameras)
			{
				bool selected = (cam == mainCamera);

				const char* name = nullptr;
				if (cam && cam->ownerGameObject)
				{
					name = cam->ownerGameObject->GetName().c_str();
				}

				if (ImGui::Selectable(name, selected))
				{
					scene->SetMainCamera(cam);
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		// culling camera
		// -------------------------------------------------------------------------------------------------------------
		ImGui::SetNextItemWidth(100.0f);
		Camera* cullingCamera = scene->GetCullingCamera();
		const char* cullingPreview = cullingCamera ? cullingCamera->ownerGameObject->GetName().c_str() : "None";

		if (ImGui::BeginCombo("Culling Camera", cullingPreview))
		{
			for (Camera* cam : cameras)
			{
				bool selected = (cam == cullingCamera);

				const char* name = nullptr;
				if (cam && cam->ownerGameObject)
				{
					name = cam->ownerGameObject->GetName().c_str();
				}

				if (ImGui::Selectable(name, selected))
				{
					scene->SetCullingCamera(cam);
				}

				if (selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
		ImGui::End();

		// physics debug rendering
		// -------------------------------------------------------------------------------------------------------------
		ImGui::Begin("Physics Render", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoScrollbar);
		bool isEnabled = scene->GetPhysicsSystem()->DebugRenderEnabled;
		if (ImGui::Checkbox("Physics Render", &isEnabled))
		{
			scene->GetPhysicsSystem()->SetDebugDraw(isEnabled);
		}
		ImGui::End();
	}

	void ImGuiClass::frameRateCounter()
	{
		ImGui::Begin("Frame Rate", nullptr,
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoScrollbar);
		ImGui::Text("FPS: %d", static_cast<int>(Engine::TimeClass::GetFrameRate()));
		ImGui::End();
	}
}