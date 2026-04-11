#pragma once

#include <d3d11.h>
#include <wrl/client.h>

namespace Engine
{
	enum class SamplerType
	{
		LinearClamp,
		LinearWrap
	};

	class Sampler
	{
	public:
		Sampler();
		~Sampler();

		HRESULT CreateSampler(SamplerType type);
		HRESULT CreateSamplerFromDesc(const D3D11_SAMPLER_DESC* desc);
		void Bind(UINT slot = 0) const;

	private:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
	};
}