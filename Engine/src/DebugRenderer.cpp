#include "pch.h"
#include "DebugRenderer.h"
#include "GameObject.h"
#include "RendererComponent.h"
#include "ConstantBufferManager.h"
#include "D3DManager.h"
#include "Scene.h"
#include "RenderCommandList.h"
#include "RenderStateManager.h"
#include "VertexType.h"
#include "ResourceManager.h"
#include "Frustum.h"
#include "Collider_Legacy.h"
#include "PhysicsSystem.h"

namespace Engine
{
    HRESULT DebugRenderer::Initialize(D3DManager* d3dManager, ConstantBufferManager* cbManager)
    {
        m_d3dManager = d3dManager;
        m_cbManager = cbManager;
        m_lineBufferCapacity = MaxDebugVertices;
        m_triangleBufferCapacity = MaxDebugVertices;

        HRESULT hr = S_OK;

        // create line buffer
        hr = createLineVertexBuffer(m_lineBufferCapacity);
        if (FAILED(hr))
        {
            return hr;
        }
        m_lineVertices.reserve(m_lineBufferCapacity);
        
        // create triangle buffer
        hr = createTriangleVertexBuffer(m_lineBufferCapacity);
        if (FAILED(hr))
        {
            return hr;
        }
        m_triangleVertices.reserve(m_lineBufferCapacity);

        return hr;
    }

    HRESULT DebugRenderer::createLineVertexBuffer(size_t bufferCapacity)
    {
        HRESULT hr = S_OK;
        auto device = m_d3dManager->GetDevice();
        // create buffer desc
        CD3D11_BUFFER_DESC vDesc{};
        vDesc.ByteWidth = static_cast<UINT>(sizeof(DebugVertex) * bufferCapacity);
        vDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vDesc.Usage = D3D11_USAGE_DYNAMIC;
        vDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        // create line buffer
        m_pLineVertexBuffer.Reset();
        hr = device->CreateBuffer(&vDesc, nullptr, m_pLineVertexBuffer.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }
        return hr;
    }

    HRESULT DebugRenderer::createTriangleVertexBuffer(size_t bufferCapacity)
    {
        HRESULT hr = S_OK;
        auto device = m_d3dManager->GetDevice();
        // create buffer desc
        CD3D11_BUFFER_DESC vDesc{};
        vDesc.ByteWidth = static_cast<UINT>(sizeof(DebugVertex) * bufferCapacity);
        vDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vDesc.Usage = D3D11_USAGE_DYNAMIC;
        vDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        // create triangle buffer
        m_pTriangleVertexBuffer.Reset();
        hr = device->CreateBuffer(&vDesc, nullptr, m_pTriangleVertexBuffer.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }
        return hr;
    }

    void DebugRenderer::Clear()
    {
        // clear vertices
        m_lineVertices.clear();
        m_triangleVertices.clear();
    }

    void DebugRenderer::Render(Scene* scene)
    {
        // render culling bounds
        //for (auto& child : scene->GetRoot()->GetChildren())
        //{
        //    traverseObject(child.get());
        //}

        // render camera frsutum
        for (auto camera : scene->GetCameras())
        {
            if (scene->GetMainCamera() == camera || !camera->frustumVisible) continue;
            AddFrustum(createFrustumFromCamera(*camera), Vector4(1.0f, 1.0f, 0.0f, 0.2f));
        }

        // render  navigate obstacle colliders
        for (auto collider : scene->GetObstacleColliders())
        {
            collider->BuildDebugRender(this);
        }

        // render physics
        scene->GetPhysicsSystem()->DrawDebug();

        // render debug vertices
        // -----------------------------------------------------------------------------------
        auto context = m_d3dManager->GetDeviceContext();
        RenderCommandList cmd(context);
        // bind render target
        ID3D11RenderTargetView* rtvs[] = { m_d3dManager->GetRenderTarget() };
        cmd.SetRenderTargets(rtvs, m_d3dManager->GetDepthStencil(), 1);
        // set view port
        cmd.SetViewport(static_cast<float>(m_d3dManager->GetBackBufferDesc().Width), static_cast<float>(m_d3dManager->GetBackBufferDesc().Height));
        // shader bind
        auto shader = ResourceManager::GetInstance().GetShader("Debug_shader");
        shader->Bind();
        // render line
        flushLine();
        flushTriangle();
    }

