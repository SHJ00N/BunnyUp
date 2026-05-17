#include "RenderPipeline.h"
#include "RenderGraph.h"
#include "D3DManager.h"
#include "ConstantBufferManager.h"
#include "RenderTargetResource.h"
#include "Scene.h"
#include "ResourceManager.h"
#include "SamplerStateManager.h"

namespace Engine
{
    HRESULT RenderPipeline::Initialize()
    {
        m_cbManager = std::make_unique<ConstantBufferManager>();
        HRESULT hr = m_cbManager->Initialize();

        return hr;
    }

	void RenderPipeline::Render(Scene* scene)
	{
		// create render graph
		RenderGraph graph;
		// add passes
		build(graph, scene);
		// make final render graph
		graph.Compile(D3DManager::GetInstance().GetDevice());
		// rendering
		graph.Execute(D3DManager::GetInstance().GetDeviceContext());
	}

	void RenderPipeline::build(RenderGraph& graph, Scene* scene)
	{
        // get back buffer info
        auto& d3d = D3DManager::GetInstance();
        ResourceDesc backBufferDesc = ResourceDesc::CreateBackBuffer(d3d.GetBackBufferDesc().Width, d3d.GetBackBufferDesc().Height, d3d.GetBackBufferDesc().Format);

        // register back buffer as a resource in the graph
        ResourceHandle backBuffer = graph.Import(
            "BackBuffer",
            backBufferDesc,
            d3d.GetBackBuffer(),
            d3d.GetRenderTarget(),
            d3d.GetDepthStencil());

        // set final ouput with back buffer
        graph.SetFinalOutput(backBuffer);

        // geometry pass
		// ----------------------------------------------------------------
		// create G-buffer resources
		ResourceHandle positionBuffer = graph.Create("PositionBuffer", ResourceDesc::CreateColorTarget(backBufferDesc.width, backBufferDesc.height, DXGI_FORMAT_R16G16B16A16_FLOAT));
		ResourceHandle normalBuffer = graph.Create("NormalBuffer", ResourceDesc::CreateColorTarget(backBufferDesc.width, backBufferDesc.height, DXGI_FORMAT_R16G16B16A16_FLOAT));
		ResourceHandle albedoBuffer = graph.Create("AlbedoBuffer", ResourceDesc::CreateColorTarget(backBufferDesc.width, backBufferDesc.height, DXGI_FORMAT_R16G16B16A16_FLOAT));
		ResourceHandle roughnessMetallicAoBuffer = graph.Create("RoughnessMetallicAoBuffer", ResourceDesc::CreateColorTarget(backBufferDesc.width, backBufferDesc.height));
		ResourceHandle depthBuffer = graph.Create("DepthBuffer", ResourceDesc::CreateDepthStencil(backBufferDesc.width, backBufferDesc.height));
		// create geometry pass parameter
        RenderPassParameter geometryPassParams;
		geometryPassParams.writes.push_back(positionBuffer);
        geometryPassParams.writes.push_back(normalBuffer);
        geometryPassParams.writes.push_back(albedoBuffer);
        geometryPassParams.writes.push_back(roughnessMetallicAoBuffer);
        geometryPassParams.writes.push_back(depthBuffer);
		// add geometry pass
        graph.AddPass(
            "GeometryPass",
            PassType::Graphics,
            geometryPassParams,
            [positionBuffer, normalBuffer, depthBuffer, albedoBuffer, roughnessMetallicAoBuffer, backBuffer, &graph, scene, this](RenderCommandList& cmd)
            {
				// get resources
				auto* positionRes = graph.GetResource(positionBuffer);
                auto* normalRes = graph.GetResource(normalBuffer);
                auto* depthRes = graph.GetResource(depthBuffer);
                auto* albedoRes = graph.GetResource(albedoBuffer);
                auto* roughnessMetallicAoRes = graph.GetResource(roughnessMetallicAoBuffer);
				// Clear render targets
                const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				cmd.ClearRenderTarget(positionRes->GetRenderTargetView(), clearColor);
				cmd.ClearRenderTarget(normalRes->GetRenderTargetView(), clearColor);
				cmd.ClearRenderTarget(albedoRes->GetRenderTargetView(), clearColor);
				cmd.ClearRenderTarget(roughnessMetallicAoRes->GetRenderTargetView(), clearColor);
				cmd.ClearDepthStencil(depthRes->GetDepthStencilView());
                // Set RenderTargets
				ID3D11RenderTargetView* rtvs[] = { positionRes->GetRenderTargetView(), normalRes->GetRenderTargetView(), albedoRes->GetRenderTargetView(), roughnessMetallicAoRes->GetRenderTargetView() };
                cmd.SetRenderTargets(rtvs, depthRes->GetDepthStencilView(), 4);
                // Set viewport
                const auto& normalDesc = graph.GetResourceDesc(normalBuffer);
                cmd.SetViewport(static_cast<float>(normalDesc.width), static_cast<float>(normalDesc.height));

                scene->Render(*m_cbManager.get());

				// Unbind resources after rendering to avoid hazard in next pass
                ID3D11ShaderResourceView* nullSRVs[4] = { nullptr, nullptr, nullptr, nullptr };
				cmd.SetShaderResource(0, nullSRVs, 4);
            }
        );
        
		// lighting pass
		// ----------------------------------------------------------------
        // create lighting pass resources
		ResourceHandle lightingPassOutput = graph.Create("LightingPassOutput", ResourceDesc::CreateColorTarget(backBufferDesc.width, backBufferDesc.height, backBufferDesc.format));
        // create lighting pass parameter
        RenderPassParameter lightingPassParams;
		lightingPassParams.reads.push_back(positionBuffer);
        lightingPassParams.reads.push_back(normalBuffer);
        lightingPassParams.reads.push_back(depthBuffer);
        lightingPassParams.reads.push_back(albedoBuffer);
        lightingPassParams.reads.push_back(roughnessMetallicAoBuffer);
		lightingPassParams.writes.push_back(lightingPassOutput);
        // get shader
		Shader* lightingShader = ResourceManager::GetInstance().GetShader("PBR_shader").get();
        // add lighting pass
        graph.AddPass(
            "LightingPass",
            PassType::Graphics,
            lightingPassParams,
            [positionBuffer, normalBuffer, depthBuffer, albedoBuffer, roughnessMetallicAoBuffer, lightingPassOutput, lightingShader, &graph, this](RenderCommandList& cmd)
            {
                // get resources
                auto* positionRes = graph.GetResource(positionBuffer);
                auto* normalRes = graph.GetResource(normalBuffer);
                auto* depthRes = graph.GetResource(depthBuffer);
                auto* albedoRes = graph.GetResource(albedoBuffer);
                auto* roughnessMetallicAoRes = graph.GetResource(roughnessMetallicAoBuffer);
                auto* lightingPassOutputRes = graph.GetResource(lightingPassOutput);
				// Clear outputBuffer
                const float clearColor[4] = { 0.1f, 0.1f, 0.3f, 1.0f };
				cmd.ClearRenderTarget(lightingPassOutputRes->GetRenderTargetView(), clearColor);
                // Set RenderTargets
                ID3D11RenderTargetView* rtvs[] = { lightingPassOutputRes->GetRenderTargetView() };
                cmd.SetRenderTargets(rtvs, nullptr);
                // Set viewport
                const auto& outPutDesc = graph.GetResourceDesc(lightingPassOutput);
                cmd.SetViewport(static_cast<float>(outPutDesc.width), static_cast<float>(outPutDesc.height));
				// Set shader resources
                ID3D11ShaderResourceView* srvs[5] = { positionRes->GetShaderResouceView(), normalRes->GetShaderResouceView(), albedoRes->GetShaderResouceView(), roughnessMetallicAoRes->GetShaderResouceView(), depthRes->GetShaderResouceView() };
                cmd.SetShaderResource(0, srvs, 5);
				SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp)->Bind(0);

				// bind shader and draw fullscreen quad
                lightingShader->Bind();
				cmd.DrawFullScreenQuad();

                // Unbind resources after rendering to avoid hazard in next pass
                ID3D11ShaderResourceView* nullSRVs[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };
                cmd.SetShaderResource(0, nullSRVs, 5);
            },
			lightingShader
		);

