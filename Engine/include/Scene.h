#pragma once

#include "ConstantBuffer.h"
#include "GameObject.h"
#include "RenderQueue.h"

#include <memory>

namespace Engine
{
	class Camera;
	class Light;
	class Collider;
	class EnvironmentMap;
	class PhysicsSystem;
	class ConstantBufferManager;
	class TextComponent;

	struct ObjectCreateRequest
	{
		GameObject* parent;
		std::unique_ptr<GameObject> object;
	};

	class Scene
	{
	public:
		Scene();
		~Scene();

		void SceneAwake();
		void SceneStart();
		void SceneUpdate(float dt);
		void SceneFixedUpdate(float fdt);
		void SceneObjectDestroy();
		void SceneExit();


		void RequestCreateObject(GameObject* parent, std::unique_ptr<GameObject> object);
		void FlushCreateObjectRequest();
		
		void CollectRenderCall(ConstantBufferManager& cbManager);
		void RenderOpaque(ConstantBufferManager& cbManager);
		void RenderTransparent(ConstantBufferManager& cbManager);
			
		GameObject* GetRoot() { return m_root.get(); }
		// Read-only version for external access
		const GameObject* GetRoot() const { return m_root.get(); }
		void SetSceneRootName(const std::string& name) { m_root->SetName(name); }

		// Add GameObject to the root of the scene
		template<typename T, typename... Args>
		T* CreateGameObject(Args&&... args)
		{
			// Ensure T is derived from SceneNode
			static_assert(std::is_base_of<GameObject, T>(), "T must be a GameObject");

			// Create a new GameObject and add it to the root
			auto object = std::make_unique<T>(std::forward<Args>(args)...); 
			T* ptr = object.get(); 
			
			// Set parent-child relationship
			m_requstedCreateObject.emplace_back(m_root.get(), std::move(object));
			return ptr;
		}

		template<typename T, typename... Args>
		T* CreateChildGameObject(GameObject* parent, Args&&... args)
		{
			// Ensure T is derived from SceneNode
			static_assert(std::is_base_of<GameObject, T>(), "T must be a GameObject");

			// Create a new GameObject and add it to the root
			auto object = std::make_unique<T>(std::forward<Args>(args)...);
			T* ptr = object.get();

			// Set parent-child relationship
			m_requstedCreateObject.emplace_back(parent, std::move(object));
			return ptr;
		}

		// camera management
		void RegisterCamera(Camera* camera);
		void UnregisterCamera(Camera* camera);
		const std::vector<Camera*>& GetCameras() const { return m_cameras; }
		Camera* GetMainCamera() const { return m_mainCamera; }
		void SetMainCamera(Camera* camera) 
		{ 
			m_mainCamera = camera;  
			if (!m_cullingCamera) m_cullingCamera = camera;
		}
		Camera* GetCullingCamera() const { return m_cullingCamera; }
		void SetCullingCamera(Camera* camera) { m_cullingCamera = camera; }	// for debugging frustum culling

		// light management
		void RegisterLight(Light* light);
		void UnregisterLight(Light* light);
		const std::vector<Light*>& GetLights() const { return m_lights; }
		void UpdateLightBuffer(ConstantBufferManager& cbManager);

		// collider management
		void RegistObstacleCollider(Rp3dCollider* collider);
		void UnregistObstacleCollider(Rp3dCollider* collider);
		const std::vector<Rp3dCollider*>& GetObstacleColliders() const { return m_navObstacleColliders; }

		// text management
		void RegistText(TextComponent* text);
		void UnregistText(TextComponent* text);
		const std::vector<TextComponent*>& GetTexts() const { return m_texts; }

		// environment map management
		void SetEnvironmentMap(std::shared_ptr<EnvironmentMap> envMap) { m_environmentMap = envMap; }
		std::shared_ptr<EnvironmentMap> GetEnvironmentMap() const { return m_environmentMap; }

		PhysicsSystem* GetPhysicsSystem() { return m_physicsSystem.get(); }

	protected:
		virtual void SceneEnter() = 0;

	private:
		// scene graph root
		std::unique_ptr<GameObject> m_root;
		std::vector<ObjectCreateRequest> m_requstedCreateObject;
		// cameras in the scene
		std::vector<Camera*> m_cameras;
		Camera* m_mainCamera = nullptr;
		Camera* m_cullingCamera = nullptr;
		// lights in the scene
		std::vector<Light*> m_lights;
		ConstantBufferPerLight m_cbPerLight;
		// texts in the scene
		std::vector<TextComponent*> m_texts;
		// colliders in the scene
		std::vector<Rp3dCollider*> m_navObstacleColliders;
		// environment map
		std::shared_ptr<EnvironmentMap> m_environmentMap;

		// destroyed object list
		std::vector<GameObject*> destroyList;

		// physics system
		std::unique_ptr<PhysicsSystem> m_physicsSystem;

		// Render queue
		RenderQueue m_renderQueue;

		// Helper functions for traversing the scene graph
		void traverseAwake(GameObject* node);
		void traverseStart(GameObject* node);
		void traverseUpdate(GameObject* node, float dt);
		void traverseFixedUpdate(GameObject* node, float fdt);
		void traverseRender(GameObject* node, struct Frustum& camFrustum);
		void traverseDestroyed(GameObject* node);
	};
}