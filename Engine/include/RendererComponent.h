#pragma once

#include "Component.h"
#include "ConstantBuffer.h"
#include "Material.h"

#include <d3d11.h>
#include <wrl/client.h>

namespace Engine
{
	class ConstantBufferManager;
	class RendererComponent : public Component
	{
	public:
		// Set all material to parameter
		void SetMaterial(std::shared_ptr<Material> material) 
		{ 
			for (auto& mat : m_materials)
			{
				mat = material;
			}
		}
		// Set material corresponded to index
		void SetMaterial(uint32_t index, std::shared_ptr<Material> material)
		{
			if (index >= m_materials.size())
			{
				return;
			}

			m_materials[index] = material->CreateClone();
		}

		// getter
		std::vector<std::shared_ptr<Material>>& GetMaterials() { return m_materials; }
		std::shared_ptr<Material> GetMaterial(uint32_t index)
		{
			if (index >= m_materials.size())
			{
				return nullptr;
			}

			return m_materials[index];
		}

		void Render(ConstantBufferManager& cbManager)
		{
			UpdateConstantBuffer(cbManager);
			OnRender(cbManager);
		}

	protected:
		std::vector<std::shared_ptr<Material>>  m_materials;
		ConstantBufferPerObject m_cbPerObject;

		virtual void UpdateConstantBuffer(ConstantBufferManager& cbManager) { }
		virtual void OnRender(ConstantBufferManager& cbManager) {};
	};
}