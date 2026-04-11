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

	HRESULT Texture2D::CreateFromFile(const std::wstring& filePath)
	{
		HRESULT hr = S_OK;
		hr = DirectX::CreateWICTextureFromFile(D3DManager::GetInstance().GetDevice(), D3DManager::GetInstance().GetDeviceContext(), filePath.c_str(), nullptr, m_shaderResourceView.GetAddressOf());

		return hr;
	}

	void Texture2D::Bind(UINT slot) const
	{
		assert(m_shaderResourceView != nullptr);
		D3DManager::GetInstance().GetDeviceContext()->PSSetShaderResources(slot, 1, m_shaderResourceView.GetAddressOf());
	}
}