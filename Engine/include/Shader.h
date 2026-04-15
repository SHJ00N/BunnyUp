#pragma once

#include "MathHelper.h"

#include <d3d11.h>
#include <wrl/client.h>
#include <array>
#include <string>

namespace Engine
{
	struct VertexP
	{
		Vector3 position;
		void SetPosition(Vector3 pos) { position = pos; }
		static const std::array<D3D11_INPUT_ELEMENT_DESC, 1> layout;
	};

	struct VertexPC : public VertexP
	{
		Vector3 color;
		void SetColor(Vector3 col) { color = col; }
		static const std::array<D3D11_INPUT_ELEMENT_DESC, 2> layout;
	};

	struct VertexPT : public VertexP
	{
		Vector2 texCoord;
		void SetUV(Vector2 uv) { texCoord = uv; }
		static const std::array<D3D11_INPUT_ELEMENT_DESC, 2> layout;
	};

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