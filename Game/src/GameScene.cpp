#include <EngineSystem.h>

#include "GameScene.h"
#include "Player/PlayerController.h"
#include "Input/PlayerInputManager.h"

namespace Game
{
    GameScene::GameScene()
    {
    }

    GameScene::~GameScene()
    {
    }

    void GameScene::SceneEnter()
    {
        SetEnvironmentMap(ResourceManager::GetInstance().GetEnvironmentMap("Sky_EnvMap"));

        auto playerInputManager = CreateGameObject<GameObject>("PlayerInputManager");
        playerInputManager->AddComponent<PlayerInputManager>();

        // Player
        // -------------------------------------------------------------------------------------------------------
        auto bunny = CreateGameObject<GameObject>("Bunny");
        bunny->SetTag(ObjectTag::Player);
        bunny->AddComponent<Game::PlayerController>();
        bunny->transform.SetLocalPosition(Vector3(-20.0f, 20.0f, 0.0f));
        bunny->transform.SetLocalScale(Vector3(0.1f, 0.1f, 0.1f));
        bunny->transform.SetLocalRotation(Vector3(0.0f, 180.0f, 0.0f));
        bunny->AddComponent<SkinnedRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Chibi_Rabbit"));

        auto bunnyRigidbody = bunny->AddComponent<Rp3dRigidbody>(1.0f, BodyType::DYNAMIC, true);
        bunnyRigidbody->SetAngularLock(false, true, false); // Lock Y-axis rotation
        auto bunnyCollider = bunny->AddComponent<Rp3dCapsuleCollider>(4.5f, 6.0f);
        bunnyCollider->SetLocalPosition(Vector3(0.0f, 7.6f, 0.0f));
        bunnyCollider->SetFriction(1.0f);
        bunnyCollider->SetBounciness(0.0f);
        bunnyCollider->SetCollisionLayer(CollisionLayer::Player);

        auto animator = bunny->AddComponent<Animator>();

        animator->RegistAnimation("IdleA", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_IdleA").get());
        animator->RegistAnimation("IdleC", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_IdleC").get());
        animator->RegistAnimation("Walk", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_Walk").get());
        animator->RegistAnimation("Run", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_Run").get());
        animator->RegistAnimation("Jump", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_Jump").get());
        animator->RegistAnimation("Jump_Place", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_Jump_Place").get());
        animator->RegistAnimation("Fall", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_Fall").get());

        // Map
        // -------------------------------------------------------------------------------------------------------
        auto floor = CreateGameObject<GameObject>("Floor");
        floor->SetTag(ObjectTag::Ground);
        floor->transform.SetLocalRotation(Vector3(00.0f, 0.0f, 0.0f));
        floor->transform.SetLocalScale(Vector3(0.5f, 0.5f, 0.5f));
        floor->transform.SetLocalPosition(Vector3(0.0f, -10.0f, 0.0f));

        auto floorRenderer = floor->AddComponent<MeshRenderer>();
        floorRenderer->SetMesh(ResourceManager::GetInstance().GetModel("Winter_Floating_Islend"));

        auto floorRigidbody = floor->AddComponent<Rp3dRigidbody>(1000.0f, BodyType::STATIC, false);
        auto floorCollider = floor->AddComponent<Rp3dBoxCollider>();
        floorCollider->SetSize(Vector3(600.0f, 10.0f, 600.0f));
        floorCollider->SetCollisionLayer(CollisionLayer::Ground);
        floorCollider->SetBounciness(0.0f);
    }
}