    void DebugRenderer::traverseObject(GameObject* node)
    {
        for (auto& component : node->GetComponents())
        {
            auto renderable = dynamic_cast<RendererComponent*>(component.get());
            if (!renderable)
            {
                continue;
            }

            const AABB* bounds = renderable->GetBound();

            if (bounds)
            {
                AddAABB(bounds, renderable->ownerGameObject->transform, Vector4(1.0f, 1.0f, 0.0f, 1.0f));
            }
        }

        for (auto& child : node->GetChildren())
        {
            traverseObject(child.get());
        }
    }

    void DebugRenderer::traverseBVHNode(const BVH::BVHNode* node)
    {
        if (node == nullptr) return;

		AddAABB(&node->bounds, Transform(), Vector4(1.0f, 0.0f, 1.0f, 1.0f));
        if (!node->IsLeaf())
        {
            traverseBVHNode(node->left.get());
            traverseBVHNode(node->right.get());
		}
    }

    void DebugRenderer::AddLine(const Vector3& p0, const Vector3& p1, const Vector4& color)
    {
        DebugVertex v0;
        v0.position = p0;
        v0.color = color;

        DebugVertex v1;
        v1.position = p1;
        v1.color = color;

        m_lineVertices.push_back(v0);
        m_lineVertices.push_back(v1);
    }

    void DebugRenderer::AddLine(const Vector3& p0, const Vector3& p1, const Vector4& color0, const Vector4& color1)
    {
        DebugVertex v0;
        v0.position = p0;
        v0.color = color0;

        DebugVertex v1;
        v1.position = p1;
        v1.color = color1;

        m_lineVertices.push_back(v0);
        m_lineVertices.push_back(v1);
    }

    void DebugRenderer::AddTriangle(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector4& color)
    {
        DebugVertex v0;
        v0.position = p0;
        v0.color = color;

        DebugVertex v1;
        v1.position = p1;
        v1.color = color;

        DebugVertex v2;
        v2.position = p2;
        v2.color = color;

        m_triangleVertices.push_back(v0);
        m_triangleVertices.push_back(v1);
        m_triangleVertices.push_back(v2);
    }

    void DebugRenderer::AddTriangle(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector4& color0, const Vector4& color1, const Vector4& color2)
    {
        DebugVertex v0;
        v0.position = p0;
        v0.color = color0;

        DebugVertex v1;
        v1.position = p1;
        v1.color = color1;

        DebugVertex v2;
        v2.position = p2;
        v2.color = color2;

        m_triangleVertices.push_back(v0);
        m_triangleVertices.push_back(v1);
        m_triangleVertices.push_back(v2);
    }

    void DebugRenderer::AddAABB(const AABB* bound, const Transform& transform, const Vector4& color)
    {
        const auto vertices = bound->GetVertices();    // get vertices
        const auto& worldMatrix = transform.GetWorldMatrix();   // world matrix

        static constexpr uint32_t edges[] =
        {
            0,1,    1,3,    3,2,    2,0,
            4,5,    5,7,    7,6,    6,4,
            0,4,    1,5,    2,6,    3,7
        };

        for (uint32_t i = 0; i < 24; i += 2)
        {
            const auto& p0 = vertices[edges[i]] * worldMatrix;
            const auto& p1 = vertices[edges[i + 1]] * worldMatrix;
            AddLine(p0, p1, color);
        }
    }

