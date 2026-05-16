#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <WICTextureLoader.h>
#include <string>

namespace Engine
{
	enum class TextureType
	{
		Default,
		Albedo,
		Normal
	};
	class Texture2D
	{
	public:
		Texture2D();
		~Texture2D();

		HRESULT CreateFromFile(const std::string& filePath, TextureType type);
		const std::string& GetName() const { return m_name; }

		void Bind(UINT slot = 0) const;
		
		ID3D11ShaderResourceView* GetShaderResourceView() const { return m_shaderResourceView.Get(); }
	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
		std::string m_name;

		DirectX::WIC_LOADER_FLAGS getLoaderFlags(TextureType type) const;
	};
}