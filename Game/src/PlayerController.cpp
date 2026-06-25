#include "PlayerController.h"

namespace Player
{
    void PlayerController::Update(float dt)
    {
        auto& input = Engine::InputManager::GetInstance();

        if (input.IsKeyDown(DirectX::Keyboard::Keys::Space))
        {
            auto* rigidbody = ownerGameObject->GetComponent<Rp3dRigidbody>();

            rigidbody->AddForce(Vector3(0.0f, 30.0f, 0.0f));
        }
    }
}