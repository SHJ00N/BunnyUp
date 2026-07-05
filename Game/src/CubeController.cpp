#include "CubeController.h"

namespace Game
{
    void CubeController::OnStart()
    {
        auto* rigidbody = ownerGameObject->GetComponent<Rp3dRigidbody>();

        if (rigidbody)
        {
            Vector3 force(Random::Range(-10.0f, 10.0f), Random::Range(-20.0f, 0.0f), Random::Range(-10.0f, 10.0f));
            rigidbody->AddImpulse(force);
        }
    }
}