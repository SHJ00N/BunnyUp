#pragma once

#include "Rp3dCollider.h"

namespace Engine
{
	struct Rp3dCollisionPair
	{
		Rp3dCollider* a;
		Rp3dCollider* b;

        Rp3dCollisionPair(Rp3dCollider* lhs, Rp3dCollider* rhs)
        {
            if (lhs < rhs)
            {
                a = lhs;
                b = rhs;
            }
            else
            {
                a = rhs;
                b = lhs;
            }
        }

        bool operator==(const Rp3dCollisionPair& other) const
        {
            return a == other.a && b == other.b;
        }
	};

    struct Rp3dCollisionPairHash
    {
        size_t operator()(const Rp3dCollisionPair& pair) const
        {
            size_t h1 = std::hash<Rp3dCollider*>()(pair.a);
            size_t h2 = std::hash<Rp3dCollider*>()(pair.b);

            return h1 ^ (h2 << 1);
        }
    };
}