#include "Sampler.h"
#include "D3DManager.h"

namespace Engine
{
	Sampler::Sampler()
	{
	}

	Sampler::~Sampler()
	{
	}

	HRESULT Sampler::CreateSampler(SamplerType type)
	{
		D3D11_SAMPLER_DESC desc = { };

		switch (type)
		{
			case SamplerType::LinearWrap:
			{
				desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				desc.AddressU = desc.AddressV = desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
				break;
			}
			case SamplerType::LinearClamp:
			{
				desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
				desc.AddressU = desc.AddressV = desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
				break;
			}
		}

		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = 0.0f;
		desc.MaxLOD = D3D11_FLOAT32_MAX;

		return D3DManager::GetInstance().GetDevice()->CreateSamplerState(&desc, m_samplerState.GetAddressOf());
	}

	HRESULT Sampler::CreateSamplerFromDesc(const D3D11_SAMPLER_DESC* desc)
	{
		return D3DManager::GetInstance().GetDevice()->CreateSamplerState(desc, m_samplerState.GetAddressOf());
	}

	void Sampler::Bind(UINT slot) const
	{
		assert(m_samplerState != nullptr);
		D3DManager::GetInstance().GetDeviceContext()->PSSetSamplers(slot, 1, m_samplerState.GetAddressOf());
	}
}