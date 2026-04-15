#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <string>

namespace Engine
{
	class Texture2D
	{
	public:
		Texture2D();
		~Texture2D();

		HRESULT CreateFromFile(const std::string& filePath);

		void Bind(UINT slot = 0) const;
		
		ID3D11ShaderResourceView* GetShaderResourceView() const { return m_shaderResourceView.Get(); }
	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
	};
}