#pragma once

#include "Sampler.h"
#include "Singleton.h"

#include <unordered_map>
#include <memory>

namespace Engine
{
	class SamplerStateManager : public Singleton<SamplerStateManager>
	{
	public:
		SamplerStateManager();
		~SamplerStateManager();

		HRESULT Initialize();
		std::shared_ptr<Sampler> GetSampler(SamplerType type);

	private:
		std::unordered_map<SamplerType, std::shared_ptr<Sampler>> m_samplers;
	};
}