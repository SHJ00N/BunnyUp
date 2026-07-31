#pragma once

#include <d3d11.h>
#include <wrl/client.h>
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
		HRESULT CreateHDRFromFile(const std::string& filePath);
		HRESULT CreateFromMemory(const std::string& name, uint32_t width, uint32_t height, DXGI_FORMAT format, const void* data, uint32_t rowPitch);
		const std::string& GetName() const { return m_name; }

		void Bind(UINT slot = 0) const;
		
		ID3D11ShaderResourceView* GetShaderResourceView() const { return m_pShaderResourceView.Get(); }
	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
		std::string m_name;

		uint32_t getLoaderFlags(TextureType type) const;
	};
}