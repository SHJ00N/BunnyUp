#include "pch.h"
#include "RenderTargetResource.h"
#include "ResourceDesc.h"

#include <string>

namespace Engine
{
	ID3D11RenderTargetView* RenderTargetResource::GetRenderTargetView() const
	{
		return m_pRenderTargetViews.empty() ? nullptr : m_pRenderTargetViews[0].Get();
	}
	
	ID3D11RenderTargetView* RenderTargetResource::GetRenderTargetView(UINT mip) const
	{
		return (mip < m_pRenderTargetViews.size()) ? m_pRenderTargetViews[mip].Get() : nullptr;
	}

	ID3D11RenderTargetView* RenderTargetResource::GetRenderTargetView(UINT mip, UINT face) const
	{
		UINT index = mip * 6 + face;
		return (index < m_pRenderTargetViews.size()) ? m_pRenderTargetViews[index].Get() : nullptr;
	}

	RenderTargetReousrceExportData RenderTargetResource::ExportResource() const
	{
		RenderTargetReousrceExportData data;
		data.texture = m_pTexture;
		data.dsvs = m_pDepthStencilViews;
		data.srv = m_pShaderResourceView;

		return data;
	}

	HRESULT RenderTargetResource::CreateResource(const ResourceDesc& desc, ID3D11Device* device)
	{
		// Check d3d device
		if (!device) return E_FAIL;

		// Reset resouces
		m_pTexture.Reset();
		m_pRenderTargetViews.clear();
		m_pDepthStencilViews.clear();
		m_pShaderResourceView.Reset();

		// Check invalid desc
		if (desc.width == 0 || desc.height == 0 || desc.format == DXGI_FORMAT_UNKNOWN)
		{
			return E_INVALIDARG;
		}

		if(desc.type == ResourceType::Texture2D)
		{
			return createTexture2DResource(desc, device);
		}
		else if(desc.type == ResourceType::TextureCube)
		{
			return createCubeResource(desc, device);
		}

		return E_INVALIDARG;
	}

	void RenderTargetResource::WrapExternalResource(ID3D11Texture2D* texture, ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, ID3D11ShaderResourceView* srv)
	{
		m_pTexture = texture;
		m_pShaderResourceView = srv;
		
		if (dsv)
		{
			m_pDepthStencilViews.clear();
			m_pDepthStencilViews.push_back(dsv);
		}

		if (rtv)
		{
			m_pRenderTargetViews.clear();
			m_pRenderTargetViews.push_back(rtv);
		}
	}

	HRESULT RenderTargetResource::createTexture2DResource(const ResourceDesc& desc, ID3D11Device* device)
	{
		HRESULT hr = S_OK;
		// get bind flags
		UINT bindFlags = getBindFlags(desc);
		if (bindFlags == 0)
		{
			return E_INVALIDARG;
		}
		// Create texture
		D3D11_TEXTURE2D_DESC textureDesc = { };
		textureDesc.Width = desc.width;
		textureDesc.Height = desc.height;
		textureDesc.MipLevels = desc.mipLevels;
		textureDesc.ArraySize = 1;
		textureDesc.Format = desc.format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = bindFlags;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;
		// set mip generation flag
		if((bindFlags & D3D11_BIND_RENDER_TARGET) && (bindFlags & D3D11_BIND_SHADER_RESOURCE) && (desc.mipLevels != 1))
		{
			textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}
		hr = device->CreateTexture2D(&textureDesc, nullptr, m_pTexture.GetAddressOf());
		if (FAILED(hr))
		{
			return hr;
		}
		// Create view based on bindFlags
		if (bindFlags & D3D11_BIND_RENDER_TARGET)
		{
			m_pRenderTargetViews.resize(1);

			hr = device->CreateRenderTargetView(m_pTexture.Get(), nullptr, m_pRenderTargetViews[0].GetAddressOf());
			if (FAILED(hr))
			{
				return hr;
			}
		}
		if (bindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			m_pDepthStencilViews.resize(1);

			D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = { };
			dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			hr = device->CreateDepthStencilView(m_pTexture.Get(), &dsvDesc, m_pDepthStencilViews[0].GetAddressOf());
			if (FAILED(hr))
			{
				return hr;
			}
		}
		if (bindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = bindFlags & D3D11_BIND_DEPTH_STENCIL ? DXGI_FORMAT_R24_UNORM_X8_TYPELESS : textureDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = desc.mipLevels == 0 ? -1 : desc.mipLevels;

			hr = device->CreateShaderResourceView(m_pTexture.Get(), &srvDesc, m_pShaderResourceView.GetAddressOf());
			if (FAILED(hr))
			{
				return hr;
			}
		}
		return S_OK;
	}