    void DebugRenderer::AddAABB(const AABB* bound, const Vector4& color)
    {
        const auto vertices = bound->GetVertices();    // get vertices

        static constexpr uint32_t edges[] =
        {
            0,1,    1,3,    3,2,    2,0,
            4,5,    5,7,    7,6,    6,4,
            0,4,    1,5,    2,6,    3,7
        };

        for (uint32_t i = 0; i < 24; i += 2)
        {
            AddLine(vertices[edges[i]], vertices[edges[i + 1]], color);
        }
    }

    void DebugRenderer::AddFrustum(const Frustum& frustum, const Vector4& color)
    {
        const auto corners = frustum.GetCorners();

        // near
        Vector3 n0 = corners[0];
        Vector3 n1 = corners[1];
        Vector3 n2 = corners[2];
        Vector3 n3 = corners[3];

        // far
        Vector3 f0 = corners[4];
        Vector3 f1 = corners[5];
        Vector3 f2 = corners[6];
        Vector3 f3 = corners[7];

        // Near
        AddTriangle(n0, n1, n2, color);
        AddTriangle(n2, n1, n3, color);

        // Far
        AddTriangle(f0, f2, f1, color);
        AddTriangle(f2, f3, f1, color);

        // Left
        AddTriangle(n0, n2, f0, color);
        AddTriangle(f0, n2, f2, color);

        // Right
        AddTriangle(n1, f1, n3, color);
        AddTriangle(f1, f3, n3, color);

        // Top
        AddTriangle(n0, f0, n1, color);
        AddTriangle(f0, f1, n1, color);

        // Bottom
        AddTriangle(n2, n3, f2, color);
        AddTriangle(f2, n3, f3, color);

        static constexpr uint32_t edges[] =
        {
            0,1,    1,3,    3,2,    2,0,
            4,5,    5,7,    7,6,    6,4,
            0,4,    1,5,    2,6,    3,7
        };

        Vector3 verts[8] =
        {
            n0,n1,n2,n3,
            f0,f1,f2,f3
        };

        for (uint32_t i = 0; i < 24; i += 2)
        {
            const auto& p0 = verts[edges[i]];
            const auto& p1 = verts[edges[i + 1]];
            AddLine(p0, p1, Vector4(color.x, color.y, color.z, 1.0f));
        }
    }

    void DebugRenderer::AddBox(const Vector3& center, const Vector3& extents, const Transform& transform, const Vector4& color)
    {
        AABB aabb(center, extents.x, extents.y, extents.z);

        AddAABB(&aabb, transform, color);
    }

    void DebugRenderer::AddSphere(const Vector3& center, float radius, const Vector4& color)
    {
        constexpr int segments = 16;
        constexpr float PI = 3.14159265359f;

        for (int x = 0; x < segments; ++x)
        {
            const float theta0 = PI * static_cast<float>(x) / segments;
            const float theta1 = PI * static_cast<float>(x + 1) / segments;

            for (int y = 0; y < segments; ++y)
            {
                const float phi0 = 2.0f * PI * static_cast<float>(y) / segments;
                const float phi1 = 2.0f * PI * static_cast<float>(y + 1) / segments;

                Vector3 p00 =
                {
                    radius * sinf(theta0) * cosf(phi0),
                    radius * cosf(theta0),
                    radius * sinf(theta0) * sinf(phi0)
                };

                Vector3 p01 =
                {
                    radius * sinf(theta0) * cosf(phi1),
                    radius * cosf(theta0),
                    radius * sinf(theta0) * sinf(phi1)
                };

                Vector3 p10 =
                {
                    radius * sinf(theta1) * cosf(phi0),
                    radius * cosf(theta1),
                    radius * sinf(theta1) * sinf(phi0)
                };

                p00 += center;
                p01 += center;
                p10 += center;

                // latitude line
                AddLine(p00, p01, color);

                // longitude line
                AddLine(p00, p10, color);
            }
        }
    }

