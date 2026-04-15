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

		// Opaque
		RenderState opaque;

		// Blend (OFF)
		D3D11_BLEND_DESC opaqueBlendDesc{};
		opaqueBlendDesc.RenderTarget[0].BlendEnable = FALSE;
		opaqueBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&opaqueBlendDesc, &opaque.blend);
		if (FAILED(hr)) return hr;

		// Depth (ON)
		D3D11_DEPTH_STENCIL_DESC opaqueDepthDesc{};
		opaqueDepthDesc.DepthEnable = TRUE;
		opaqueDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		opaqueDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;
		hr = device->CreateDepthStencilState(&opaqueDepthDesc, &opaque.depth);
		if (FAILED(hr)) return hr;

		// Raster (Cull Back)
		D3D11_RASTERIZER_DESC opaqueRasterDesc{};
		opaqueRasterDesc.CullMode = D3D11_CULL_BACK;
		opaqueRasterDesc.FillMode = D3D11_FILL_SOLID;
		opaqueRasterDesc.FrontCounterClockwise = FALSE;
		hr = device->CreateRasterizerState(&opaqueRasterDesc, &opaque.raster);
		if (FAILED(hr)) return hr;

		m_states["Opaque"] = opaque;

		// Transparent
		RenderState transparent;

		// Blend (Alpha)
		D3D11_BLEND_DESC transparentBlendDesc{};
		transparentBlendDesc.RenderTarget[0].BlendEnable = TRUE;
		transparentBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		transparentBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		transparentBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		transparentBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		transparentBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		transparentBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		transparentBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&transparentBlendDesc, &transparent.blend);
		if (FAILED(hr)) return hr;

		// Depth (Read Only)
		D3D11_DEPTH_STENCIL_DESC transparentDepthDesc{};
		transparentDepthDesc.DepthEnable = TRUE;
		transparentDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		transparentDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;
		hr = device->CreateDepthStencilState(&transparentDepthDesc, &transparent.depth);
		if (FAILED(hr)) return hr;

		// Raster (Cull Back)
		D3D11_RASTERIZER_DESC transparentRasterDesc{};
		transparentRasterDesc.CullMode = D3D11_CULL_BACK;
		transparentRasterDesc.FillMode = D3D11_FILL_SOLID;
		opaqueRasterDesc.FrontCounterClockwise = FALSE;
		hr = device->CreateRasterizerState(&transparentRasterDesc, &transparent.raster);
		if (FAILED(hr)) return hr;

		m_states["Transparent"] = transparent;

		return S_OK;
	}

	const RenderState& RenderStateManager::GetState(const std::string& name)
	{
		auto it = m_states.find(name);
		assert(it != m_states.end());
		return it->second;
	}
}