#include "Player/FallingState.h"
#include "Player/PlayerController.h"
#include "Player/MovingState.h"
#include "Input/PlayerInputManager.h"

namespace Game
{
    bool FallingState::HandleInput(PlayerController& playerController, const PlayerInputState& inputState)
    {
        if (IsGround(playerController, inputState)) return true;
        if (CanAttack(playerController, inputState)) return true;

        return false;
    }

    void FallingState::Enter(PlayerController& playerController)
    {
        LOG_INFO("Entering Falling State");
        playerController.GetAnimator()->PlayAnimation("Fall");
    }

    void FallingState::Exit(PlayerController& playerController)
    {
        OnAirState::Exit(playerController);
        LOG_INFO("Exiting Falling State");
    }
}