    void DebugRenderer::AddSphere(const Vector3& center, float radius, const Transform& transform, const Vector4& color)
    {
        constexpr int segments = 16;
        constexpr float PI = 3.14159265359f;
        const auto& scale = transform.GetLocalScale();
        float uniformScale = std::max(scale.x, std::max(scale.y, scale.z));
        radius *= uniformScale;

        for (int x = 0; x < segments; ++x)
        {
            const float theta0 = PI * static_cast<float>(x) / segments;
            const float theta1 = PI * static_cast<float>(x + 1) / segments;

            for (int y = 0; y < segments; ++y)
            {
                const float phi0 = 2.0f * PI * static_cast<float>(y) / segments;
                const float phi1 = 2.0f * PI * static_cast<float>(y + 1) / segments;

                Vector3 p00 =
                {
                    radius * sinf(theta0) * cosf(phi0),
                    radius * cosf(theta0),
                    radius * sinf(theta0) * sinf(phi0)
                };

                Vector3 p01 =
                {
                    radius * sinf(theta0) * cosf(phi1),
                    radius * cosf(theta0),
                    radius * sinf(theta0) * sinf(phi1)
                };

                Vector3 p10 =
                {
                    radius * sinf(theta1) * cosf(phi0),
                    radius * cosf(theta1),
                    radius * sinf(theta1) * sinf(phi0)
                };

                p00 += center;
                p01 += center;
                p10 += center;

                // latitude line
                AddLine(p00, p01, color);

                // longitude line
                AddLine(p00, p10, color);
            }
        }
    }

    void DebugRenderer::flushLine()
    {
        auto context = m_d3dManager->GetDeviceContext();
        RenderCommandList cmd(context);

        // set up render state
        cmd.SetDepthState(RenderStateManager::GetInstance().GetDepthState(DepthType::Read));
        cmd.SetRasterState(RenderStateManager::GetInstance().GetRasterState(RasterType::None));

        if (m_lineVertices.size() > m_lineBufferCapacity)
        {
            m_lineBufferCapacity = m_lineVertices.size() * 2;
            createLineVertexBuffer(m_lineBufferCapacity);
        }

        D3D11_MAPPED_SUBRESOURCE mapped{ };
        context->Map(m_pLineVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

        memcpy(mapped.pData, m_lineVertices.data(), sizeof(DebugVertex) * m_lineVertices.size());
        context->Unmap(m_pLineVertexBuffer.Get(), 0);

        UINT stride = sizeof(DebugVertex);
        UINT offset = 0;

        context->IASetVertexBuffers(0, 1, m_pLineVertexBuffer.GetAddressOf(), &stride, &offset);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

        context->Draw(static_cast<UINT>(m_lineVertices.size()), 0);
    }

    void DebugRenderer::flushTriangle()
    {
        auto context = m_d3dManager->GetDeviceContext();
        RenderCommandList cmd(context);

        // set up render state
        cmd.SetBlendState(RenderStateManager::GetInstance().GetBlendState(BlendType::Alpha));
        cmd.SetDepthState(RenderStateManager::GetInstance().GetDepthState(DepthType::Read));
        cmd.SetRasterState(RenderStateManager::GetInstance().GetRasterState(RasterType::None));

        if (m_triangleVertices.size() > m_triangleBufferCapacity)
        {
            m_triangleBufferCapacity = m_triangleVertices.size() * 2;
            createLineVertexBuffer(m_triangleBufferCapacity);
        }

        D3D11_MAPPED_SUBRESOURCE mapped{ };
        context->Map(m_pTriangleVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

        memcpy(mapped.pData, m_triangleVertices.data(), sizeof(DebugVertex) * m_triangleVertices.size());
        context->Unmap(m_pTriangleVertexBuffer.Get(), 0);

        UINT stride = sizeof(DebugVertex);
        UINT offset = 0;

        context->IASetVertexBuffers(0, 1, m_pTriangleVertexBuffer.GetAddressOf(), &stride, &offset);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        context->Draw(static_cast<UINT>(m_triangleVertices.size()), 0);
    }
}