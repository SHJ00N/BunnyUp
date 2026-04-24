#pragma once

#include "MathHelper.h"

#include <d3d11.h>
#include <wrl/client.h>
#include <array>
#include <string>

namespace Engine
{
	constexpr int  MaxWeight = 4;

	struct VertexP
	{
		Vector3 position;
		void SetPosition(Vector3 v) { position = v; }
		static const std::array<D3D11_INPUT_ELEMENT_DESC, 1> layout;
	};

	struct VertexPC : public VertexP
	{
		Vector3 color;
		void SetColor(Vector3 v) { color = v; }
		static const std::array<D3D11_INPUT_ELEMENT_DESC, 2> layout;
	};

	struct VertexPU : public VertexP
	{
		Vector2 uv;
		void SetUV(Vector2 v) { uv = v; }
		static const std::array<D3D11_INPUT_ELEMENT_DESC, 2> layout;
	};

	struct VertexPN : public VertexP
	{
		Vector3 normal;
		void SetNormal(Vector3 v) { normal = v; }
		static const std::array<D3D11_INPUT_ELEMENT_DESC, 2> layout;
	};

	struct VertexPNU : public VertexPN
	{
		Vector2 uv;
		void SetUV(Vector2 v) { uv = v; }
		static const std::array<D3D11_INPUT_ELEMENT_DESC, 3> layout;
	};

	struct VertexPNUT : public VertexPNU
	{
		Vector4 tangent;
		void SetTangent(Vector4 v) { tangent = v; }
		static const std::array<D3D11_INPUT_ELEMENT_DESC, 4> layout;
	};

	struct VertexSkin : public VertexPNUT
	{
		std::array<int32_t, MaxWeight> boneIDs;
		std::array<float, MaxWeight> weights = { 0.0f };
		static const std::array<D3D11_INPUT_ELEMENT_DESC, 6> layout;
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