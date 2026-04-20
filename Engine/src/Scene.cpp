#include "Scene.h"
#include "Renderer.h"
#include "RendererComponent.h"

namespace Engine
{
	Scene::Scene()
	{
		m_root = std::make_unique<GameObject>("Root");
	}

	Scene::~Scene()
	{
		m_root.reset();
	}

	void Scene::SceneAwake()
	{
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

	void Scene::Render(Renderer& renderer)
	{
		traverseRender(m_root.get(), renderer);
	}

	void Scene::traverseRender(GameObject* node, Renderer& renderer)
	{
		for (auto& component : node->GetComponents())
		{
			auto renderable = dynamic_cast<RendererComponent*>(component.get());
			if (renderable)
			{
				renderable->Render(renderer);
			}
		}
		for (auto& child : node->GetChildren())
		{
			traverseRender(child.get(), renderer);
		}
	}
}