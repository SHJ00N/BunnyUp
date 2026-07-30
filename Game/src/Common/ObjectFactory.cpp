#include "Common/ObjectFactory.h"
#include "Enemy/MeleeEnemyBehaviorTree.h"
#include "Enemy/RangedEnemyBehaviorTree.h"
#include "Enemy/BossEnemyBehaviorTree.h"
#include "Enemy/MeleeEnemyController.h"
#include "Enemy/RangedEnemyController.h"
#include "Enemy/BossEnemyController.h"
#include "Common/Health.h"
#include "Enemy/Thron.h"

namespace Game
{
    using namespace Engine;

    void ObjectFactory::Initialize()
    {
        m_registry["Slime"] = [this](GameObject* parent) { return createSlime(parent); };
        m_registry["TurtleShell"] = [this](GameObject* parent) { return createTurtleShell(parent); };
        m_registry["Boss"] = [this](GameObject* parent) { return createBoss(parent); };
        m_registry["Thron"] = [this](GameObject* parent) { return createThron(parent); };
        m_registry["BigThron"] = [this](GameObject* parent) { return createBigThron(parent); };
    }

    GameObject* ObjectFactory::Create(const std::string& name, GameObject* parent)
    {
        auto it = m_registry.find(name);
        if (it != m_registry.end())
            return it->second(parent);

        return nullptr;
    }

    GameObject* ObjectFactory::createSlime(GameObject* parent)
    {
        auto* enemy = parent->scene->CreateChildGameObject<GameObject>(parent, "Slime");
        enemy->SetTag(ObjectTag::Enemy);
        enemy->AddComponent<SkinnedRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Monster_Slime"));

        auto enemyController = enemy->AddComponent<MeleeEnemyController>();
        enemyController->GetMovement()->SetSpeed(12.5f);
        enemyController->GetMeleeAttack()->SetHitBoxCenter(Vector3(0.0f, 4.0f, 6.0f));
        enemyController->GetMeleeAttack()->SetHitBoxSize(Vector3(11.0f, 7.5f, 10.0f));
        enemy->AddComponent<MeleeEnemyBehaviorTree>(enemyController);

        auto enemyRigidbody = enemy->AddComponent<Rp3dRigidbody>(1.0f, BodyType::DYNAMIC, true);
        enemyRigidbody->SetAngularLock(false, false, false); // Lock angular rotation
        auto enemyCollider = enemy->AddComponent<Rp3dCapsuleCollider>(5.0f, 0.5f);
        enemyCollider->SetLocalPosition(Vector3(0.0f, 5.5f, 0.0f));
        enemyCollider->SetFriction(1.0f);
        enemyCollider->SetBounciness(0.0f);
        enemyCollider->SetCollisionLayer(CollisionLayer::Enemy);

        auto enemyHealth = enemy->AddComponent<Health>();
        enemyHealth->SetMaxHealth(30);

        auto enemyAnimator = enemy->AddComponent<Animator>();
        enemyAnimator->RegistAnimation("Idle", ResourceManager::GetInstance().GetAnimation("Monster_Slime_Idle").get());
        enemyAnimator->RegistAnimation("Walk", ResourceManager::GetInstance().GetAnimation("Monster_Slime_Walk").get());
        enemyAnimator->RegistAnimation("Attack", ResourceManager::GetInstance().GetAnimation("Monster_Slime_Attack").get());
        enemyAnimator->RegistAnimation("Damage", ResourceManager::GetInstance().GetAnimation("Monster_Slime_Damage").get());
        enemyAnimator->RegistAnimation("Death", ResourceManager::GetInstance().GetAnimation("Monster_Slime_Death").get());

        return enemy;
    }

    GameObject* ObjectFactory::createTurtleShell(GameObject* parent)
    {
        auto turtleShell = parent->scene->CreateChildGameObject<GameObject>(parent, "TurtleShell");
        turtleShell->SetTag(ObjectTag::Enemy);
        turtleShell->AddComponent<SkinnedRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Monster_TurtleShell"));

        auto turtleShellController = turtleShell->AddComponent<RangedEnemyController>();
        turtleShellController->GetRangedAttack()->SetAttackRange(150.0f);
        turtleShellController->GetRangedAttack()->SetAttackCoolTime(7.5f);
        turtleShell->AddComponent<RangedEnemyBehaviorTree>(turtleShellController);

        auto turtleShellRigidbody = turtleShell->AddComponent<Rp3dRigidbody>(1.0f, BodyType::DYNAMIC, true);
        turtleShellRigidbody->SetAngularLock(false, false, false); // Lock angular rotation
        auto turtleShellCollider = turtleShell->AddComponent<Rp3dCapsuleCollider>(5.0f, 0.5f);
        turtleShellCollider->SetLocalPosition(Vector3(0.0f, 5.5f, 0.0f));
        turtleShellCollider->SetFriction(1.0f);
        turtleShellCollider->SetBounciness(0.0f);
        turtleShellCollider->SetCollisionLayer(CollisionLayer::Enemy);

        auto turtleShellAnimator = turtleShell->AddComponent<Animator>();
        turtleShellAnimator->RegistAnimation("Idle", ResourceManager::GetInstance().GetAnimation("Monster_TurtleShell_Idle").get());
        turtleShellAnimator->RegistAnimation("Walk", ResourceManager::GetInstance().GetAnimation("Monster_TurtleShell_Walk").get());
        turtleShellAnimator->RegistAnimation("RangedAttack", ResourceManager::GetInstance().GetAnimation("Monster_TurtleShell_Attack2").get());
        turtleShellAnimator->RegistAnimation("Damage", ResourceManager::GetInstance().GetAnimation("Monster_TurtleShell_Damage").get());
        turtleShellAnimator->RegistAnimation("Death", ResourceManager::GetInstance().GetAnimation("Monster_TurtleShell_Death").get());

        auto turtleShellHealth = turtleShell->AddComponent<Health>();
        turtleShellHealth->SetMaxHealth(50);

        return turtleShell;
    }

