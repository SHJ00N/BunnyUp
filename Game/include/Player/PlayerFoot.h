#pragma once

#include <EngineSystem.h>

namespace Game
{
    using namespace Engine;

    class PlayerController;
    class PlayerFoot : public Component
    {
    public:
        void OnTriggerEnter(Rp3dCollider* other) override;
        void OnTriggerExit(Rp3dCollider* other) override;

    protected:
        void OnStart() override;

    private:
        PlayerController* m_controller = nullptr;
    };
}