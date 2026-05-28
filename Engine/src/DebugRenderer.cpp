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

namespace Engine
{
    HRESULT DebugRenderer::Initialize(D3DManager* d3dManager, ConstantBufferManager* cbManager)
    {
        m_d3dManager = d3dManager;
        m_cbManager = cbManager;

        HRESULT hr = S_OK;
        auto device = m_d3dManager->GetDevice();
        // create buffer desc
        CD3D11_BUFFER_DESC vDesc{};
        vDesc.ByteWidth = static_cast<UINT>(sizeof(DebugVertex) * MaxDebugVertices);
        vDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vDesc.Usage = D3D11_USAGE_DYNAMIC;
        vDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        // create line buffer
        hr = device->CreateBuffer(&vDesc, nullptr, m_pLineVertexBuffer.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }
        // create triangle buffer
        hr = device->CreateBuffer(&vDesc, nullptr, m_pTriangleVertexBuffer.GetAddressOf());
        if (FAILED(hr))
        {
            return hr;
        }

        return hr;
    }

    void DebugRenderer::Render(Scene* scene)
    {
        // clear vertices
        m_lineVertices.clear();
        m_triangleVertices.clear();

        // render scene objects
        for (auto& child : scene->GetRoot()->GetChildren())
        {
            traverseObject(child.get());
        }

        //// render camera frsutum
        for (auto camera : scene->GetCameras())
        {
            if (scene->GetMainCamera() == camera || !camera->frustumVisible) continue;
            addFrustum(createFrustumFromCamera(*camera));
        }

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
                addAABB(bounds, renderable->ownerGameObject->transform);
            }
        }

        for (auto& child : node->GetChildren())
        {
            traverseObject(child.get());
        }
    }

    void DebugRenderer::addLine(const Vector3& p0, const Vector3& p1, const Vector4& color)
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

    void DebugRenderer::addTriangle(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector4& color)
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

    void DebugRenderer::addAABB(const AABB* bound, const Transform& transform)
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
            addLine(p0, p1, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
        }
    }

    void DebugRenderer::addFrustum(const Frustum& frustum)
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

        // color
        Vector4 color = Vector4(1.0f, 1.0f, 0.0f, 0.2f);

        // Near
        addTriangle(n0, n1, n2, color);
        addTriangle(n2, n1, n3, color);

        // Far
        addTriangle(f0, f2, f1, color);
        addTriangle(f2, f3, f1, color);

        // Left
        addTriangle(n0, n2, f0, color);
        addTriangle(f0, n2, f2, color);

        // Right
        addTriangle(n1, f1, n3, color);
        addTriangle(f1, f3, n3, color);

        // Top
        addTriangle(n0, f0, n1, color);
        addTriangle(f0, f1, n1, color);

        // Bottom
        addTriangle(n2, n3, f2, color);
        addTriangle(f2, n3, f3, color);

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
            addLine(p0, p1, Vector4(1.0f, 1.0f, 0.0f, 1.0f));
        }
    }

    void DebugRenderer::flushLine()
    {
        auto context = m_d3dManager->GetDeviceContext();
        RenderCommandList cmd(context);

        // set up render state
        cmd.SetDepthState(RenderStateManager::GetInstance().GetDepthState(DepthType::Read));
        cmd.SetRasterState(RenderStateManager::GetInstance().GetRasterState(RasterType::None));

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