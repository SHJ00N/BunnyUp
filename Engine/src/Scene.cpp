#include "pch.h"
#include "Scene.h"
#include "ConstantBufferManager.h"
#include "RendererComponent.h"
#include "Log.h"
#include "Camera.h"
#include "EditorCameraController.h"
#include "Light.h"
#include "Frustum.h"
#include "Collider_Legacy.h"
#include "EventBus.h"
#include "PhysicsSystem.h"
#include "GameObject.h"

namespace Engine
{
	Scene::Scene()
	{
		// Initialize physics system
		m_physicsSystem = std::make_unique<PhysicsSystem>();
		m_physicsSystem->Initialize();

		// Create root GameObject
		m_root = std::make_unique<GameObject>("Root");
		m_root->scene = this;

		// Create Editor camera
		auto editorCamera = CreateGameObject<GameObject>("EditorCamera");
		editorCamera->AddComponent<Camera>();
		editorCamera->AddComponent<EditorCameraController>();
		editorCamera->transform.SetLocalPosition(Vector3(0.0f, 100.0f, -150.0f));
	}

	Scene::~Scene()
	{
	}

	void Scene::SceneAwake()
	{
		SceneEnter();
		FlushCreateObjectRequest();
	}

	void Scene::SceneStart()
	{
		traverseStart(m_root.get());
	}

	void Scene::SceneExit()
	{
		m_root.reset();
		m_physicsSystem->Shutdown();
	}

	void Scene::SceneUpdate(float dt)
	{
		traverseUpdate(m_root.get(), dt);
	}

	void Scene::SceneFixedUpdate(float fdt)
	{
		// Update physics system
		m_physicsSystem->Update(fdt);

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
		if (!node || node->IsDestroyed())
		{
			return;
		}

		node->Update(dt);
		for (auto& child : node->GetChildren())
		{
			traverseUpdate(child.get(), dt);
		}
	}

	void Scene::traverseFixedUpdate(GameObject* node, float fdt)
	{
		if(!node || node->IsDestroyed())
		{
			return;
		}

		node->FixedUpdate(fdt);
		for (auto& child : node->GetChildren())
		{
			traverseFixedUpdate(child.get(), fdt);
		}
	}

	void Scene::SceneObjectDestroy()
	{
		destroyList.clear();
		traverseDestroyed(m_root.get());	// collect destoryed object
		// destroy
		for (auto object : destroyList)
		{
			object->DestroyObject();
		}
	}

	void Scene::traverseDestroyed(GameObject* node)
	{
		if (m_root.get() != node && node->IsDestroyed())
		{
			destroyList.push_back(node);
			return;
		}

		for (auto& child : node->GetChildren())
		{
			traverseDestroyed(child.get());
		}
	}

	void Scene::RequestCreateObject(GameObject* parent, std::unique_ptr<GameObject> object)
	{
		m_requstedCreateObject.push_back({ parent, std::move(object) });
	}

	void Scene::FlushCreateObjectRequest()
	{
		for (auto& request : m_requstedCreateObject)
		{
			request.parent->AddChild(std::move(request.object));
		}

		traverseAwake(m_root.get());
		m_requstedCreateObject.clear();
	}

	void Scene::Render(ConstantBufferManager& cbManager)
	{
		if (!m_mainCamera)
		{
			// LOG_WARNING("Main camera does not exist");
			return;
		}

		m_mainCamera->UpdateConstantBuffer(cbManager);
		auto camFrustum = createFrustumFromCamera(m_cullingCamera? *m_cullingCamera : *m_mainCamera);

		UpdateLightBuffer(cbManager);

		for (auto& child : m_root->GetChildren())
		{
			traverseRender(child.get(), cbManager, camFrustum);
		}
	}

	void Scene::traverseRender(GameObject* node, ConstantBufferManager& cbManager, Frustum& camFrustum)
	{
		for (auto& component : node->GetComponents())
		{
			auto renderable = dynamic_cast<RendererComponent*>(component.get());
			if (renderable)
			{
				const AABB* bound = renderable->GetBound();

				if (!bound || bound->IsOnFrustum(camFrustum, renderable->ownerGameObject->transform))
				{
					renderable->Render(cbManager);
				}
			}
		}
		for (auto& child : node->GetChildren())
		{
			traverseRender(child.get(), cbManager, camFrustum);
		}
	}

	void Scene::RegisterCamera(Camera* camera)
	{
		m_cameras.push_back(camera);

		if (!m_mainCamera)
		{
			SetMainCamera(camera);
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
			if (!m_cameras.empty())
			{
				SetMainCamera(m_cameras[0]);
			} 
			else
			{
				m_mainCamera = nullptr;
			}
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

	void Scene::RegistObstacleCollider(Rp3dCollider* collider)
	{
		m_navObstacleColliders.push_back(collider);
	}

	void Scene::UnregistObstacleCollider(Rp3dCollider* collider)
	{
		m_navObstacleColliders.erase(
			std::remove(m_navObstacleColliders.begin(), m_navObstacleColliders.end(), collider),
			m_navObstacleColliders.end()
		);
	}
}