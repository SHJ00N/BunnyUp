#include "RenderTargetResource.h"
#include "ResourceDesc.h"

#include <string>

namespace Engine
{
	HRESULT RenderTargetResource::CreateResource(const ResourceDesc& desc, ID3D11Device* device)
	{
		// Check d3d device
		if (!device) return E_FAIL;

		// Reset resouces
		m_pTexture.Reset();
		m_pRenderTargetView.Reset();
		m_pDepthStencilView.Reset();
		m_pShaderResourceView.Reset();

		// Check invalid desc
		if (desc.width == 0 || desc.height == 0 || desc.format == DXGI_FORMAT_UNKNOWN)
		{
			return E_INVALIDARG;
		}

		// Build D3D11 bind flags
		UINT bindFlags = 0;
		if (HasFlag(desc.flags, ResourceFlags::RTV))
		{
			bindFlags |= D3D11_BIND_RENDER_TARGET;
		}
		if (HasFlag(desc.flags, ResourceFlags::DSV))
		{
			bindFlags |= D3D11_BIND_DEPTH_STENCIL;
		}
		if (HasFlag(desc.flags, ResourceFlags::SRV))
		{
			bindFlags |= D3D11_BIND_SHADER_RESOURCE;
		}

		if (bindFlags == 0)
		{
			return E_INVALIDARG;
		}

		// Create texture
		D3D11_TEXTURE2D_DESC textureDesc = { };
		textureDesc.Width = desc.width;
		textureDesc.Height = desc.height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = desc.format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = bindFlags;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		HRESULT hr = device->CreateTexture2D(&textureDesc, nullptr, m_pTexture.GetAddressOf());
		if (FAILED(hr))
		{
			return hr;
		}

		// Create view based on bindFlags
		if (HasFlag(desc.flags, ResourceFlags::RTV))
		{
			hr = device->CreateRenderTargetView(m_pTexture.Get(), nullptr, m_pRenderTargetView.GetAddressOf());

			if (FAILED(hr))
			{
				return hr;
			}
		}

		if (HasFlag(desc.flags, ResourceFlags::DSV))
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
			dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			hr = device->CreateDepthStencilView(m_pTexture.Get(), &dsvDesc, m_pDepthStencilView.GetAddressOf());

			if (FAILED(hr))
			{
				return hr;
			}
		}

		if (HasFlag(desc.flags, ResourceFlags::SRV))
		{
			if (HasFlag(desc.flags, ResourceFlags::DSV))
			{
				D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Texture2D.MipLevels = -1;
				hr = device->CreateShaderResourceView(m_pTexture.Get(), &srvDesc, m_pShaderResourceView.GetAddressOf());
			}
			else
			{
				hr = device->CreateShaderResourceView(m_pTexture.Get(), nullptr, m_pShaderResourceView.GetAddressOf());
			}

			if (FAILED(hr))
			{
				return hr;
			}
		}

		return S_OK;
	}

	void RenderTargetResource::WrapExternalResource(ID3D11Texture2D* texture, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, ID3D11ShaderResourceView* srv)
	{
		m_pTexture = texture;
		m_pRenderTargetView = rtv;
		m_pDepthStencilView = dsv;
		m_pShaderResourceView = srv;
	}
}