#pragma once

#include "Common/HitBox.h"

namespace Game
{
    class PlayerAttackHitBox : public HitBox
    {
    public:
        void OnTriggerStay(Engine::Rp3dCollider* other) override;
    };
}