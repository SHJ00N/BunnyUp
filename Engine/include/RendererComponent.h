#pragma once

#include "Component.h"
#include "ConstantBuffer.h"
#include "Material.h"
#include "BoundingVolume.h"

#include <d3d11.h>
#include <wrl/client.h>

namespace Engine
{
	struct RenderItem;
	class RenderQueue;
	class ConstantBufferManager;
	class RendererComponent : public Component
	{
	public:
		// Set all material to parameter
		void SetMaterial(std::shared_ptr<Material> material);
		// Set material corresponded to index
		void SetMaterial(uint32_t index, std::shared_ptr<Material> material);

		// getter
		std::vector<std::shared_ptr<Material>>& GetMaterials() { return m_materials; }
		std::shared_ptr<Material> GetMaterial(uint32_t index);

		const AABB* GetBound() const { return m_bound.get(); }

		// submit renderer on render queue
		virtual void Submit(RenderQueue& queue) = 0;
		virtual void Draw(const RenderItem& item, ConstantBufferManager& cbManager) = 0;

		virtual void OnImGui() override;

	protected:
		virtual ~RendererComponent() = default;

		std::vector<std::shared_ptr<Material>>  m_materials;
		ConstantBufferPerObject m_cbPerObject;
		std::unique_ptr<AABB> m_bound;

		virtual void UpdateConstantBuffer(ConstantBufferManager& cbManager) { }

		virtual void generateBound() { }
	};
}