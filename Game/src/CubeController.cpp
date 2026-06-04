#include "CubeController.h"

namespace Cube
{
    void CubeController::OnTriggerEnter(Collider* other)
    {
        LOG_INFO("CubeController: OnTriggerEnter with %s", other->ownerGameObject->GetName().c_str());
    }
    void CubeController::OnTriggerStay(Collider* other)
    {
        // LOG_INFO("CubeController: OnTriggerStay with %s", other->ownerGameObject->GetName().c_str());
    }
    
    void CubeController::OnTriggerExit(Collider* other)
    {
        LOG_INFO("CubeController: OnTriggerExit with %s", other->ownerGameObject->GetName().c_str());
    }

    void CubeController::OnCollisionEnter(Collider* other)
    {
        LOG_INFO("CubeController: OnCollisionEnter with %s", other->ownerGameObject->GetName().c_str());
    }
    void CubeController::OnCollisionStay(Collider* other)
    {
        // LOG_INFO("CubeController: OnCollisionStay with %s", other->ownerGameObject->GetName().c_str());
    }
    void CubeController::OnCollisionExit(Collider* other)
    {
        LOG_INFO("CubeController: OnCollisionExit with %s", other->ownerGameObject->GetName().c_str());
    }
}