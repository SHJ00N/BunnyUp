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
        float restitution = 1.0f;    // 반발계수
        float friction = 0.5f;

        Vector3 linearVelocity = { 0.0f, 0.0f, 0.0f };
        Vector3 angularVelocity = { 0.0f, 0.0f, 0.0f };
        Matrix3x3 inverseInertia;
        
        Vector3 position;
        Quaternion orientation;

        Rigidbody() = default;
        virtual ~Rigidbody() = default;

        float GetInverseMass() const;   // inverse mass
        void AddForce(const Vector3& force);
        void ClearForces(); // 누적된 force값 초기화
        const Vector3& GetAccumulatedForce() const { return accumulatedForce; }
        const Vector3& GetAccumulatedTorque() const { return accumulatedTorque; }

        void SetOrientation(const Quaternion& quat) { orientation = quat; m_isDirty = true; }
        const Quaternion& GetOrientation() const { return orientation; }
        const Matrix3x3& GetInverseInertia();
        Matrix3x3 GetWorldInverseInertia();
        void SetDirty() { m_isDirty = true; }
        bool IsDirty() const { return m_isDirty; }

        void FixedUpdate(float fdt) override;
        void OnImGui() override;

    protected:
        void OnAwake() override;
    private:
        Vector3 accumulatedForce = { 0.0f, 0.0f, 0.0f };
        Vector3 accumulatedTorque = { 0.0f, 0.0f, 0.0f };
        bool m_isDirty = true;
    };
}