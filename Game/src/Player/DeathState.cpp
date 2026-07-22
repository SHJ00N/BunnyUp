#include "Player/DeathState.h"
#include "Player/PlayerController.h"

namespace Game
{
    bool DeathState::HandleInput(PlayerController& playerController, const PlayerInputState& inputState)
    {
        return false;
    }

    void DeathState::Enter(PlayerController& playerController)
    {
        playerController.isInvincible = true;
        playerController.GetAnimator()->PlayAnimation("DeathA");
    }

    void DeathState::Exit(PlayerController& playerController)
    {
        playerController.isInvincible = false;
    }
}