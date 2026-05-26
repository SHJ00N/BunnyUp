#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <cstdint>
#include <vector>
#include <memory>
#include <span>

#include "Material.h"
#include "D3DManager.h"
#include "BoundingVolume.h"

namespace Engine
{
	struct SubMesh
	{
		uint32_t indexStart = 0;
		uint32_t indexCount = 0;
		uint32_t materialIndex = 0;

		// default primitive is trianglelist
		D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	};

	class Mesh
	{
	public:

		std::vector<SubMesh> subMeshes;
		
		Mesh();
		~Mesh();

		template<typename VertexType>
		bool CreateMesh(const std::vector<VertexType>& vertices, const std::vector<uint32_t>& indices)
		{
			// copy to cpu vertex data
			for (const auto& v : vertices)
			{
				m_positions.push_back(v.position);
			}
			m_indices = indices;
			// create bounding volume from vertex position data
			m_bound = std::make_unique<AABB>(m_positions);

			m_stride = sizeof(VertexType);
			m_indexCount = static_cast<UINT>(indices.size());

			// Create vertex buffer
			CD3D11_BUFFER_DESC vDesc(static_cast<UINT>(sizeof(VertexType) * vertices.size()), D3D11_BIND_VERTEX_BUFFER);

			D3D11_SUBRESOURCE_DATA vData;
			ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
			vData.pSysMem = vertices.data();
			vData.SysMemPitch = 0;
			vData.SysMemSlicePitch = 0;

			if (FAILED(D3DManager::GetInstance().GetDevice()->CreateBuffer(&vDesc, &vData, &m_pVertexBuffer)))
			{
				return false;
			}

			// Create index buffer
			CD3D11_BUFFER_DESC iDesc(static_cast <UINT>(sizeof(uint32_t)* indices.size()), D3D11_BIND_INDEX_BUFFER);
			
			D3D11_SUBRESOURCE_DATA iData;
			ZeroMemory(&iData, sizeof(D3D11_SUBRESOURCE_DATA));
			iData.pSysMem = indices.data();
			iData.SysMemPitch = 0;
			iData.SysMemSlicePitch = 0;

			if (FAILED(D3DManager::GetInstance().GetDevice()->CreateBuffer(&iDesc, &iData, &m_pIndexBuffer)))
			{
				return false;
			}

			return true;
		}

		void SetSkinned(bool isSkinned) { m_isSkinned = isSkinned; }
		bool GetSkinned() const { return m_isSkinned; }
		AABB* GetBound() const { return m_bound.get(); }

		void Render();
		void Render(const std::vector<std::shared_ptr<Material>>& materials, ConstantBufferManager& cbManager) const;
	private:
		// cpu vertex data
		std::vector<Vector3> m_positions;
		std::vector<uint32_t> m_indices;
		// gpu vertex data
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;
		// Is skinned 
		bool m_isSkinned = false;

		// bounding volume for frustum culling
		std::unique_ptr<AABB> m_bound;

		UINT m_stride;
		UINT m_indexCount;
	};
}