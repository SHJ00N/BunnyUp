#include "pch.h"
#include "RenderPipeline.h"
#include "PassRenderer.h"
#include "D3DManager.h"
#include "ConstantBufferManager.h"
#include "RenderTargetResource.h"
#include "Scene.h"
#include "ResourceManager.h"
#include "SamplerStateManager.h"
#include "EnvironmentMap.h"
#include "RenderStateManager.h"
#include "TextRenderer.h"

namespace Engine
{
    RenderPipeline::RenderPipeline()
    {
    }

    RenderPipeline::~RenderPipeline()
    {
    }

    void RenderPipeline::Initialize(ConstantBufferManager *cbManager)
    {
        m_cbManager = cbManager;

        m_textRenderer = std::make_unique<TextRenderer>();
        m_textRenderer->Initialize();
    }

    void RenderPipeline::Render(Scene *scene)
    {
        // create renderer
        PassRenderer renderer;
        // add passes
        build(renderer, scene);
        // make final render graph
        renderer.Compile(D3DManager::GetInstance().GetDevice());
        // rendering
        renderer.Execute(D3DManager::GetInstance().GetDeviceContext());
    }

    void RenderPipeline::build(PassRenderer& renderer, Scene *scene)
    {
        // get back buffer info
        auto &d3d = D3DManager::GetInstance();
        ResourceDesc backBufferDesc = ResourceDesc::CreateWriteOnlyBuffer(d3d.GetBackBufferDesc().Width, d3d.GetBackBufferDesc().Height, d3d.GetBackBufferDesc().Format);

        // register back buffer as a resource in the graph
        ResourceHandle backBuffer = renderer.Import(
            "BackBuffer",
            backBufferDesc,
            d3d.GetBackBuffer(),
            d3d.GetRenderTarget(),
            d3d.GetDepthStencil());

        // set final ouput with back buffer
        renderer.SetFinalOutput(backBuffer);

        // Geometry pass
        // ----------------------------------------------------------------
        // first step of render that only render th opaque meshes.
        // create G-buffer resources
        ResourceHandle positionBuffer = renderer.Create("PositionBuffer", ResourceDesc::CreateColorTarget(backBufferDesc.width, backBufferDesc.height, DXGI_FORMAT_R16G16B16A16_FLOAT));
        ResourceHandle normalBuffer = renderer.Create("NormalBuffer", ResourceDesc::CreateColorTarget(backBufferDesc.width, backBufferDesc.height, DXGI_FORMAT_R16G16B16A16_FLOAT));
        ResourceHandle albedoBuffer = renderer.Create("AlbedoBuffer", ResourceDesc::CreateColorTarget(backBufferDesc.width, backBufferDesc.height, DXGI_FORMAT_R16G16B16A16_FLOAT));
        ResourceHandle roughnessMetallicAoBuffer = renderer.Create("RoughnessMetallicAoBuffer", ResourceDesc::CreateColorTarget(backBufferDesc.width, backBufferDesc.height));
        ResourceHandle depthBuffer = renderer.Create("DepthBuffer", ResourceDesc::CreateDepthStencil(backBufferDesc.width, backBufferDesc.height));
        // create geometry pass parameter
        RenderPassParameter geometryPassParams;
        geometryPassParams.writes.push_back(positionBuffer);
        geometryPassParams.writes.push_back(normalBuffer);
        geometryPassParams.writes.push_back(albedoBuffer);
        geometryPassParams.writes.push_back(roughnessMetallicAoBuffer);
        geometryPassParams.writes.push_back(depthBuffer);
        // add geometry pass
        renderer.AddPass(
            "GeometryPass",
            PassType::Graphics,
            geometryPassParams,
            [positionBuffer, normalBuffer, depthBuffer, albedoBuffer, roughnessMetallicAoBuffer, backBuffer, &renderer, scene, this](RenderCommandList &cmd)
            {
                // get resources
                auto *positionRes = renderer.GetResource(positionBuffer);
                auto *normalRes = renderer.GetResource(normalBuffer);
                auto *depthRes = renderer.GetResource(depthBuffer);
                auto *albedoRes = renderer.GetResource(albedoBuffer);
                auto *roughnessMetallicAoRes = renderer.GetResource(roughnessMetallicAoBuffer);
                // Clear render targets
                const float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
                cmd.ClearRenderTarget(positionRes->GetRenderTargetView(), clearColor);
                cmd.ClearRenderTarget(normalRes->GetRenderTargetView(), clearColor);
                cmd.ClearRenderTarget(albedoRes->GetRenderTargetView(), clearColor);
                cmd.ClearRenderTarget(roughnessMetallicAoRes->GetRenderTargetView(), clearColor);
                cmd.ClearDepthStencil(depthRes->GetDepthStencilView());
                // Set RenderTargets
                ID3D11RenderTargetView *rtvs[] = {positionRes->GetRenderTargetView(), normalRes->GetRenderTargetView(), albedoRes->GetRenderTargetView(), roughnessMetallicAoRes->GetRenderTargetView()};
                cmd.SetRenderTargets(rtvs, depthRes->GetDepthStencilView(), 4);
                // Set viewport
                const auto &normalDesc = renderer.GetResourceDesc(normalBuffer);
                cmd.SetViewport(static_cast<float>(normalDesc.width), static_cast<float>(normalDesc.height));

                scene->CollectRenderCall(*m_cbManager); // first step should collect all draw call
                scene->RenderOpaque(*m_cbManager);      // render opaque meshes

                // Unbind resources after rendering to avoid hazard in next pass
                ID3D11RenderTargetView *nullRTVs[4] = {nullptr, nullptr, nullptr, nullptr};
                cmd.SetRenderTargets(nullRTVs, nullptr, 4);

                // copy depth stencil buffer to backbuffer depth stencil buffer
                auto data = depthRes->ExportResource();
                D3DManager::GetInstance().SetDepthStencil(data.texture, data.dsvs[0], data.srv);
            });

        // lighting pass
        // ----------------------------------------------------------------
        // create lighting pass resources
        ResourceHandle lightingPassOutput = renderer.Create("LightingPassOutput", ResourceDesc::CreateColorTarget(backBufferDesc.width, backBufferDesc.height, backBufferDesc.format));
        // create lighting pass parameter
        RenderPassParameter lightingPassParams;
        lightingPassParams.reads.push_back(positionBuffer);
        lightingPassParams.reads.push_back(normalBuffer);
        lightingPassParams.reads.push_back(depthBuffer);
        lightingPassParams.reads.push_back(albedoBuffer);
        lightingPassParams.reads.push_back(roughnessMetallicAoBuffer);
        lightingPassParams.writes.push_back(lightingPassOutput);
        // get shader
        Shader *lightingShader = ResourceManager::GetInstance().GetShader("PBR_shader").get();
        // add lighting pass
        renderer.AddPass(
            "LightingPass",
            PassType::Graphics,
            lightingPassParams,
            [positionBuffer, normalBuffer, depthBuffer, albedoBuffer, roughnessMetallicAoBuffer, lightingPassOutput, lightingShader, &renderer, scene, this](RenderCommandList &cmd)
            {
                // get resources
                auto *positionRes = renderer.GetResource(positionBuffer);
                auto *normalRes = renderer.GetResource(normalBuffer);
                auto *depthRes = renderer.GetResource(depthBuffer);
                auto *albedoRes = renderer.GetResource(albedoBuffer);
                auto *roughnessMetallicAoRes = renderer.GetResource(roughnessMetallicAoBuffer);
                auto *lightingPassOutputRes = renderer.GetResource(lightingPassOutput);
                // get environment map resources
                auto environmentMap = scene->GetEnvironmentMap();
                auto *irradianceMapRes = environmentMap->GetIrradianceMap();
                auto *prefilteredMapRes = environmentMap->GetPrefilteredEnvMap();
                auto *brdfLUTRes = environmentMap->GetBrdfLUT();
                // Clear outputBuffer
                const float clearColor[4] = {0.1f, 0.1f, 0.3f, 1.0f};
                cmd.ClearRenderTarget(lightingPassOutputRes->GetRenderTargetView(), clearColor);
                // Set RenderTargets
                ID3D11RenderTargetView *rtvs[] = {lightingPassOutputRes->GetRenderTargetView()};
                cmd.SetRenderTargets(rtvs, nullptr);
                // Set viewport
                const auto &outPutDesc = renderer.GetResourceDesc(lightingPassOutput);
                cmd.SetViewport(static_cast<float>(outPutDesc.width), static_cast<float>(outPutDesc.height));
                // Set shader resources
                ID3D11ShaderResourceView *srvs[8] =
                    {
                        positionRes->GetShaderResourceView(),
                        normalRes->GetShaderResourceView(),
                        albedoRes->GetShaderResourceView(),
                        roughnessMetallicAoRes->GetShaderResourceView(),
                        depthRes->GetShaderResourceView(),
                        irradianceMapRes->GetShaderResourceView(),
                        prefilteredMapRes->GetShaderResourceView(),
                        brdfLUTRes->GetShaderResourceView()};
                cmd.SetShaderResource(0, srvs, 8);
                SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp)->Bind(0);
                SamplerStateManager::GetInstance().GetSampler(SamplerType::PointClamp)->Bind(1);

                // bind shader and draw fullscreen quad
                lightingShader->Bind();
                cmd.DrawFullScreenQuad();

                // Unbind resources after rendering to avoid hazard in next pass
                ID3D11ShaderResourceView *nullSRVs[8] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
                cmd.SetShaderResource(0, nullSRVs, 8);
                ID3D11RenderTargetView *nullRTVs[1] = {nullptr};
                cmd.SetRenderTargets(nullRTVs, nullptr, 1);
            },
            lightingShader);

