#pragma once

#include "MathHelper.h"
#include "VertexType.h"

#include <wrl/client.h>
#include <string>

namespace Engine
{
	class Shader
	{
	public:
		Shader();
		~Shader();

		template<typename VertexType>
		HRESULT CompileFromFile(const std::string& filePath) { return compile(filePath, VertexType::layout.data(), VertexType::layout.size()); }

		void Bind();

	private:
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pVertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pPixelShader;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;

		HRESULT compile(const std::string& filePath, const D3D11_INPUT_ELEMENT_DESC* layout, size_t layoutSize);
	};
}