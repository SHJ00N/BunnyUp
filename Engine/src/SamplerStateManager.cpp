#include "SamplerStateManager.h"
#include "Log.h"

namespace Engine
{
	SamplerStateManager::SamplerStateManager()
	{
	}

	SamplerStateManager::~SamplerStateManager()
	{
	}

	HRESULT SamplerStateManager::Initialize()
	{
		HRESULT hr = S_OK;

		// Create Linear Wrap Sampler
		std::shared_ptr<Sampler> linearWrapSampler = std::make_shared<Sampler>();
		hr = linearWrapSampler->CreateSampler(SamplerType::LinearWrap);
		if (FAILED(hr))
		{
			return hr;
		}
		m_samplers[SamplerType::LinearWrap] = linearWrapSampler;

		// Create Linear Clamp Sampler
		std::shared_ptr<Sampler> linearClampSampler = std::make_shared<Sampler>();
		hr = linearClampSampler->CreateSampler(SamplerType::LinearClamp);
		if (FAILED(hr))
		{
			return hr;
		}
		m_samplers[SamplerType::LinearClamp] = linearClampSampler;

		return hr;
	}

	std::shared_ptr<Sampler> SamplerStateManager::GetSampler(SamplerType type)
	{
		auto it = m_samplers.find(type);
		if (it == m_samplers.end())
		{
			LOG_ERROR("Sampler not found: %d", static_cast<int>(type));
			return nullptr;
		}
		return it->second;
	}
}