    GameObject* ObjectFactory::createBoss(GameObject* parent)
    {
        auto boss = parent->scene->CreateChildGameObject<GameObject>(parent, "Boss");
        boss->SetTag(ObjectTag::Enemy);
        auto bossRenderer = boss->AddComponent<SkinnedRenderer>();
        bossRenderer->SetMesh(ResourceManager::GetInstance().GetModel("Monster_TurtleShell"));
        bossRenderer->GetMaterial(0)->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));

        auto bossController = boss->AddComponent<BossEnemyController>();
        bossController->GetMovement()->SetSpeed(5.0f);
        bossController->attackPower = 20;
        bossController->GetMeleeAttack()->SetAttackRange(40.0f);
        bossController->GetMeleeAttack()->SetAttackCoolTime(3.5f);
        bossController->GetMeleeAttack()->SetHitBoxCenter(Vector3(0.0f, 12.0f, 18.0f));
        bossController->GetMeleeAttack()->SetHitBoxSize(Vector3(33.0f, 22.5f, 30.0f));
        bossController->GetRangedAttack()->SetAttackRange(200.0f);
        bossController->GetRangedAttack()->SetAttackCoolTime(10.0f);
        boss->AddComponent<BossEnemyBehaviorTree>(bossController);

        auto bossRigidbody = boss->AddComponent<Rp3dRigidbody>(1.0f, BodyType::DYNAMIC, true);
        bossRigidbody->SetAngularLock(false, false, false); // Lock angular rotation
        auto bossCollider = boss->AddComponent<Rp3dCapsuleCollider>(15.0f, 1.5f);
        bossCollider->SetLocalPosition(Vector3(0.0f, 16.5f, 0.0f));
        bossCollider->SetFriction(1.0f);
        bossCollider->SetBounciness(0.0f);
        bossCollider->SetCollisionLayer(CollisionLayer::Enemy);

        auto bossAnimator = boss->AddComponent<Animator>();
        bossAnimator->RegistAnimation("Idle", ResourceManager::GetInstance().GetAnimation("Monster_TurtleShell_Idle").get());
        bossAnimator->RegistAnimation("Walk", ResourceManager::GetInstance().GetAnimation("Monster_TurtleShell_Walk").get());
        bossAnimator->RegistAnimation("Attack", ResourceManager::GetInstance().GetAnimation("Monster_TurtleShell_Attack1").get());
        bossAnimator->RegistAnimation("RangedAttack", ResourceManager::GetInstance().GetAnimation("Monster_TurtleShell_Attack2").get());
        bossAnimator->RegistAnimation("Damage", ResourceManager::GetInstance().GetAnimation("Monster_TurtleShell_Damage").get());
        bossAnimator->RegistAnimation("Death", ResourceManager::GetInstance().GetAnimation("Monster_TurtleShell_Death").get());

        auto bossHealth = boss->AddComponent<Health>();
        bossHealth->SetMaxHealth(200);

        return boss;
    }

    GameObject* ObjectFactory::createThron(GameObject* parent)
    {
        auto thron = parent->scene->CreateChildGameObject<GameObject>(parent, "Thron");
        thron->SetTag(ObjectTag::Trigger);
        thron->AddComponent<MeshRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Thron"));
        thron->AddComponent<Thron>(10, 50.0f);

        auto thronRigidbody = thron->AddComponent<Rp3dRigidbody>(1.0f, BodyType::KINEMATIC, false);
        auto thronCollider = thron->AddComponent<Rp3dBoxCollider>();
        thronCollider->SetCollisionLayer(CollisionLayer::Trigger);
        thronCollider->SetSize(Vector3(1.0f, 1.0f, 6.0f));
        thronCollider->SetLocalPosition(Vector3(0.0f, 0.0f, 1.0f));
        thronCollider->SetTrigger(true);

        return thron;
    }

    GameObject* ObjectFactory::createBigThron(GameObject* parent)
    {
        auto thron = parent->scene->CreateChildGameObject<GameObject>(parent, "BigThron");
        thron->SetTag(ObjectTag::Trigger);
        thron->AddComponent<MeshRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Thron"));
        thron->AddComponent<Thron>(15, 100.0f);

        auto thronRigidbody = thron->AddComponent<Rp3dRigidbody>(1.0f, BodyType::KINEMATIC, false);
        auto thronCollider = thron->AddComponent<Rp3dBoxCollider>();
        thronCollider->SetCollisionLayer(CollisionLayer::Trigger);
        thronCollider->SetSize(Vector3(3.0f, 3.0f, 18.0f));
        thronCollider->SetLocalPosition(Vector3(0.0f, 0.0f, 3.0f));
        thronCollider->SetTrigger(true);

        return thron;
    }
}