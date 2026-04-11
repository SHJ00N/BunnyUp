#include "Shader.h"
#include "D3DManager.h"

#include <d3dcompiler.h>

namespace Engine
{
	const std::array< D3D11_INPUT_ELEMENT_DESC, 1> VertexP::layout =
	{
		D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	const std::array< D3D11_INPUT_ELEMENT_DESC, 2> VertexPC::layout =
	{
		D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		D3D11_INPUT_ELEMENT_DESC{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	const std::array< D3D11_INPUT_ELEMENT_DESC, 2> VertexPT::layout =
	{
		D3D11_INPUT_ELEMENT_DESC{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		D3D11_INPUT_ELEMENT_DESC{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	Shader::Shader() : m_pVertexShader(nullptr), m_pPixelShader(nullptr), m_pInputLayout(nullptr)
	{
	}

	Shader::~Shader()
	{
	}

	HRESULT Shader::compile(const std::wstring& filePath, const D3D11_INPUT_ELEMENT_DESC* layout, size_t layoutSize)
	{
		HRESULT hr = S_OK;

		// Use direct3D device to compile the shader and create the shader and input layout objects
		ID3D11Device* device = D3DManager::GetInstance().GetDevice();

		Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;

		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		flags |= D3DCOMPILE_DEBUG;
#endif

		// compile vertex shader
		hr = D3DCompileFromFile(filePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VSMain", "vs_5_0", flags, 0, &vertexShaderBlob, &errorBlob);
		if(FAILED(hr))
		{
			if (errorBlob)
			{
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			}
			return hr;
		}

		// compile pixel shader
		errorBlob.Reset();
		hr = D3DCompileFromFile(filePath.c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PSMain", "ps_5_0", flags, 0, &pixelShaderBlob, &errorBlob);
		if (FAILED(hr))
		{
			if (errorBlob)
			{
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			}
			return hr;
		}

		// create vertex shader
		hr = device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &m_pVertexShader);
		if (FAILED(hr))
		{
			return hr;
		}
		// create pixel shader
		hr = device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &m_pPixelShader);
		if (FAILED(hr))
		{
			return hr;
		}

		// create input layout
		hr = device->CreateInputLayout(layout, static_cast<UINT>(layoutSize), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &m_pInputLayout);
		if(FAILED(hr))
		{
			return hr;
		}

		return hr;
	}

	void Shader::Bind()
	{
		ID3D11DeviceContext* context = D3DManager::GetInstance().GetDeviceContext();
		context->IASetInputLayout(m_pInputLayout.Get());
		context->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
		context->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
	}
}