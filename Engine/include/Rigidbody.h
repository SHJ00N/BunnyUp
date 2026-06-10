#pragma once

#include "Component.h"
#include "MathHelper.h"

namespace Engine
{
    enum class RigidbodyType
    {
        Static,
        Kinematic,
        Dynamic
    };

    class Rigidbody : public Component
    {
    public:
        RigidbodyType type = RigidbodyType::Dynamic;
        bool useGravity = true;
        float mass = 1.0f;
        float restitution = 0.0f;    // 반발계수
        Vector3 velocity = { 0.0f, 0.0f, 0.0f };

        Rigidbody() = default;
        virtual ~Rigidbody() = default;

        float GetInverseMass() const;   // inverse mass
        void AddForce(const Vector3& force);
        void ClearForces(); // 누적된 force값 초기화
        const Vector3& GetAccumulatedForce() const { return accumulatedForce; }

        void OnImGui() override;
    private:
        Vector3 accumulatedForce = { 0.0f, 0.0f, 0.0f };
    };
}