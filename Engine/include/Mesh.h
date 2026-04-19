#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <cstdint>
#include <vector>
#include <memory>
#include <span>

#include "Material.h"
#include "D3DManager.h"

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
			CD3D11_BUFFER_DESC iDesc(static_cast < UINT>(sizeof(uint32_t)* indices.size()), D3D11_BIND_INDEX_BUFFER);
			
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

		void Render();
		void Render(const std::vector<std::shared_ptr<Material>>& materials) const;
	private:
		// gpu vertex data
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;

		UINT m_stride;
		UINT m_indexCount;
	};
}