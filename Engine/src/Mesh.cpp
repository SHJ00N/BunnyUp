#include "Mesh.h"

namespace Engine
{
	Mesh::Mesh() : m_stride(0), m_indexCount(0)
	{
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::Render()
	{
		assert(m_pVertexBuffer && m_pIndexBuffer);

		ID3D11DeviceContext* context = D3DManager::GetInstance().GetDeviceContext();
		UINT offset = 0;

		// Set buffers
		context->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &m_stride, &offset);
		context->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		if (subMeshes.empty())
		{
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			context->DrawIndexed(m_indexCount, 0, 0);
		}
		else
		{
			for (const auto& sub : subMeshes)
			{
				// draw
				context->IASetPrimitiveTopology(sub.topology);
				context->DrawIndexed(static_cast<UINT>(sub.indexCount), sub.indexStart, 0);
			}
		}
	}

	void Mesh::Render(const std::vector<std::shared_ptr<Material>>& materials) const
	{
		assert(m_pVertexBuffer && m_pIndexBuffer);

		ID3D11DeviceContext* context = D3DManager::GetInstance().GetDeviceContext();
		UINT offset = 0;

		// Set buffers
		context->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &m_stride, &offset);
		context->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

		for (const auto& sub : subMeshes)
		{
			auto& mat = materials[sub.materialIndex];

			mat->Bind();

			context->IASetPrimitiveTopology(sub.topology);
			context->DrawIndexed(static_cast<UINT>(sub.indexCount), sub.indexStart, 0);
		}
	}
}