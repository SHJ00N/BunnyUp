#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

namespace Engine
{
	struct ResourceDesc;

	struct RenderTargetReousrceExportData
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		std::vector<Microsoft::WRL::ComPtr<ID3D11DepthStencilView>> dsvs;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
	};
	class RenderTargetResource
	{
	public:
		HRESULT CreateResource(const ResourceDesc& desc, ID3D11Device* device);
		void WrapExternalResource(ID3D11Texture2D* texture, ID3D11RenderTargetView* rtv = nullptr, ID3D11DepthStencilView* dsv = nullptr, ID3D11ShaderResourceView* srv = nullptr);

		ID3D11Texture2D* GetTexture() const { return m_pTexture.Get(); }
		ID3D11RenderTargetView* GetRenderTargetView() const;
		ID3D11RenderTargetView* GetRenderTargetView(UINT mip) const;
		ID3D11RenderTargetView* GetRenderTargetView(UINT mip, UINT face) const;
		ID3D11DepthStencilView* GetDepthStencilView() const { return m_pDepthStencilViews[0].Get(); }
		ID3D11ShaderResourceView* GetShaderResourceView() const { return m_pShaderResourceView.Get(); }

		RenderTargetReousrceExportData ExportResource() const;
	private:
		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture;
		std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> m_pRenderTargetViews;
		std::vector<Microsoft::WRL::ComPtr<ID3D11DepthStencilView>> m_pDepthStencilViews;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;

		HRESULT createTexture2DResource(const ResourceDesc& desc, ID3D11Device* device);
		HRESULT createCubeResource(const ResourceDesc& desc, ID3D11Device* device);

		UINT getBindFlags(const ResourceDesc& desc) const;
	};
}