        // Skybox pass
        // ----------------------------------------------------------------
        // create forward pass resource
        ResourceHandle forwardPassOutput = renderer.Create("ForwardPassOutput", ResourceDesc::CreateColorTarget(backBufferDesc.width, backBufferDesc.height, backBufferDesc.format));
        // create skybox pass parameter
        RenderPassParameter forwardPassParams;
        forwardPassParams.reads.push_back(depthBuffer);
        forwardPassParams.reads.push_back(lightingPassOutput);
        forwardPassParams.writes.push_back(forwardPassOutput);
        // get shader
        Shader *skyboxShader = ResourceManager::GetInstance().GetShader("Skybox_shader").get();
        // add skybox pass
        renderer.AddPass(
            "ForwardPass",
            PassType::Graphics,
            forwardPassParams,
            [depthBuffer, lightingPassOutput, forwardPassOutput, skyboxShader, &renderer, scene, this](RenderCommandList &cmd)
            {
                // Get resources
                auto *depthRes = renderer.GetResource(depthBuffer);
                auto *lightingPassOutputRes = renderer.GetResource(lightingPassOutput);
                auto *forwardPassOutputRes = renderer.GetResource(forwardPassOutput);
                // get environment map resources
                auto environmentMap = scene->GetEnvironmentMap();
                auto *environmentMapRes = environmentMap->GetEnvCubeMap();
                auto *irradianceMapRes = environmentMap->GetIrradianceMap();
                auto *prefilteredMapRes = environmentMap->GetPrefilteredEnvMap();
                auto *brdfLUTRes = environmentMap->GetBrdfLUT();
                // Clear outputBuffer
                const float clearColor[4] = {0.1f, 0.1f, 0.3f, 1.0f};
                cmd.ClearRenderTarget(forwardPassOutputRes->GetRenderTargetView(), clearColor);

                // Set RenderTargets
                ID3D11RenderTargetView *rtvs[] = {forwardPassOutputRes->GetRenderTargetView()};
                cmd.SetRenderTargets(rtvs, nullptr);
                // Set viewport
                const auto &outPutDesc = renderer.GetResourceDesc(forwardPassOutput);
                cmd.SetViewport(static_cast<float>(outPutDesc.width), static_cast<float>(outPutDesc.height));

                // render skybox
                // Set shader resources
                ID3D11ShaderResourceView *skySrvs[3] = {environmentMapRes->GetShaderResourceView(), lightingPassOutputRes->GetShaderResourceView(), depthRes->GetShaderResourceView()};
                cmd.SetShaderResource(0, skySrvs, 3);
                SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp)->Bind(0);
                SamplerStateManager::GetInstance().GetSampler(SamplerType::PointClamp)->Bind(1);

                // bind shader and draw skybox
                skyboxShader->Bind();
                cmd.DrawFullScreenQuad();

                // Unbind resources after rendering to avoid hazard in next pass
                ID3D11ShaderResourceView *nullSRVs[3] = {nullptr, nullptr, nullptr};
                cmd.SetShaderResource(0, nullSRVs, 3);
                ID3D11RenderTargetView *nullRTVs[1] = {nullptr};
                cmd.SetRenderTargets(nullRTVs, nullptr, 1);

                // render transparent
                cmd.SetRenderTargets(rtvs, depthRes->GetDepthStencilView());
                // Set shader resources
                ID3D11ShaderResourceView *srvs[3] =
                    {
                        irradianceMapRes->GetShaderResourceView(),
                        prefilteredMapRes->GetShaderResourceView(),
                        brdfLUTRes->GetShaderResourceView()};
                cmd.SetShaderResource(10, srvs, 3);
                SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp)->Bind(10);

