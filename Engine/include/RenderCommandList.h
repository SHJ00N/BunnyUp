#pragma once

#include <d3d11.h>

namespace Engine
{
	class RenderGraph;
	class RenderTargetResource;
	class RenderCommandList
	{
	public:
		RenderCommandList() : m_context(nullptr) {}
		RenderCommandList(ID3D11DeviceContext* ctx) : m_context(ctx) {}

		void ClearRenderTarget(ID3D11RenderTargetView* rtv, const float color[4]);
		void ClearDepthStencil(ID3D11DepthStencilView* dsv);
		void SetRenderTargets(ID3D11RenderTargetView* const* rtvs, ID3D11DepthStencilView* dsv, UINT numViews = 1);
		void SetShaderResource(UINT slot, ID3D11ShaderResourceView* const* srv, UINT numViews = 1);
		void SetViewport(const float width, const float height, const float minDepth = 0.0f, const float maxDepth = 1.0f);

		// set render state
		void SetBlendState(ID3D11BlendState* state);
		void SetDepthState(ID3D11DepthStencilState* state);
		void SetRasterState(ID3D11RasterizerState* state);
		// draw fullscreen Triangle
		void DrawFullScreenQuad();
	private:
		ID3D11DeviceContext* m_context;
	};
}