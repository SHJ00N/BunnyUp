#pragma once

#include <EngineSystem.h>

namespace Player
{
    using namespace Engine;
    class PlayerController : public Component
    {
    public:
        void OnTriggerEnter(Collider* other) override;
        void OnTriggerStay(Collider* other) override;
        void OnTriggerExit(Collider* other) override;

        void OnCollisionEnter(Collider* other) override;
        void OnCollisionStay(Collider* other) override;
        void OnCollisionExit(Collider* other) override;
    };
}