                // draw transparent meshes
                scene->RenderTransparent(*m_cbManager);

                // Unbind resources after rendering to avoid hazard in next pass
                cmd.SetShaderResource(10, nullSRVs, 3);
                cmd.SetRenderTargets(nullRTVs, nullptr, 1);
            },
            skyboxShader);

        // Post-process pass
        // ----------------------------------------------------------------
        ResourceHandle postProcessPassOutput = renderer.Create("PostProcessPassOutput", ResourceDesc::CreateColorTarget(backBufferDesc.width, backBufferDesc.height, backBufferDesc.format));
        RenderPassParameter postProcessPassParams;
        postProcessPassParams.reads.push_back(forwardPassOutput);
        postProcessPassParams.reads.push_back(normalBuffer);
        postProcessPassParams.reads.push_back(depthBuffer);
        postProcessPassParams.writes.push_back(postProcessPassOutput);
        // get shader
        Shader *postProcessShader = ResourceManager::GetInstance().GetShader("PostProcess_shader").get();
        // add post-process pass
        renderer.AddPass(
            "PostProcessPass",
            PassType::Graphics,
            postProcessPassParams,
            [forwardPassOutput, postProcessPassOutput, normalBuffer, depthBuffer, postProcessShader, &renderer, this](RenderCommandList &cmd)
            {
                // get resources
                auto *forwardPassOutputRes = renderer.GetResource(forwardPassOutput);
                auto *normalRes = renderer.GetResource(normalBuffer);
                auto *depthRes = renderer.GetResource(depthBuffer);
                auto *postProcessPassOutputRes = renderer.GetResource(postProcessPassOutput);
                // Clear backBuffer
                const float clearColor[4] = {0.1f, 0.1f, 0.3f, 1.0f};
                cmd.ClearRenderTarget(postProcessPassOutputRes->GetRenderTargetView(), clearColor);
                // Set RenderTargets
                ID3D11RenderTargetView *rtvs[] = {postProcessPassOutputRes->GetRenderTargetView()};
                cmd.SetRenderTargets(rtvs, nullptr);
                // Set viewport
                const auto &ppDesc = renderer.GetResourceDesc(postProcessPassOutput);
                cmd.SetViewport(static_cast<float>(ppDesc.width), static_cast<float>(ppDesc.height));
                // Set shader resources
                ID3D11ShaderResourceView *srvs[3] = {forwardPassOutputRes->GetShaderResourceView(), normalRes->GetShaderResourceView(), depthRes->GetShaderResourceView()};
                cmd.SetShaderResource(0, srvs, 3);
                SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp)->Bind(0);
                SamplerStateManager::GetInstance().GetSampler(SamplerType::PointClamp)->Bind(1);

                // bind shader and draw fullscreen quad
                postProcessShader->Bind();
                cmd.DrawFullScreenQuad();

                // Unbind resources after rendering to avoid hazard in next pass
                ID3D11ShaderResourceView *nullSRVs[3] = {nullptr, nullptr, nullptr};
                cmd.SetShaderResource(0, nullSRVs, 3);
            },
            postProcessShader);

        // UI pass
        // ----------------------------------------------------------------
        RenderPassParameter UIPassParams;
        UIPassParams.reads.push_back(postProcessPassOutput);
        UIPassParams.writes.push_back(backBuffer);
        // get shader
        Shader *backBufferShader = ResourceManager::GetInstance().GetShader("BackBuffer_shader").get();
        // add post-process pass
        renderer.AddPass(
            "UIPass",
            PassType::Graphics,
            UIPassParams,
            [postProcessPassOutput, backBuffer, backBufferShader, &renderer, scene, this](RenderCommandList &cmd)
            {
                // get resources
                auto *postProcessPassOutputRes = renderer.GetResource(postProcessPassOutput);
                auto *backBufferRes = renderer.GetResource(backBuffer);
                // Clear backBuffer
                const float clearColor[4] = {0.1f, 0.1f, 0.3f, 1.0f};
                cmd.ClearRenderTarget(backBufferRes->GetRenderTargetView(), clearColor);
                // Set RenderTargets
                ID3D11RenderTargetView *rtvs[] = {backBufferRes->GetRenderTargetView()};
                cmd.SetRenderTargets(rtvs, nullptr);
                // Set viewport
                const auto &bbDesc = renderer.GetResourceDesc(backBuffer);
                cmd.SetViewport(static_cast<float>(bbDesc.width), static_cast<float>(bbDesc.height));
                // Set shader resources
                ID3D11ShaderResourceView *srvs[1] = {postProcessPassOutputRes->GetShaderResourceView()};
                cmd.SetShaderResource(0, srvs, 1);
                SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp)->Bind(0);

                // bind shader and draw fullscreen quad
                backBufferShader->Bind();
                cmd.DrawFullScreenQuad();

                // Unbind resources after rendering to avoid hazard in next pass
                ID3D11ShaderResourceView *nullSRVs[1] = {nullptr};
                cmd.SetShaderResource(0, nullSRVs, 1);

                m_textRenderer->Render(scene->GetTexts(), m_cbManager);
            },
            backBufferShader);

        // copy gDepth pass
        RenderPassParameter depthCopyPassParams;
        depthCopyPassParams.reads.push_back(depthBuffer);
        depthCopyPassParams.writes.push_back(backBuffer);
    }
}