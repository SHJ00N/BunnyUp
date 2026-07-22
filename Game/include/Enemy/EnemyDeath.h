#pragma once

#include <EngineSystem.h>
#include "Enemy/EnemyState.h"

namespace Game
{
    class EnemyController;
    class EnemyDeath : public EnemyState
    {
    public:
        // override function
        void Enter(EnemyController& controller) override;
    protected:
        void OnUpdate(EnemyController& controller, float dt) override;
    };
}