#pragma once

#include <EngineSystem.h>

namespace Player
{
    using namespace Engine;
    class PlayerController : public Component
    {
    public:
        void Update(float dt) override;

        void OnCollisionEnter(Rp3dCollider* other) override;
    };
}