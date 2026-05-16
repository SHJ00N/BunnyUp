#pragma once

#include <d3d11.h>
#include <wrl/client.h>

namespace Engine
{
	struct ResourceDesc;
	class RenderTargetResource
	{
	public:
		HRESULT CreateResource(const ResourceDesc& desc, ID3D11Device* device);
		void WrapExternalResource(ID3D11Texture2D* texture, ID3D11RenderTargetView* rtv = nullptr, ID3D11DepthStencilView* dsv = nullptr, ID3D11ShaderResourceView* srv = nullptr);

		ID3D11Texture2D* GetTexture() const { return m_pTexture.Get(); }
		ID3D11RenderTargetView* GetRenderTargetView() const { return m_pRenderTargetView.Get(); }
		ID3D11DepthStencilView* GetDepthStencilView() const { return m_pDepthStencilView.Get(); }
		ID3D11ShaderResourceView* GetShaderResouceView() const { return m_pShaderResourceView.Get(); }

	private:
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
	};
}