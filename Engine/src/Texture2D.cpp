#include "Texture2D.h"
#include "D3DManager.h"

#include <WICTextureLoader.h>

namespace Engine
{
	Texture2D::Texture2D()
	{
	}

	Texture2D::~Texture2D()
	{
	}

	HRESULT Texture2D::CreateFromFile(const std::string& filePath)
	{
		HRESULT hr = S_OK;
		std::wstring wFilePath(filePath.begin(), filePath.end());
		hr = DirectX::CreateWICTextureFromFile(D3DManager::GetInstance().GetDevice(), D3DManager::GetInstance().GetDeviceContext(), wFilePath.c_str(), nullptr, m_shaderResourceView.GetAddressOf());

		return hr;
	}

	void Texture2D::Bind(UINT slot) const
	{
		//assert(m_shaderResourceView != nullptr);
		D3DManager::GetInstance().GetDeviceContext()->PSSetShaderResources(slot, 1, m_shaderResourceView.GetAddressOf());
	}
}