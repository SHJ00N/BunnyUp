#include "RenderStateManager.h"
#include "D3DManager.h"

namespace Engine
{
	RenderStateManager::RenderStateManager()
	{
	}

	RenderStateManager::~RenderStateManager()
	{
	}

	HRESULT RenderStateManager::Initialize()
	{
		HRESULT hr = S_OK;
		auto device = D3DManager::GetInstance().GetDevice();

		// Blend state
		// Disable
		D3D11_BLEND_DESC blendDisableDesc{};
		blendDisableDesc.RenderTarget[0].BlendEnable = FALSE;
		blendDisableDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		// Alpha
		D3D11_BLEND_DESC blendAlphaDesc{};
		blendAlphaDesc.RenderTarget[0].BlendEnable = TRUE;
		blendAlphaDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendAlphaDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendAlphaDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendAlphaDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendAlphaDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendAlphaDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendAlphaDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		// Depth state
		// Disable
		D3D11_DEPTH_STENCIL_DESC disableDepthDesc{};
		disableDepthDesc.DepthEnable = FALSE;
		disableDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		disableDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;
		// Read and Write
		D3D11_DEPTH_STENCIL_DESC rwDepthDesc{};
		rwDepthDesc.DepthEnable = TRUE;
		rwDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		rwDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;
		// Read Only
		D3D11_DEPTH_STENCIL_DESC rDepthDesc{};
		rDepthDesc.DepthEnable = TRUE;
		rDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		rDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;

		// Cull state
		// Disable
		D3D11_RASTERIZER_DESC disableCullDesc{};
		disableCullDesc.CullMode = D3D11_CULL_NONE;
		disableCullDesc.FillMode = D3D11_FILL_SOLID;
		disableCullDesc.FrontCounterClockwise = TRUE;
		// Cull Back
		D3D11_RASTERIZER_DESC cullBackDesc{};
		cullBackDesc.CullMode = D3D11_CULL_BACK;
		cullBackDesc.FillMode = D3D11_FILL_SOLID;
		cullBackDesc.FrontCounterClockwise = TRUE;

		// Opaque
		RenderState opaque;
		hr = device->CreateBlendState(&blendDisableDesc, &opaque.blend); if (FAILED(hr)) return hr;
		hr = device->CreateDepthStencilState(&rwDepthDesc, &opaque.depth); if (FAILED(hr)) return hr;
		hr = device->CreateRasterizerState(&cullBackDesc, &opaque.raster); if (FAILED(hr)) return hr;
		m_states["Opaque"] = opaque;

		// Transparent
		RenderState transparent;
		hr = device->CreateBlendState(&blendAlphaDesc, &transparent.blend); if (FAILED(hr)) return hr;
		hr = device->CreateDepthStencilState(&rwDepthDesc, &transparent.depth); if (FAILED(hr)) return hr;
		hr = device->CreateRasterizerState(&cullBackDesc, &transparent.raster); if (FAILED(hr)) return hr;
		m_states["Transparent"] = transparent;

		// disable depth and cull
		RenderState disableDepthCull;
		hr = device->CreateBlendState(&blendDisableDesc, &disableDepthCull.blend); if (FAILED(hr)) return hr;
		hr = device->CreateDepthStencilState(&disableDepthDesc, &disableDepthCull.depth); if (FAILED(hr)) return hr;
		hr = device->CreateRasterizerState(&disableCullDesc, &disableDepthCull.raster); if (FAILED(hr)) return hr;
		m_states["DisableDepthCull"] = disableDepthCull;

		return S_OK;
	}

	const RenderState& RenderStateManager::GetState(const std::string& name)
	{
		auto it = m_states.find(name);
		assert(it != m_states.end());
		return it->second;
	}
}