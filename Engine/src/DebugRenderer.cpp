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
        // render scene objects
        for (auto& child : scene->GetRoot()->GetChildren())
        {
            traverseRender(child.get());
        }
	}

	void DebugRenderer::traverseRender(GameObject* node)
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
                // update constant buffer per obj
                ConstantBufferPerObject cbPerObject = { };
                cbPerObject.world = renderable->ownerGameObject->transform.GetWorldMatrix();
                cbPerObject.normalMatrix = Transpose(Inverse(cbPerObject.world));
                m_cbManager->UpdatePerObject(cbPerObject);
                // render aabb bound
                renderAABB(bounds);
            }
        }

        for (auto& child : node->GetChildren())
        {
            traverseRender(child.get());
        }
	}

	void DebugRenderer::renderAABB(const AABB* bound)
	{
        auto context = m_d3dManager->GetDeviceContext();
        RenderCommandList cmd(context);
        // bind render target
        ID3D11RenderTargetView* rtvs[] = { m_d3dManager->GetRenderTarget() };
        cmd.SetRenderTargets(rtvs, m_d3dManager->GetDepthStencil(), 1);

        // set view port
        cmd.SetViewport(static_cast<float>(m_d3dManager->GetBackBufferDesc().Width), static_cast<float>(m_d3dManager->GetBackBufferDesc().Height));

        // shader bind
        auto shader = ResourceManager::GetInstance().GetShader("Debug_Bound_shader");
        shader->Bind();

        // set up render state
        cmd.SetDepthState(RenderStateManager::GetInstance().GetDepthState(DepthType::Read));
        cmd.SetRasterState(RenderStateManager::GetInstance().GetRasterState(RasterType::None));

        // set up vertex buffer
        const auto& vertices = bound->GetVertices();    // get vertices
        static constexpr uint32_t edges[] =
        {
            0,1,
            1,3,
            3,2,
            2,0,

            4,5,
            5,7,
            7,6,
            6,4,

            0,4,
            1,5,
            2,6,
            3,7
        };
        m_lineVertices.clear();
        for (uint32_t i = 0; i < 24; i += 2)
        {
            DebugVertex v0;
            v0.position = vertices[edges[i]];
            v0.color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);

            DebugVertex v1;
            v1.position = vertices[edges[i + 1]];
            v1.color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);

            m_lineVertices.push_back(v0);
            m_lineVertices.push_back(v1);
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
}