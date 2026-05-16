#include "Scene.h"
#include "ConstantBufferManager.h"
#include "RendererComponent.h"
#include "Log.h"
#include "Camera.h"
#include "EditorCameraController.h"
#include "Light.h"

namespace Engine
{
	Scene::Scene()
	{
		m_root = std::make_unique<GameObject>("Root");
		m_root->scene = this;

		// Create Editor camera
		auto editorCamera = CreateGameObject<GameObject>("EditorCamera");
		editorCamera->AddComponent<Camera>();
		editorCamera->AddComponent<EditorCameraController>();
		editorCamera->transform.SetLocalPosition(Vector3(0.0f, 25.0f, -60.0f));
	}

	Scene::~Scene()
	{
		m_root.reset();
	}

	void Scene::SceneAwake()
	{
		SceneEnter();
		traverseAwake(m_root.get());
	}

	void Scene::SceneStart()
	{
		traverseStart(m_root.get());
	}

	void Scene::SceneUpdate(float dt)
	{
		traverseUpdate(m_root.get(), dt);
	}

	void Scene::SceneFixedUpdate(float fdt)
	{
		traverseFixedUpdate(m_root.get(), fdt);
	}

	void Scene::traverseAwake(GameObject* node)
	{
		node->Awake();
		for (auto& child : node->GetChildren())
		{
			traverseAwake(child.get());
		}
	}

	void Scene::traverseStart(GameObject* node)
	{
		node->Start();
		for (auto& child : node->GetChildren())
		{
			traverseStart(child.get());
		}
	}

	void Scene::traverseUpdate(GameObject* node, float dt)
	{
		node->Update(dt);
		for (auto& child : node->GetChildren())
		{
			traverseUpdate(child.get(), dt);
		}
	}

	void Scene::traverseFixedUpdate(GameObject* node, float fdt)
	{
		node->FixedUpdate(fdt);
		for (auto& child : node->GetChildren())
		{
			traverseFixedUpdate(child.get(), fdt);
		}
	}

	void Scene::Render(ConstantBufferManager& cbManager)
	{
		if (!m_mainCamera)
		{
			LOG_WARNING("Main camera does not exist");
			return;
		}

		m_mainCamera->UpdateConstantBuffer(cbManager);
		UpdateLightBuffer(cbManager);
		traverseRender(m_root.get(), cbManager);
	}

	void Scene::traverseRender(GameObject* node, ConstantBufferManager& cbManager)
	{
		for (auto& component : node->GetComponents())
		{
			auto renderable = dynamic_cast<RendererComponent*>(component.get());
			if (renderable)
			{
				renderable->Render(cbManager);
			}
		}
		for (auto& child : node->GetChildren())
		{
			traverseRender(child.get(), cbManager);
		}
	}

	void Scene::RegisterCamera(Camera* camera)
	{
		m_cameras.push_back(camera);

		if (!m_mainCamera)
		{
			m_mainCamera = camera;
		}
	}

	void Scene::UnregisterCamera(Camera* camera)
	{
		m_cameras.erase(
			std::remove(m_cameras.begin(), m_cameras.end(), camera),
			m_cameras.end()
		);

		if (m_mainCamera == camera)
		{
			m_mainCamera = m_cameras.empty() ? nullptr : m_cameras[0];
		}
	}

	void Scene::RegisterLight(Light* light)
	{
		m_lights.push_back(light);
	}

	void Scene::UnregisterLight(Light* light)
	{
		m_lights.erase(
			std::remove(m_lights.begin(), m_lights.end(), light),
			m_lights.end()
		);
	}

	void Scene::UpdateLightBuffer(ConstantBufferManager& cbManager)
	{
		const uint32_t count = std::min<uint32_t>(static_cast<uint32_t>(m_lights.size()), MaxLights);
		for (size_t i = 0; i < count; ++i)
		{
			auto& lightBuffer = m_cbPerLight.lights[i]; // constant buffer
			auto* light = m_lights[i]; // scene light component
			// update world position and type
			Vector3 worldPos = light->ownerGameObject->transform.GetWorldPosition();
			lightBuffer.position = Vector4(worldPos.x, worldPos.y, worldPos.z, static_cast<float>(light->type));
			// update world direction
			Vector3 worldDir = light->ownerGameObject->transform.GetForward();
			lightBuffer.direction = Vector4(worldDir.x, worldDir.y, worldDir.z, 0.0f);
			// update color and parameters
			lightBuffer.color = light->color;
			lightBuffer.parameters = Vector4(light->innerCutOff, light->outerCutOff, light->isEnabled ? 1.0f : 0.0f, 0.0f);
		}
		m_cbPerLight.lightCount = count;
		cbManager.UpdatePerLight(m_cbPerLight);
	}
}