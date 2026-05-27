#include "RenderCommandList.h"
#include "ResourceManager.h"

#include <assert.h>

namespace Engine
{
	void RenderCommandList::ClearRenderTarget(ID3D11RenderTargetView* rtv, const float color[4])
	{
		assert(rtv && "RenderTargetView is NULL");
		m_context->ClearRenderTargetView(rtv, color);
	}
	void RenderCommandList::ClearDepthStencil(ID3D11DepthStencilView* dsv)
	{
		assert(dsv && "DepthStencilView is NULL");
		m_context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}
	void RenderCommandList::SetRenderTargets(ID3D11RenderTargetView* const* rtvs, ID3D11DepthStencilView* dsv, UINT numViews)
	{
		m_context->OMSetRenderTargets(numViews, rtvs, dsv);
	}
	void RenderCommandList::SetShaderResource(UINT slot, ID3D11ShaderResourceView* const* srv, UINT numViews)
	{
		m_context->PSSetShaderResources(slot, numViews, srv);
	}
	void RenderCommandList::SetViewport(const float width, const float height, const float minDepth, const float maxDepth)
	{
		D3D11_VIEWPORT viewport = {};
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = width;
		viewport.Height = height;
		viewport.MinDepth = minDepth;
		viewport.MaxDepth = maxDepth;

		m_context->RSSetViewports(1, &viewport);
	}

	void RenderCommandList::SetBlendState(ID3D11BlendState* state)
	{
		float blendFactor[4] = { 0,0,0,0 };
		m_context->OMSetBlendState(state, blendFactor, 0xffffffff);
	}

	void RenderCommandList::SetDepthState(ID3D11DepthStencilState* state)
	{
		m_context->OMSetDepthStencilState(state, 0);
	}

	void RenderCommandList::SetRasterState(ID3D11RasterizerState* state)
	{
		m_context->RSSetState(state);
	}

	void RenderCommandList::DrawFullScreenQuad()
	{
		// disable depth cull
		auto& state = RenderStateManager::GetInstance().GetState("DisableDepthCull");
		SetBlendState(state.blend.Get());
		SetDepthState(state.depth.Get());
		SetRasterState(state.raster.Get());
		// no vertex buffer
		const auto& fullScreenQuad = ResourceManager::GetInstance().GetMesh("Primitive_fullscreen_quad");
		fullScreenQuad->Render();
	}
}