#include "EnvironmentMap.h"
#include "Texture2D.h"
#include "RenderTargetResource.h"
#include "ResourceDesc.h"
#include "MathHelper.h"
#include "ConstantBufferManager.h"
#include "ResourceManager.h"
#include "SamplerStateManager.h"
#include "RenderCommandList.h"
#include "D3DManager.h"
#include "RenderStateManager.h"

namespace Engine
{
	HRESULT EnvironmentMap::CreateFromHDRFile(const std::string& filePath)
	{
		HRESULT hr = S_OK;
		m_pHDRTexture = std::make_shared<Texture2D>();
		// Load the HDR texture from file
		hr = m_pHDRTexture->CreateHDRFromFile(filePath);
		if (FAILED(hr))
		{
			return hr;
		}
		// Set up capture projection and views for cubemap generation
		Matrix4x4 captureProjection = PerspectiveFovLH(Radians(90.0f), 1.0f, 0.1f, 10.0f);
		Matrix4x4 captureViews[] =
		{
			LookAtLH(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f)),
			LookAtLH(Vector3(0.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f)),
			LookAtLH(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f)),
			LookAtLH(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f)),
			LookAtLH(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, 1.0f, 0.0f)),
			LookAtLH(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, 1.0f, 0.0f)),
		};
		// Generate cubemap from HDR texture
		const auto& device = D3DManager::GetInstance().GetDevice();
		const auto& context = D3DManager::GetInstance().GetDeviceContext();
		hr = generateCubemap(device, context, captureProjection, captureViews);
		if (FAILED(hr))
		{
			return hr;
		}
		// Generate irradiance map from environment cube map
		hr = generateIrradianceMap(device, context, captureProjection, captureViews);
		if (FAILED(hr))
		{
			return hr;
		}
		// Generate prefiltered map from environment cube map
		hr = generatePrefilteredEnvMap(device, context, captureProjection, captureViews);
		if (FAILED(hr))
		{
			return hr;
		}

		// Generate brdflut
		hr = generateBrdfLUT(device, context);
		if (FAILED(hr))
		{
			return hr;
		}

		return hr;
	}

	HRESULT EnvironmentMap::generateCubemap(ID3D11Device* device, ID3D11DeviceContext* context, const Matrix4x4& captureProjection, const Matrix4x4* captureViews)
	{
		HRESULT hr = S_OK;
		// Create cubemap render target resource
		ResourceDesc cubeMapDesc = ResourceDesc::CreateColorTarget(EnvCubeMapSize, EnvCubeMapSize, DXGI_FORMAT_R16G16B16A16_FLOAT, ResourceType::TextureCube);
		m_pEnvCubeMap = std::make_shared<RenderTargetResource>();
		hr = m_pEnvCubeMap->CreateResource(cubeMapDesc, device);
		if (FAILED(hr))
		{
			return hr;
		}

		// Bind Shader
		ResourceManager::GetInstance().GetShader("EnvCubeMap_shader")->Bind();

		// Bind HDR texture to pixel shader
		m_pHDRTexture->Bind(0);
		SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp)->Bind(0);

		RenderCommandList cmdList(context); // Create a command list for rendering
		// Set viewport for rendering to cubemap faces
		cmdList.SetViewport(static_cast<float>(EnvCubeMapSize), static_cast<float>(EnvCubeMapSize));

		// Set up projection and view matrices for capturing the environment map
		auto& cbManager = ConstantBufferManager::GetInstance();
		ConstantBufferPerCamera cbData = { };
		cbData.projection = captureProjection;

		// Render to each face of the cubemap
		for (UINT face = 0; face < 6; ++face)
		{
			// set per-face view and update constant buffer
			cbData.view = captureViews[face];
			cbManager.UpdatePerCamera(cbData);
			// get rtv
			ID3D11RenderTargetView* rtv = m_pEnvCubeMap->GetRenderTargetView(0, face);
			// clear render target and set it
			float clearColor[4] = { 0,0,0,1 };
			cmdList.ClearRenderTarget(rtv, clearColor);
			cmdList.SetRenderTargets(&rtv, nullptr, 1);
			// Render a cube to capture the environment
			ResourceManager::GetInstance().GetMesh("Primitive_cube")->Render();
		}
		// Unbind resources after rendering to avoid hazard in next pass
		ID3D11RenderTargetView* nullRTV[1] = { nullptr };
		cmdList.SetRenderTargets(nullRTV, nullptr);
		ID3D11ShaderResourceView* nullSRVs[1] = { nullptr };
		cmdList.SetShaderResource(0, nullSRVs, 1);

		return hr;
	}

	HRESULT EnvironmentMap::generateIrradianceMap(ID3D11Device* device, ID3D11DeviceContext* context, const Matrix4x4& captureProjection, const Matrix4x4* captureViews)
	{
		HRESULT hr = S_OK;
		// Create irradiance map render target resource
		ResourceDesc irradianceMapDesc = ResourceDesc::CreateColorTarget(IrradianceMapSize, IrradianceMapSize, DXGI_FORMAT_R16G16B16A16_FLOAT, ResourceType::TextureCube);
		m_pIrradianceMap = std::make_shared<RenderTargetResource>();
		hr = m_pIrradianceMap->CreateResource(irradianceMapDesc, device);
		if (FAILED(hr))
		{
			return hr;
		}

		// Bind Shader
		ResourceManager::GetInstance().GetShader("IrradianceMap_shader")->Bind();

		RenderCommandList cmdList(context); // Create a command list for rendering
		// set shader resource view with environment cube map
		ID3D11ShaderResourceView* srv = m_pEnvCubeMap->GetShaderResourceView();
		cmdList.SetShaderResource(0, &srv, 1);
		SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp)->Bind(0);

		// Set viewport for rendering to cubemap faces
		cmdList.SetViewport(static_cast<float>(IrradianceMapSize), static_cast<float>(IrradianceMapSize));

		// Set up projection and view matrices for capturing the environment map
		auto& cbManager = ConstantBufferManager::GetInstance();
		ConstantBufferPerCamera cbData = { };
		cbData.projection = captureProjection;

		// Render to each face of the cubemap
		for (UINT face = 0; face < 6; ++face)
		{
			// set per-face view and update constant buffer
			cbData.view = captureViews[face];
			cbManager.UpdatePerCamera(cbData);
			// get rtv
			ID3D11RenderTargetView* rtv = m_pIrradianceMap->GetRenderTargetView(0, face);
			// clear render target and set it
			float clearColor[4] = { 0,0,0,1 };
			cmdList.ClearRenderTarget(rtv, clearColor);
			cmdList.SetRenderTargets(&rtv, nullptr, 1);
			// Render a cube to capture the environment
			ResourceManager::GetInstance().GetMesh("Primitive_cube")->Render();
		}
		// Unbind resources after rendering to avoid hazard in next pass
		ID3D11RenderTargetView* nullRTV[1] = { nullptr };
		cmdList.SetRenderTargets(nullRTV, nullptr);
		ID3D11ShaderResourceView* nullSRVs[1] = { nullptr };
		cmdList.SetShaderResource(0, nullSRVs, 1);

		return hr;
	}

	HRESULT EnvironmentMap::generatePrefilteredEnvMap(ID3D11Device* device, ID3D11DeviceContext* context, const Matrix4x4& captureProjection, const Matrix4x4* captureViews)
	{
		HRESULT hr = S_OK;
		// Create prefiltered map render target resource
		ResourceDesc prefilteredMapDesc = ResourceDesc::CreateColorTarget(PrefilteredEnvMapSize, PrefilteredEnvMapSize, DXGI_FORMAT_R16G16B16A16_FLOAT, ResourceType::TextureCube, MipLevels);
		m_pPrefilteredEnvMap = std::make_shared<RenderTargetResource>();
		hr = m_pPrefilteredEnvMap->CreateResource(prefilteredMapDesc, device);
		if (FAILED(hr))
		{
			return hr;
		}

		// Bind Shader
		ResourceManager::GetInstance().GetShader("PrefilteredEnvMap_shader")->Bind();

		RenderCommandList cmdList(context); // Create a command list for rendering
		// set shader resource view with environment cube map
		ID3D11ShaderResourceView* srv = m_pEnvCubeMap->GetShaderResourceView();
		cmdList.SetShaderResource(0, &srv, 1);
		SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp)->Bind(0);

		// Set up projection and view matrices for capturing the environment map
		auto& cbManager = ConstantBufferManager::GetInstance();
		ConstantBufferPerCamera cbCameraData = { };
		cbCameraData.projection = captureProjection;
		// Set up constant buffer data for calculating prefiltered map
		ConstantBufferPrefilteredEnvMap cbPrefilteredMapData = { };
		cbPrefilteredMapData.resolution = EnvCubeMapSize;
		cbPrefilteredMapData.maxMip = MipLevels;
		for (UINT mip = 0; mip < MipLevels; ++mip)
		{
			// Set viewport based on mip level
			float mipWidth = static_cast<float>(PrefilteredEnvMapSize * std::pow(0.5, mip));
			float mipHeight = static_cast<float>(PrefilteredEnvMapSize * std::pow(0.5, mip));
			cmdList.SetViewport(mipWidth, mipHeight);
			// Calculate roughness and update constant buffer
			cbPrefilteredMapData.roughness = static_cast<float>(mip) / static_cast<float>(MipLevels - 1);
			cbManager.UpdatePrefilteredEnvMap(cbPrefilteredMapData);

			// Render to each face of the cubemap
			for (UINT face = 0; face < 6; ++face)
			{
				// set per-face view and update constant buffer
				cbCameraData.view = captureViews[face];
				cbManager.UpdatePerCamera(cbCameraData);
				// get rtv
				ID3D11RenderTargetView* rtv = m_pPrefilteredEnvMap->GetRenderTargetView(mip, face);
				// clear render target and set it
				float clearColor[4] = { 0,0,0,1 };
				cmdList.ClearRenderTarget(rtv, clearColor);
				cmdList.SetRenderTargets(&rtv, nullptr, 1);
				// Render a cube to capture the environment
				ResourceManager::GetInstance().GetMesh("Primitive_cube")->Render();
			}
		}
		// Unbind resources after rendering to avoid hazard in next pass
		ID3D11RenderTargetView* nullRTV[1] = { nullptr };
		cmdList.SetRenderTargets(nullRTV, nullptr);
		ID3D11ShaderResourceView* nullSRVs[1] = { nullptr };
		cmdList.SetShaderResource(0, nullSRVs, 1);

		return hr;
	}

	HRESULT EnvironmentMap::generateBrdfLUT(ID3D11Device* device, ID3D11DeviceContext* context)
	{
		HRESULT hr = S_OK;
		// Create cubemap render target resource
		ResourceDesc brdfLUTDesc = ResourceDesc::CreateColorTarget(BrdfLUTSize, BrdfLUTSize, DXGI_FORMAT_R16G16_FLOAT, ResourceType::Texture2D);
		m_pBrdfLUT = std::make_shared<RenderTargetResource>();
		hr = m_pBrdfLUT->CreateResource(brdfLUTDesc, device);
		if (FAILED(hr))
		{
			return hr;
		}

		auto& state = RenderStateManager::GetInstance().GetState("DisableDepthCull");
		float blendFactor[4] = { 0,0,0,0 };
		context->OMSetBlendState(state.blend.Get(), blendFactor, 0xffffffff);
		context->OMSetDepthStencilState(state.depth.Get(), 0);
		context->RSSetState(state.raster.Get());

		// Bind Shader
		ResourceManager::GetInstance().GetShader("Brdf_shader")->Bind();

		RenderCommandList cmdList(context); // Create a command list for rendering
		// Set viewport
		cmdList.SetViewport(static_cast<float>(BrdfLUTSize), static_cast<float>(BrdfLUTSize));

		// get rtv
		ID3D11RenderTargetView* rtv = m_pBrdfLUT->GetRenderTargetView();
		// clear render target and set it
		float clearColor[4] = { 0,0,0,1 };
		cmdList.ClearRenderTarget(rtv, clearColor);
		cmdList.SetRenderTargets(&rtv, nullptr, 1);
		// render fullscreen quad
		cmdList.DrawFullScreenQuad();

		// Unbind resources after rendering to avoid hazard in next pass
		ID3D11RenderTargetView* nullRTV[1] = { nullptr };
		cmdList.SetRenderTargets(nullRTV, nullptr);

		return hr;
	}
}