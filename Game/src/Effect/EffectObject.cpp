#include "Effect/EffectObject.h"
#include "Effect/EffectPoolManager.h"

namespace Game
{
    using namespace Engine;

    EffectObject::EffectObject(EffectObjectType type) : m_type(type)
    {
    }

    void EffectObject::Update(float dt)
    {
        if (!m_effect) return;

        if (!m_effect->IsFinish() && !m_effect->IsPlaying())
        {
            m_effect->Play();
        }

        if (m_effect->IsFinish())
        {
            EffectPoolManager::GetInstance().ReturnPool(this);
            return;
        }
    }

    void EffectObject::OnStart()
    {
        m_effect = ownerGameObject->GetComponent<EffectComponent>();
    }
}