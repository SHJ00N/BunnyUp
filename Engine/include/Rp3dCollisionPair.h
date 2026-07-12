#pragma once

#include "Rp3dCollider.h"

namespace Engine
{
    struct Rp3dContact
    {
        Vector3 normal;
        float penetrationDepth = 0.0f;
    };

	struct Rp3dCollisionPair
	{
		Rp3dCollider* a = nullptr;
		Rp3dCollider* b = nullptr;

        Rp3dContact contact;

        Rp3dCollisionPair(Rp3dCollider* lhs, Rp3dCollider* rhs) : a(lhs), b(rhs) { }

        bool operator==(const Rp3dCollisionPair& other) const
        {
            return (a == other.a && b == other.b) || (a == other.b && b == other.a);
        }
	};

    struct Rp3dCollisionPairHash
    {
        size_t operator()(const Rp3dCollisionPair& pair) const
        {
            auto* p1 = std::min(pair.a, pair.b);
            auto* p2 = std::max(pair.a, pair.b);

            size_t h1 = std::hash<Rp3dCollider*>()(p1);
            size_t h2 = std::hash<Rp3dCollider*>()(p2);

            return h1 ^ (h2 << 1);
        }
    };
}