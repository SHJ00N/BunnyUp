#include "pch.h"
#include "Texture2D.h"
#include "D3DManager.h"
#include "stb_image.h"

#include <WICTextureLoader.h>
#include <filesystem>

namespace Engine
{
	Texture2D::Texture2D()
	{
	}

	Texture2D::~Texture2D()
	{
	}

	HRESULT Texture2D::CreateFromFile(const std::string& filePath, TextureType type)
	{
		HRESULT hr = S_OK;
		std::wstring wFilePath(filePath.begin(), filePath.end());
		hr = DirectX::CreateWICTextureFromFileEx(
			D3DManager::GetInstance().GetDevice(), 
			wFilePath.c_str(), 
			0, 
			D3D11_USAGE_DEFAULT, 
			D3D11_BIND_SHADER_RESOURCE,  
			0,
			0, 
			static_cast<DirectX::WIC_LOADER_FLAGS>(getLoaderFlags(type)),
			nullptr, 
			m_pShaderResourceView.GetAddressOf()
		);

		std::filesystem::path path(filePath);
		m_name = path.filename().string(); // ex) "diffuse.png"

		return hr;
	}

	HRESULT Texture2D::CreateHDRFromFile(const std::string& filePath)
	{
		HRESULT hr = S_OK;

		// Load HDR image data using stb_image
		stbi_set_flip_vertically_on_load(true);
		int width, height, channels;
		float* data = stbi_loadf(filePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
		stbi_set_flip_vertically_on_load(false);
		if (!data)
		{
			return E_FAIL;
		}

		// Create a texture description
		D3D11_TEXTURE2D_DESC desc = { };
		desc.Width = static_cast<UINT>(width);
		desc.Height = static_cast<UINT>(height);
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // Use a high-precision format for HDR
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		// resource data
		D3D11_SUBRESOURCE_DATA initData = { };
		initData.pSysMem = data;
		initData.SysMemPitch = static_cast<UINT>(width) * sizeof(float) * 4; // 4 channels (RGBA) * size of float

		// Create the texture
		const auto& device = D3DManager::GetInstance().GetDevice();
		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
		hr = device->CreateTexture2D(&desc, &initData, texture.GetAddressOf());
		if (FAILED(hr))
		{
			return hr;
		}
		stbi_image_free(data);

		// Create shader resource view
		hr = device->CreateShaderResourceView(texture.Get(), nullptr, m_pShaderResourceView.GetAddressOf());

		std::filesystem::path path(filePath);
		m_name = path.filename().string(); // ex) "diffuse.png"

		return hr;
	}

	void Texture2D::Bind(UINT slot) const
	{
		//assert(m_shaderResourceView != nullptr);
		D3DManager::GetInstance().GetDeviceContext()->PSSetShaderResources(slot, 1, m_pShaderResourceView.GetAddressOf());
	}

	uint32_t Texture2D::getLoaderFlags(TextureType type) const
	{
		switch (type)
		{
		case TextureType::Albedo:
			return DirectX::WIC_LOADER_FLAGS::WIC_LOADER_FORCE_SRGB | DirectX::WIC_LOADER_FLAGS::WIC_LOADER_FORCE_RGBA32;
		case TextureType::Normal:
			return DirectX::WIC_LOADER_FLAGS::WIC_LOADER_IGNORE_SRGB | DirectX::WIC_LOADER_FLAGS::WIC_LOADER_FORCE_RGBA32;

		default:
			return DirectX::WIC_LOADER_FLAGS::WIC_LOADER_FORCE_RGBA32;
		}
	}
}