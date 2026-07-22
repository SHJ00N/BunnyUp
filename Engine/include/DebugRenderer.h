#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

#include "Singleton.h"
#include "MathHelper.h"
#include "BVHNode.h"

namespace Engine
{
	class ConstantBufferManager;
	class D3DManager;
	class GameObject;
	class Scene;
	class Transform;
	struct AABB;
	struct Frustum;
	
	inline constexpr UINT MaxDebugVertices = 1048576;

	struct DebugVertex
	{
		Vector3 position;
		Vector4 color;
	};

	class DebugRenderer : public Singleton<DebugRenderer>
	{
	public:
		HRESULT Initialize(D3DManager* d3dManager, ConstantBufferManager* cbManager);
		void Clear();
		void Render(Scene* scene);

		void AddBox(const Vector3& center, const Vector3& extents, const Transform& transform, const Vector4& color);
		void AddSphere(const Vector3& center, float radius, const Transform& transform, const Vector4& color);
		void AddSphere(const Vector3& center, float radius, const Vector4& color);

		void AddLine(const Vector3& p0, const Vector3& p1, const Vector4& color);
		void AddLine(const Vector3& p0, const Vector3& p1, const Vector4& color0, const Vector4& color1);
		void AddTriangle(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector4& color);
		void AddTriangle(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector4& color0, const Vector4& color1, const Vector4& color2);
		void AddAABB(const AABB* bound, const Transform& transform, const Vector4& color);
		void AddAABB(const AABB* bound, const Vector4& color);
		void AddFrustum(const Frustum& frustum, const Vector4& color);
	private:
		// manager instances
		D3DManager* m_d3dManager;
		ConstantBufferManager* m_cbManager;

		// cpu batch
		std::vector<DebugVertex> m_lineVertices;
		std::vector<DebugVertex> m_triangleVertices;
		// buffers
		size_t m_lineBufferCapacity;
		size_t m_triangleBufferCapacity;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pLineVertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pTriangleVertexBuffer;
		HRESULT createLineVertexBuffer(size_t bufferCapacity);
		HRESULT createTriangleVertexBuffer(size_t bufferCapacity);

		// add vertices
		void traverseObject(GameObject* node);
		void traverseBVHNode(const BVH::BVHNode* node);
		
		// flush
		void flushLine();
		void flushTriangle();
	};
}