	HRESULT RenderTargetResource::createCubeResource(const ResourceDesc& desc, ID3D11Device* device)
	{
		HRESULT hr = S_OK;
		// get bind flags
		UINT bindFlags = getBindFlags(desc);
		if (bindFlags == 0)
		{
			return E_INVALIDARG;
		}

		// Cube texture must have equal width and height
		if (desc.width != desc.height)
		{
			return E_INVALIDARG;
		}

		// Create texture
		D3D11_TEXTURE2D_DESC textureDesc = { };
		textureDesc.Width = desc.width;
		textureDesc.Height = desc.height;
		textureDesc.MipLevels = desc.mipLevels;
		textureDesc.ArraySize = 6;
		textureDesc.Format = desc.format;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = bindFlags;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		// set mip generation flag
		if ((bindFlags & D3D11_BIND_RENDER_TARGET) && (bindFlags & D3D11_BIND_SHADER_RESOURCE) && (desc.mipLevels != 1))
		{
			textureDesc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
		}
		hr = device->CreateTexture2D(&textureDesc, nullptr, m_pTexture.GetAddressOf());
		if (FAILED(hr))
		{
			return hr;
		}

		// Calculate mip levels
		UINT mipLevels = desc.mipLevels;
		if (mipLevels == 0)
		{
			UINT size = desc.width;
			mipLevels = 1;
			while (size > 1)
			{
				size /= 2;
				mipLevels++;
			}
		}

		// Create view based on bindFlags
		if (bindFlags & D3D11_BIND_RENDER_TARGET)
		{
			// Create RTV for each face and mip level
			m_pRenderTargetViews.resize(static_cast<size_t>(mipLevels) * 6);
			for (UINT mip = 0; mip < mipLevels; ++mip)
			{
				for (UINT face = 0; face < 6; ++face)
				{
					D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
					rtvDesc.Format = textureDesc.Format;
					rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
					rtvDesc.Texture2DArray.MipSlice = mip;
					rtvDesc.Texture2DArray.FirstArraySlice = face;
					rtvDesc.Texture2DArray.ArraySize = 1;

					UINT index = mip * 6 + face;

					hr = device->CreateRenderTargetView(m_pTexture.Get(), &rtvDesc, m_pRenderTargetViews[index].GetAddressOf());
					if (FAILED(hr))
					{
						return hr;
					}
				}
			}
		}
		if (bindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			m_pDepthStencilViews.resize(6);

			for (UINT face = 0; face < 6; ++face)
			{
				D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
				dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
				dsvDesc.Texture2DArray.MipSlice = 0;
				dsvDesc.Texture2DArray.FirstArraySlice = face;
				dsvDesc.Texture2DArray.ArraySize = 1;

				hr = device->CreateDepthStencilView(m_pTexture.Get(), &dsvDesc,m_pDepthStencilViews[face].GetAddressOf());

				if (FAILED(hr))
				{
					return hr;
				}
			}
		}
		if (bindFlags & D3D11_BIND_SHADER_RESOURCE)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = bindFlags & D3D11_BIND_DEPTH_STENCIL ? DXGI_FORMAT_R24_UNORM_X8_TYPELESS : textureDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			srvDesc.TextureCube.MostDetailedMip = 0;
			srvDesc.TextureCube.MipLevels = desc.mipLevels == 0 ? -1 : desc.mipLevels;

			hr = device->CreateShaderResourceView(m_pTexture.Get(), &srvDesc, m_pShaderResourceView.GetAddressOf());
			if (FAILED(hr))
			{
				return hr;
			}
		}

		return S_OK;
	}

	UINT RenderTargetResource::getBindFlags(const ResourceDesc& desc) const
	{
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
		return bindFlags;
	}
}