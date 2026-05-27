#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

#include "MathHelper.h"

namespace Engine
{
	class ConstantBufferManager;
	class D3DManager;
	class GameObject;
	class Scene;
	struct AABB;
	
	constexpr UINT MaxDebugVertices = 65536;

	struct DebugVertex
	{
		Vector3 position;
		Vector4 color;
	};

	class DebugRenderer
	{
	public:
		HRESULT Initialize(D3DManager* d3dManager, ConstantBufferManager* cbManager);
		void Render(Scene* scene);
	private:
		// manager instances
		D3DManager* m_d3dManager;
		ConstantBufferManager* m_cbManager;

		// cpu batch
		std::vector<DebugVertex> m_lineVertices;
		std::vector<DebugVertex> m_triangleVertices;
		// buffers
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pLineVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pTriangleVertexBuffer;

		// draw bounds
		void traverseRender(GameObject* node);
		void renderAABB(const AABB* bound);
		// draw camera frustum
		// void renderFrustums();
	};
}