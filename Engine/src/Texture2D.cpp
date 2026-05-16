#include "Texture2D.h"
#include "D3DManager.h"

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
			getLoaderFlags(type),
			nullptr, 
			m_shaderResourceView.GetAddressOf()
		);

		std::filesystem::path path(filePath);
		m_name = path.filename().string(); // ex) "diffuse.png"

		return hr;
	}

	void Texture2D::Bind(UINT slot) const
	{
		//assert(m_shaderResourceView != nullptr);
		D3DManager::GetInstance().GetDeviceContext()->PSSetShaderResources(slot, 1, m_shaderResourceView.GetAddressOf());
	}

	DirectX::WIC_LOADER_FLAGS Texture2D::getLoaderFlags(TextureType type) const
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