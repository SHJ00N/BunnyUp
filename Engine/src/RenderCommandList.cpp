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

	void RenderCommandList::DisableDepthCull()
	{
		auto& state = RenderStateManager::GetInstance().GetState("DisableDepthCull");
		float blendFactor[4] = { 0,0,0,0 };
		m_context->OMSetBlendState(state.blend.Get(), blendFactor, 0xffffffff);
		m_context->OMSetDepthStencilState(state.depth.Get(), 0);
		m_context->RSSetState(state.raster.Get());
	}

	void RenderCommandList::DrawFullScreenQuad()
	{
		// no vertex buffer
		const auto& fullScreenQuad = ResourceManager::GetInstance().GetMesh("Primitive_fullscreen_quad");
		fullScreenQuad->Render();
	}
}