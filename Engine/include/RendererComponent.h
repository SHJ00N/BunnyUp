#pragma once

#include "Component.h"
#include "ConstantBuffer.h"
#include "Material.h"

#include <d3d11.h>
#include <wrl/client.h>

namespace Engine
{
	class Renderer;
	class RendererComponent : public Component
	{
	public:
		void AddMaterial(std::shared_ptr<Material> material) { m_materials.push_back(material); }
		std::vector<std::shared_ptr<Material>>& GetMaterials() { return m_materials; }
		void Render(Renderer& renderer) 
		{
			BindConstantBuffer(renderer);
			OnRender();
		}

	protected:
		std::vector<std::shared_ptr<Material>>  m_materials;
		ConstantBufferPerObject m_cbPerObject;

		virtual void BindConstantBuffer(Renderer& renderer) { }
		virtual void OnRender() {};
	};
}