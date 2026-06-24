#include "pch.h"
#include "Rigidbody_Legacy.h"
#include "ImGuiClass.h"
#include "GameObject.h"
#include "Collider_Legacy.h"

namespace Engine
{
    void Rigidbody::OnAwake()
    {
        position = ownerGameObject->transform.GetLocalPosition();
        orientation = ownerGameObject->transform.GetLocalQuaternionRotation();
    }

    void Rigidbody::FixedUpdate(float fdt)
    {
        position = ownerGameObject->transform.GetLocalPosition();
        orientation = ownerGameObject->transform.GetLocalQuaternionRotation();
    }

    float Rigidbody::GetInverseMass() const
    {
        switch (type)
        {
        case RigidbodyType::Static:
        case RigidbodyType::Kinematic:
            return 0.0f;
        case RigidbodyType::Dynamic:
            return  mass > 0.0f ? 1.0f / mass : 0.0f;
        }
        return 0.0f;
    }

    const Matrix3x3& Rigidbody::GetInverseInertia()
    {
        if (m_isDirty)
        {
            m_isDirty = false;

            auto collider = ownerGameObject->GetComponent<Collider>();
            inverseInertia = collider ? collider->GetInverseInertia(mass) : Matrix3x3(0.0f);
        }

        return inverseInertia;
    }

    Matrix3x3 Rigidbody::GetWorldInverseInertia()
    {
        if (type != RigidbodyType::Dynamic)
        {
            return Matrix3x3(0.0f);
        }

        Matrix3x3 R = ownerGameObject->transform.GetRotationMatrix();
        return Transpose(R) * GetInverseInertia() * R;
    }

    void Rigidbody::AddForce(const Vector3& force)
    {
        accumulatedForce += force;
    }

    void Rigidbody::ClearForces()
    {
        accumulatedForce = { 0.0f, 0.0f, 0.0f };
        accumulatedTorque = { 0.0f, 0.0f, 0.0f };
    }

    void Rigidbody::OnImGui()
    {
        const char* rigidbodyTypes[] =
        {
            "Static",
            "Kinematic",
            "Dynamic"
        };

        int currentType = static_cast<int>(type);

        if (ImGui::Combo("Type", &currentType, rigidbodyTypes, IM_ARRAYSIZE(rigidbodyTypes)))
        {
            type = static_cast<RigidbodyType>(currentType);
        }

        ImGui::Checkbox("Gravity", &useGravity);
        ImGui::DragFloat("Mass", &mass, 1.0f, 1.0f, 10000.0f);
        ImGui::DragFloat("Restitution", &restitution, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Friction", &friction, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat3("Linear", &linearVelocity.x, 0.1f);
        ImGui::DragFloat3("Angular", &angularVelocity.x, 0.1f);
    }
}