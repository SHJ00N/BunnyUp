#pragma once

#include <random>

namespace Engine
{
	class Random
	{
	public:
		static void Init()
		{
			m_generator.seed(std::random_device{ }());
		}

		static void SetSeed(uint32_t seed)
		{
			m_generator.seed(seed);
		}

		static int Range(int min, int max)
		{
			std::uniform_int_distribution<int> dist(min, max);
			return dist(m_generator);
		}

		static float Range(float min, float max)
		{
			std::uniform_real_distribution<float> dist(min, max);
			return dist(m_generator);
		}

		static bool Bool()
		{
			return Range(0, 1) == 1;
		}

	private:
		inline static std::mt19937 m_generator{ };
	};
}