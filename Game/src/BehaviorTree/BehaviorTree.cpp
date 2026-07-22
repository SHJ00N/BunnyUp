#include "BehaviorTree/BehaviorTree.h"

namespace Behavior
{
    void BehaviorTree::Update(float dt)
    {
        if (!m_root) return;

        m_root->Evaluate(dt);
    }

    void BehaviorTree::OnStart()
    {
        m_root = SetupTree();
    }
}