        // Post-process pass
		// ----------------------------------------------------------------
		RenderPassParameter postProcessPassParams;
		postProcessPassParams.reads.push_back(lightingPassOutput);
		postProcessPassParams.reads.push_back(normalBuffer);
		postProcessPassParams.reads.push_back(depthBuffer);
		postProcessPassParams.writes.push_back(backBuffer);
		// get shader
		Shader* postProcessShader = ResourceManager::GetInstance().GetShader("PostProcess_shader").get();
		// add post-process pass
        graph.AddPass(
            "PostProcessPass",
            PassType::Graphics,
            postProcessPassParams,
            [lightingPassOutput, backBuffer, normalBuffer, depthBuffer, postProcessShader, & graph, this](RenderCommandList& cmd)
            {
                // get resources
                auto* lightingPassOutputRes = graph.GetResource(lightingPassOutput);
				auto* normalRes = graph.GetResource(normalBuffer);
				auto* depthRes = graph.GetResource(depthBuffer);
                auto* backBufferRes = graph.GetResource(backBuffer);
				// Clear backBuffer
				const float clearColor[4] = { 0.1f, 0.1f, 0.3f, 1.0f };
                cmd.ClearRenderTarget(backBufferRes->GetRenderTargetView(), clearColor);
                // Set RenderTargets
                ID3D11RenderTargetView* rtvs[] = { backBufferRes->GetRenderTargetView() };
                cmd.SetRenderTargets(rtvs, nullptr);
                // Set viewport
                const auto& bbDesc = graph.GetResourceDesc(backBuffer);
                cmd.SetViewport(static_cast<float>(bbDesc.width), static_cast<float>(bbDesc.height));
                // Set shader resources
                ID3D11ShaderResourceView* srvs[3] = { lightingPassOutputRes->GetShaderResouceView(), normalRes->GetShaderResouceView(), depthRes->GetShaderResouceView() };
                cmd.SetShaderResource(0, srvs, 3);
                SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp)->Bind(0);

				// bind shader and draw fullscreen quad
                postProcessShader->Bind();
                cmd.DrawFullScreenQuad();

                // Unbind resources after rendering to avoid hazard in next pass
                ID3D11ShaderResourceView* nullSRVs[3] = { nullptr, nullptr, nullptr };
				cmd.SetShaderResource(0, nullSRVs, 3);
            },
            postProcessShader
        );
	}
}