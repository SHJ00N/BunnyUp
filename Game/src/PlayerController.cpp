#include "PlayerController.h"

namespace Player
{
    void PlayerController::OnTriggerEnter(Collider* other)
    {
        LOG_INFO("PlayerController: OnTriggerEnter with %s", other->ownerGameObject->GetName().c_str());
        this->ownerGameObject->Destroy();
    }
    void PlayerController::OnTriggerStay(Collider* other)
    {
        LOG_INFO("PlayerController: OnTriggerStay with %s", other->ownerGameObject->GetName().c_str());
    }
    void PlayerController::OnTriggerExit(Collider* other)
    {
        LOG_INFO("PlayerController: OnTriggerExit with %s", other->ownerGameObject->GetName().c_str());
    }
    void PlayerController::OnCollisionEnter(Collider* other)
    {
        LOG_INFO("PlayerController: OnCollisionEnter with %s", other->ownerGameObject->GetName().c_str());
    }
    void PlayerController::OnCollisionStay(Collider* other)
    {
        LOG_INFO("PlayerController: OnCollisionStay with %s", other->ownerGameObject->GetName().c_str());
    }
    void PlayerController::OnCollisionExit(Collider* other)
    {
        LOG_INFO("PlayerController: OnCollisionExit with %s", other->ownerGameObject->GetName().c_str());
    }
}