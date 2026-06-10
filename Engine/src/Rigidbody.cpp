#include "Rigidbody.h"
#include "ImGuiClass.h"

namespace Engine
{
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

    void Rigidbody::AddForce(const Vector3& force)
    {
        accumulatedForce += force;
    }

    void Rigidbody::ClearForces()
    {
        accumulatedForce = { 0.0f, 0.0f, 0.0f };
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
        ImGui::DragFloat("Mass", &mass, 1.0f, 0.0f, 10000.0f);
        ImGui::DragFloat("Restitution", &restitution, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat3("Velocity", &velocity.x, 0.1f, 0.0f);
    }
}