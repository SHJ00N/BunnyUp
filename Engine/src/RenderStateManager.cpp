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
		hr = device->CreateBlendState(&blendDisableDesc, &m_blendStates[BlendType::Disable]); if (FAILED(hr)) return hr;
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
		hr = device->CreateBlendState(&blendAlphaDesc, &m_blendStates[BlendType::Alpha]); if (FAILED(hr)) return hr;

		// Depth state
		// Disable
		D3D11_DEPTH_STENCIL_DESC disableDepthDesc{};
		disableDepthDesc.DepthEnable = FALSE;
		disableDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		disableDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;
		hr = device->CreateDepthStencilState(&disableDepthDesc, &m_depthStates[DepthType::Disable]); if (FAILED(hr)) return hr;
		// Read and Write
		D3D11_DEPTH_STENCIL_DESC rwDepthDesc{};
		rwDepthDesc.DepthEnable = TRUE;
		rwDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		rwDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;
		hr = device->CreateDepthStencilState(&rwDepthDesc, &m_depthStates[DepthType::ReadWrite]); if (FAILED(hr)) return hr;
		// Read Only
		D3D11_DEPTH_STENCIL_DESC rDepthDesc{};
		rDepthDesc.DepthEnable = TRUE;
		rDepthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		rDepthDesc.DepthFunc = D3D11_COMPARISON_LESS;
		hr = device->CreateDepthStencilState(&rDepthDesc, &m_depthStates[DepthType::Read]); if (FAILED(hr)) return hr;

		// Cull state
		// Disable
		D3D11_RASTERIZER_DESC disableCullDesc{};
		disableCullDesc.CullMode = D3D11_CULL_NONE;
		disableCullDesc.FillMode = D3D11_FILL_SOLID;
		disableCullDesc.FrontCounterClockwise = TRUE;
		hr = device->CreateRasterizerState(&disableCullDesc, &m_rasterStates[RasterType::None]); if (FAILED(hr)) return hr;
		// Cull Back
		D3D11_RASTERIZER_DESC cullBackDesc{};
		cullBackDesc.CullMode = D3D11_CULL_BACK;
		cullBackDesc.FillMode = D3D11_FILL_SOLID;
		cullBackDesc.FrontCounterClockwise = TRUE;
		hr = device->CreateRasterizerState(&cullBackDesc, &m_rasterStates[RasterType::Back]); if (FAILED(hr)) return hr;

		// Opaque
		RenderState opaque;
		opaque.blend = m_blendStates[BlendType::Disable];
		opaque.depth = m_depthStates[DepthType::ReadWrite];
		opaque.raster = m_rasterStates[RasterType::Back];
		m_states["Opaque"] = opaque;

		// Transparent
		RenderState transparent;
		transparent.blend = m_blendStates[BlendType::Alpha];
		transparent.depth = m_depthStates[DepthType::ReadWrite];
		transparent.raster = m_rasterStates[RasterType::Back];
		m_states["Transparent"] = transparent;

		// disable depth and cull
		RenderState disableDepthCull;
		disableDepthCull.blend = m_blendStates[BlendType::Disable];
		disableDepthCull.depth = m_depthStates[DepthType::Disable];
		disableDepthCull.raster = m_rasterStates[RasterType::None];
		m_states["DisableDepthCull"] = disableDepthCull;

		return S_OK;
	}

	const RenderState& RenderStateManager::GetState(const std::string& name)
	{
		auto it = m_states.find(name);
		assert(it != m_states.end());
		return it->second;
	}

	ID3D11BlendState* RenderStateManager::GetBlendState(BlendType type)
	{
		auto it = m_blendStates.find(type);
		assert(it != m_blendStates.end());
		return it->second.Get();
	}

	ID3D11DepthStencilState* RenderStateManager::GetDepthState(DepthType type)
	{
		auto it = m_depthStates.find(type);
		assert(it != m_depthStates.end());
		return it->second.Get();
	}

	ID3D11RasterizerState* RenderStateManager::GetRasterState(RasterType type)
	{
		auto it = m_rasterStates.find(type);
		assert(it != m_rasterStates.end());
		return it->second.Get();
	}
}