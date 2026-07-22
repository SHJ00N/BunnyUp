#pragma once

#include <stdint.h>
#include <bitset>
#include <array>

namespace Engine
{
	enum class CollisionLayer
	{
		Default = 0,
		Player,
		Enemy,
		Ground,
		Wall,
		Trigger,
		Object,
		EnemyTrigger,
		PlayerTrigger,
		Count // layer counter
	};

	class CollisionMatrix
	{
	public:
		CollisionMatrix()
		{
			constexpr size_t layerCount = static_cast<size_t>(CollisionLayer::Count);

			for (size_t i = 0; i < layerCount; ++i)
			{
				m_matrix[i].set();	// default setting is all collsion enabled
			}
		}

		void SetCollision(CollisionLayer a, CollisionLayer b, bool enable)
		{
			m_matrix[static_cast<uint32_t>(a)].set(static_cast<uint32_t>(b), enable);
			m_matrix[static_cast<uint32_t>(b)].set(static_cast<uint32_t>(a), enable);
		}

		bool CanCollide(CollisionLayer a, CollisionLayer b) const
		{
			return m_matrix[static_cast<uint32_t>(a)].test(static_cast<uint32_t>(b));
		}

		uint16_t GetCollisionMask(CollisionLayer layer) const
		{
			uint16_t mask = 0;
			// create bitmask
			for (size_t i = 0; i < static_cast<size_t>(CollisionLayer::Count); ++i)
			{
				if (CanCollide(layer, static_cast<CollisionLayer>(i)))
				{
					mask |= (1 << i);
				}
			}

			return mask;
		}

	private:
		std::array<std::bitset<32>, 32> m_matrix;
	};
}