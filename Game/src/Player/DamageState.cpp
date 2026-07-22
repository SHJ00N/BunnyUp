#include "Player/DamageState.h"
#include "Player/PlayerController.h"

namespace Game
{
    bool DamageState::HandleInput(PlayerController& playerController, const PlayerInputState& inputState)
    {
        if (Elapsed(0.5f))
        {
            if(playerController.isGrounded)
            {
                playerController.ChangeState(playerController.GetCombatState());
            }
            else
            {
                playerController.ChangeState(playerController.GetFallingState());
            }
        }

        return false;
    }

    void DamageState::Enter(PlayerController& playerController)
    {
        ResetTimer();
        playerController.isInvincible = true;
        playerController.GetAnimator()->PlayAnimation("Damage", true);
    }

    void DamageState::Exit(PlayerController& playerController)
    {
        playerController.isInvincible = false;
        playerController.hasDamage = false;
    }
}