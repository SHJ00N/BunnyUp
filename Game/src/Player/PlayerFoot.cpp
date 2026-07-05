#include "Player/PlayerFoot.h"
#include "Player/PlayerController.h"

namespace Game
{
    void PlayerFoot::OnStart()
    {
        m_controller = ownerGameObject->GetParent()->GetComponent<PlayerController>();
    }
    
    void PlayerFoot::OnTriggerEnter(Rp3dCollider* other)
    {
        if (other->ownerGameObject->GetTag() == ObjectTag::Ground || other->ownerGameObject->GetTag() == ObjectTag::Object)
        {
            LOG_WARNING("ground enter");
            m_controller->isGrounded = true;
        }
    }

    void PlayerFoot::OnTriggerExit(Rp3dCollider* other)
    {
        if (other->ownerGameObject->GetTag() == ObjectTag::Ground || other->ownerGameObject->GetTag() == ObjectTag::Object)
        {
            LOG_WARNING("ground exit");
            m_controller->isGrounded = false;
        }
    }
}