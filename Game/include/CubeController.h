#pragma once

#include <EngineSystem.h>

namespace Cube
{
    using namespace Engine;
    class CubeController :public Component
    {
    protected:
        void OnStart() override;
    };
}