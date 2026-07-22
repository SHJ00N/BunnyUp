#pragma once

#include "Common/HitBox.h"

namespace Game
{
    class EnemyAttackHitBox : public HitBox
    {
    public:
        void OnTriggerStay(Engine::Rp3dCollider* other) override;
    };
}