#include <EngineSystem.h>

#include "DemoScene1.h"
#include "CameraController.h"
#include "Player/PlayerController.h"
#include "CubeController.h"
#include "Input/PlayerInputManager.h"
#include "Enemy/NavigationManager.h"
#include "Enemy/EnemyBehaviorTree.h"
#include "Enemy/EnemyController.h"
#include "Common/Health.h"
#include "Effect/EffectPoolManager.h"

namespace Game
{
	DemoScene1::DemoScene1()
	{
	}

	DemoScene1::~DemoScene1()
	{
	}

	void DemoScene1::SceneEnter()
	{
        SetEnvironmentMap(ResourceManager::GetInstance().GetEnvironmentMap("Sky_EnvMap"));

        auto navigationManager = CreateGameObject<GameObject>("NavigationManager");
        navigationManager->AddComponent<NavigationManager>(1.0f);

        auto playerInputManager = CreateGameObject<GameObject>("PlayerInputManager");
        playerInputManager->AddComponent<PlayerInputManager>();

        auto effectPoolManager = CreateGameObject<GameObject>("EffectPoolManager");
        effectPoolManager->AddComponent<EffectPoolManager>();

        // Directional Light
        auto directionalLight = CreateGameObject<GameObject>("DirectionalLight");
        auto directionalLightRenderer = directionalLight->AddComponent<MeshRenderer>();
        auto directionalLightComponent = directionalLight->AddComponent<Light>();
        directionalLightComponent->type = LightType::Directional;
        directionalLightComponent->color = Vector4(1.0f, 1.0f, 0.8f, 1.0f);
        directionalLight->transform.SetLocalPosition(Vector3(0.0f, 30.0f, 0.0f));
        directionalLight->transform.SetLocalRotation(Vector3(50.0f, 0.0f, 0.0f));

        // Player
        // -------------------------------------------------------------------------------------------------------
        auto staff = CreateGameObject<GameObject>("Staff");
        staff->AddComponent<MeshRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Chibi_Candy_Staff"));
        auto earmuff = CreateGameObject<GameObject>("Earmuff");
        earmuff->AddComponent<MeshRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Chibi_Earmuff"));
        auto scarf = CreateGameObject<GameObject>("Scarf");
        scarf->AddComponent<MeshRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Chibi_Scarf"));

		auto bunny = CreateGameObject<GameObject>("Bunny");
        bunny->SetTag(ObjectTag::Player);
        bunny->AddComponent<Game::PlayerController>();
        bunny->transform.SetLocalPosition(Vector3(0.0f, 0.0f, -50.0f));
		bunny->transform.SetLocalScale(Vector3(0.1f, 0.1f, 0.1f));
		bunny->transform.SetLocalRotation(Vector3(0.0f, 180.0f, 0.0f));
		bunny->AddComponent<SkinnedRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Chibi_Rabbit"));

        auto bunnyRigidbody = bunny->AddComponent<Rp3dRigidbody>(1.0f, BodyType::DYNAMIC, true);
        bunnyRigidbody->SetAngularLock(false, false, false); //  Lock angular rotation
        auto bunnyCollider = bunny->AddComponent<Rp3dCapsuleCollider>(4.5f, 6.0f);
        bunnyCollider->SetLocalPosition(Vector3(0.0f, 7.6f, 0.0f));
        bunnyCollider->SetFriction(1.0f);
        bunnyCollider->SetBounciness(0.0f);
        bunnyCollider->SetCollisionLayer(CollisionLayer::Player);

		auto animator = bunny->AddComponent<Animator>();
		animator->RegistAnimation("IdleA", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_IdleA").get());
        animator->RegistAnimation("IdleB", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_IdleB").get());
		animator->RegistAnimation("IdleC", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_IdleC").get());
        animator->RegistAnimation("Idle03", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_Idle03").get());
        animator->RegistAnimation("Walk", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_Walk").get());
		animator->RegistAnimation("Run", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_Run").get());
        animator->RegistAnimation("Jump", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_Jump").get());
        animator->RegistAnimation("Jump_Place", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_Jump_Place").get());
        animator->RegistAnimation("Fall", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_Fall").get());
        animator->RegistAnimation("Dash", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_Dash").get());
        animator->RegistAnimation("Attack", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_Attack").get());
        animator->RegistAnimation("Damage", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_Damage").get());
        animator->RegistAnimation("DeathA", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_DeathA").get());
        animator->RegistAnimation("DeathB", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_DeathB").get());
        animator->RegistAnimation("DeathC", ResourceManager::GetInstance().GetAnimation("Chibi_Rabbit_DeathC").get());

        auto bunnySocket = bunny->AddComponent<SocketComponent>();
        bunnySocket->Attach("Weapon", 22, staff, Vector3(0.0f), Vector3(0.0f, 0.0f, 2.5f), Vector3(1.0f));
        bunnySocket->Attach("Helmet", 42, earmuff, Vector3(0.0f), Vector3(1.7f, -1.45f, -3.0f), Vector3(1.0f));
        bunnySocket->Attach("Cape", 10, scarf, Vector3(3.0f, 0.0f, 0.0f), Vector3(4.75f, 0.0f, -1.5f), Vector3(1.0f));

        auto bunnyHealth = bunny->AddComponent<Health>();
        bunnyHealth->SetMaxHealth(100);
        
        // Camera
        // -------------------------------------------------------------------------------------------------------
		auto camera2 = CreateGameObject<GameObject>("MainCamera");
		camera2->transform.SetLocalPosition(Vector3(0.0f, 0.0f, 0.0f));
        camera2->transform.SetLocalScale(Vector3(2.0f));
		auto cameraComponent = camera2->AddComponent<Camera>();
		auto camera2Controller = camera2->AddComponent<CameraController>();
        camera2Controller->SetTarget(bunny);
        camera2Controller->SetCameraOffset(Vector3(0.0f, 40.0f, -60.0f));
        camera2Controller->SetTargetOffset(Vector3(0.0f, 20.0f, 0.0f));
        auto camera2Renderer = camera2->AddComponent<MeshRenderer>();
        camera2Renderer->SetMesh(ResourceManager::GetInstance().GetMesh("Primitive_cube"));

        SetMainCamera(cameraComponent);


        // Monster
        // -------------------------------------------------------------------------------------------------------
        //auto slime = CreateGameObject<GameObject>("Slime");
        //slime->transform.SetLocalScale(Vector3(0.1f));
        //slime->AddComponent<SkinnedRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Monster_Slime"));

        //auto slimeAnimator = slime->AddComponent<Animator>();
        //slimeAnimator->RegistAnimation("Idle", ResourceManager::GetInstance().GetAnimation("Monster_Slime_Idle").get());

        auto turtleShell = CreateGameObject<GameObject>("TurtleShell");
        turtleShell->SetTag(ObjectTag::Enemy);
        turtleShell->transform.SetLocalPosition(Vector3(-50.0f, -45.0f, 50.0f));
        turtleShell->transform.SetLocalScale(Vector3(0.1f));
        turtleShell->AddComponent<SkinnedRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Monster_TurtleShell"));
        
        auto turtleShellController = turtleShell->AddComponent<EnemyController>();
        turtleShellController->SetTarget(bunny);
        turtleShell->AddComponent<EnemyBehaviorTree>(turtleShellController);

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
        turtleShellAnimator->RegistAnimation("Attack", ResourceManager::GetInstance().GetAnimation("Monster_TurtleShell_Attack").get());
        turtleShellAnimator->RegistAnimation("Damage", ResourceManager::GetInstance().GetAnimation("Monster_TurtleShell_Damage").get()); 
        turtleShellAnimator->RegistAnimation("Death", ResourceManager::GetInstance().GetAnimation("Monster_TurtleShell_Death").get());

        auto turtleShellHealth = turtleShell->AddComponent<Health>();
        turtleShellHealth->SetMaxHealth(50);

        // Floor
        // -------------------------------------------------------------------------------------------------------
        auto floor = CreateGameObject<GameObject>("Floor");
        floor->SetTag(ObjectTag::Ground);
        floor->transform.SetLocalRotation(Vector3(90.0f, 0.0f, 0.0f));
        floor->transform.SetLocalScale(Vector3(1500.0f, 1500.0f, 1.0f));
        floor->transform.SetLocalPosition(Vector3(0.0f, -50.0f, 0.0f));

        auto floorRenderer = floor->AddComponent<MeshRenderer>();
        floorRenderer->SetMesh(ResourceManager::GetInstance().GetMesh("Primitive_quad"));
        floorRenderer->SetMaterial(0, ResourceManager::GetInstance().GetMaterial("Textured_material"));
        auto floorMat = floorRenderer->GetMaterial(0);
        floorMat->SetTexture(0, ResourceManager::GetInstance().GetTexture("Snow_Floor_Color"));
        floorMat->SetTexture(1, ResourceManager::GetInstance().GetTexture("Snow_Floor_Normal"));
        floorMat->SetTexture(2, ResourceManager::GetInstance().GetTexture("Snow_Floor_Roughness"));
        floorMat->SetTexture(4, ResourceManager::GetInstance().GetTexture("Snow_Floor_Ambient_Occlusion"));
        floorMat->SetHasNormalMap(true);
        floorMat->SetHasRoughnessMap(true);
        floorMat->SetHasAOMap(true);

        auto floorRigidbody = floor->AddComponent<Rp3dRigidbody>(1000.0f, BodyType::STATIC, false);
        auto floorCollider = floor->AddComponent<Rp3dBoxCollider>();
        floorCollider->SetLocalPosition(Vector3(0.0f, 0.0f, 4.9f));
        floorCollider->SetSize(Vector3(1500.0f, 1500.0f, 10.0f));
        floorCollider->SetCollisionLayer(CollisionLayer::Ground);
        floorCollider->SetBounciness(0.0f);

        // Wall
        // -------------------------------------------------------------------------------------------------------
        // Front
        auto wallFront = CreateGameObject<GameObject>("WallFront");
        wallFront->SetTag(ObjectTag::Wall);
        wallFront->transform.SetLocalPosition(Vector3(0.0f, 0.0f, 700.0f));
        wallFront->AddComponent<Rp3dRigidbody>(1000.0f, BodyType::STATIC, false);
        auto wallFrontCollider = wallFront->AddComponent<Rp3dBoxCollider>();
        wallFrontCollider->SetSize(Vector3(1500.0f, 250.0f, 300.0f));
        wallFrontCollider->SetCollisionLayer(CollisionLayer::Wall);
        wallFrontCollider->SetFriction(0.0f);
        RegistObstacleCollider(wallFrontCollider);
        
        auto frontRock1 = wallFront->CreateGameObject<GameObject>("frontRock1Collider");
        frontRock1->transform.SetLocalPosition(Vector3(-180.0f, 33.0f, -195.0f));
        frontRock1->transform.SetLocalRotation(Vector3(0.0f, 45.0f, -0.0f));
        frontRock1->AddComponent<Rp3dRigidbody>(1000.0f, BodyType::STATIC, false);
        auto frontRockCollider1 = frontRock1->AddComponent<Rp3dBoxCollider>();
        frontRockCollider1->SetSize(Vector3(60.0f, 75.0f, 60.0f));
        frontRockCollider1->SetCollisionLayer(CollisionLayer::Object);
        frontRockCollider1->SetFriction(0.0f);
        // RegistObstacleCollider(frontRockCollider1);

        auto frontRock2 = wallFront->CreateGameObject<GameObject>("frontRock2Collider");
        frontRock2->transform.SetLocalPosition(Vector3(110.0f, -33.0f, -220.0f));
        frontRock2->transform.SetLocalRotation(Vector3(0.0f, 45.0f, -0.0f));
        frontRock2->AddComponent<Rp3dRigidbody>(1000.0f, BodyType::STATIC, false);
        auto frontRockCollider2 = frontRock2->AddComponent<Rp3dBoxCollider>();
        frontRockCollider2->SetSize(Vector3(55.0f, 50.0f, 55.0f));
        frontRockCollider2->SetCollisionLayer(CollisionLayer::Object);
        frontRockCollider2->SetFriction(0.0f);
        RegistObstacleCollider(frontRockCollider2);

        auto frontRock3 = wallFront->CreateGameObject<GameObject>("frontRock3Collider");
        frontRock3->transform.SetLocalPosition(Vector3(200.0f, -7.5f, -215.0f));
        frontRock3->transform.SetLocalRotation(Vector3(0.0f, 60.0f, -0.0f));
        frontRock3->AddComponent<Rp3dRigidbody>(1000.0f, BodyType::STATIC, false);
        auto frontRockCollider3 = frontRock3->AddComponent<Rp3dBoxCollider>();
        frontRockCollider3->SetSize(Vector3(55.0f, 85.0f, 65.0f));
        frontRockCollider3->SetCollisionLayer(CollisionLayer::Object);
        frontRockCollider3->SetFriction(0.0f);
        RegistObstacleCollider(frontRockCollider3);

        auto frontWall1 = wallFront->CreateGameObject<GameObject>("FrontWall1");
        frontWall1->transform.SetLocalPosition(Vector3(-450.0f, 300.0f, 50.0f));
        frontWall1->transform.SetLocalRotation(Vector3(0.0f, 45.0f, 0.0f));
        frontWall1->transform.SetLocalScale(Vector3(0.7f));
        frontWall1->AddComponent<MeshRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Winter_Floating_Islend"));
        auto frontWall2 = wallFront->CreateGameObject<GameObject>("FrontWall2");
        frontWall2->transform.SetLocalPosition(Vector3(0.0f, 200.0f, 200.0f));
        frontWall2->transform.SetLocalScale(Vector3(0.7f));
        frontWall2->AddComponent<MeshRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Winter_Floating_Islend"));
        auto frontWall3 = wallFront->CreateGameObject<GameObject>("FrontWall3");
        frontWall3->transform.SetLocalPosition(Vector3(450.0f, 250.0f, 60.0f));
        frontWall1->transform.SetLocalRotation(Vector3(0.0f, -90.0f, 0.0f));
        frontWall3->transform.SetLocalScale(Vector3(0.7f));
        frontWall3->AddComponent<MeshRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Winter_Floating_Islend"));

        // Back
        auto wallBack = CreateGameObject<GameObject>("WallBack");
        wallBack->SetTag(ObjectTag::Wall);
        wallBack->transform.SetLocalPosition(Vector3(0.0f, 0.0f, -700.0f));
        wallBack->transform.SetLocalRotation(Vector3(0.0f, 180.0f, 0.0f));
        wallBack->AddComponent<Rp3dRigidbody>(1000.0f, BodyType::STATIC, false);
        auto wallBackCollider = wallBack->AddComponent<Rp3dBoxCollider>();
        wallBackCollider->SetSize(Vector3(1500.0f, 250.0f, 300.0f));
        wallBackCollider->SetCollisionLayer(CollisionLayer::Wall);
        wallBackCollider->SetFriction(0.0f);
        RegistObstacleCollider(wallBackCollider);

        auto backWall1 = wallBack->CreateGameObject<GameObject>("BackWall1");
        backWall1->transform.SetLocalPosition(Vector3(-510.0f, 120.0f, 0.0f));
        backWall1->transform.SetLocalRotation(Vector3(0.0f, -130.0f, 0.0f));
        backWall1->transform.SetLocalScale(Vector3(0.5f));
        backWall1->AddComponent<MeshRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Winter_Floating_Islend"));
        auto backWall2 = wallBack->CreateGameObject<GameObject>("BackWall2");
        backWall2->transform.SetLocalPosition(Vector3(-70.0f, 170.0f, 130.0f));
        backWall2->transform.SetLocalRotation(Vector3(0.0f, 90.0f, 0.0f));
        backWall2->transform.SetLocalScale(Vector3(0.7f));
        backWall2->AddComponent<MeshRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Winter_Floating_Islend"));
        auto backWall3 = wallBack->CreateGameObject<GameObject>("BackWall3");
        backWall3->transform.SetLocalPosition(Vector3(430.0f, 200.0f, 60.0f));
        backWall3->transform.SetLocalRotation(Vector3(0.0f, 95.0f, 0.0f));
        backWall3->transform.SetLocalScale(Vector3(0.6f));
        backWall3->AddComponent<MeshRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Winter_Floating_Islend"));

        // Right
        auto wallRight = CreateGameObject<GameObject>("WallRight");
        wallRight->SetTag(ObjectTag::Wall);
        wallRight->transform.SetLocalPosition(Vector3(700.0f, 0.0f, 0.0f));
        wallRight->transform.SetLocalRotation(Vector3(0.0f, 90.0f, 0.0f));
        wallRight->AddComponent<Rp3dRigidbody>(1000.0f, BodyType::STATIC, false);
        auto wallRightCollider = wallRight->AddComponent<Rp3dBoxCollider>();
        wallRightCollider->SetSize(Vector3(1500.0f, 250.0f, 300.0f));
        wallRightCollider->SetCollisionLayer(CollisionLayer::Wall);
        wallRightCollider->SetFriction(0.0f);
        RegistObstacleCollider(wallRightCollider);

        auto rightRock1 = wallRight->CreateGameObject<GameObject>("rightRock1Collider");
        rightRock1->transform.SetLocalPosition(Vector3(-80.0f, -35.0f, -220.0f));
        rightRock1->transform.SetLocalRotation(Vector3(0.0f, 25.0f, -0.0f));
        rightRock1->AddComponent<Rp3dRigidbody>(1000.0f, BodyType::STATIC, false);
        auto rightRockCollider1 = rightRock1->AddComponent<Rp3dBoxCollider>();
        rightRockCollider1->SetSize(Vector3(50.0f, 55.0f, 50.0f));
        rightRockCollider1->SetCollisionLayer(CollisionLayer::Object);
        rightRockCollider1->SetFriction(0.0f);
        RegistObstacleCollider(rightRockCollider1);

        auto rightRock2 = wallRight->CreateGameObject<GameObject>("rightRock2Collider");
        rightRock2->transform.SetLocalPosition(Vector3(295.0f, -27.0f, -160.0f));
        rightRock2->transform.SetLocalRotation(Vector3(0.0f, 0.0f, -0.0f));
        rightRock2->AddComponent<Rp3dRigidbody>(1000.0f, BodyType::STATIC, false);
        auto rightRockCollider2 = rightRock2->AddComponent<Rp3dBoxCollider>();
        rightRockCollider2->SetSize(Vector3(58.0f, 50.0f, 55.0f));
        rightRockCollider2->SetCollisionLayer(CollisionLayer::Object);
        rightRockCollider2->SetFriction(0.0f);
        RegistObstacleCollider(rightRockCollider2);

        auto rightRock3 = wallRight->CreateGameObject<GameObject>("rightRock3Collider");
        rightRock3->transform.SetLocalPosition(Vector3(460.0f, -41.5f, -185.5f));
        rightRock3->transform.SetLocalRotation(Vector3(0.0f, 75.0f, -0.0f));
        rightRock3->AddComponent<Rp3dRigidbody>(1000.0f, BodyType::STATIC, false);
        auto rightRockCollider3 = rightRock3->AddComponent<Rp3dBoxCollider>();
        rightRockCollider3->SetSize(Vector3(40.0f, 35.0f, 47.0f));
        rightRockCollider3->SetCollisionLayer(CollisionLayer::Object);
        rightRockCollider3->SetFriction(0.0f);
        RegistObstacleCollider(rightRockCollider3);

        auto rightWall1 = wallRight->CreateGameObject<GameObject>("RightWall1");
        rightWall1->transform.SetLocalPosition(Vector3(-435.0f, 300.0f, 60.0f));
        rightWall1->transform.SetLocalRotation(Vector3(0.0f, 45.0f, 0.0f));
        rightWall1->transform.SetLocalScale(Vector3(0.7f));
        rightWall1->AddComponent<MeshRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Winter_Floating_Islend"));
        auto rightWall2 = wallRight->CreateGameObject<GameObject>("RightWall2");
        rightWall2->transform.SetLocalPosition(Vector3(-10.0f, 200.0f, 150.0f));
        rightWall2->transform.SetLocalRotation(Vector3(0.0f, -30.0f, 0.0f));
        rightWall2->transform.SetLocalScale(Vector3(0.7f));
        rightWall2->AddComponent<MeshRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Winter_Floating_Islend"));
        auto rightWall3 = wallRight->CreateGameObject<GameObject>("RightWall3");
        rightWall3->transform.SetLocalPosition(Vector3(400.0f, 150.0f, 45.0f));
        rightWall3->transform.SetLocalRotation(Vector3(0.0f, 100.0f, 0.0f));
        rightWall3->transform.SetLocalScale(Vector3(0.5f));
        rightWall3->AddComponent<MeshRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Winter_Floating_Islend"));

        // Left
        auto wallLeft = CreateGameObject<GameObject>("WallLeft");
        wallLeft->SetTag(ObjectTag::Wall);
        wallLeft->transform.SetLocalPosition(Vector3(-700.0f, 0.0f, 0.0f));
        wallLeft->transform.SetLocalRotation(Vector3(0.0f, -90.0f, 0.0f));
        wallLeft->AddComponent<Rp3dRigidbody>(1000.0f, BodyType::STATIC, false);
        auto wallLeftCollider = wallLeft->AddComponent<Rp3dBoxCollider>();
        wallLeftCollider->SetSize(Vector3(1500.0f, 250.0f, 300.0f));
        wallLeftCollider->SetCollisionLayer(CollisionLayer::Wall);
        wallLeftCollider->SetFriction(0.0f);
        RegistObstacleCollider(wallLeftCollider);

        auto leftRock1 = wallLeft->CreateGameObject<GameObject>("leftRock1Collider");
        leftRock1->transform.SetLocalPosition(Vector3(-85.0f, -35.0f, -215.0f));
        leftRock1->transform.SetLocalRotation(Vector3(0.0f, 20.0f, -0.0f));
        leftRock1->AddComponent<Rp3dRigidbody>(1000.0f, BodyType::STATIC, false);
        auto leftLockCollider1 = leftRock1->AddComponent<Rp3dBoxCollider>();
        leftLockCollider1->SetSize(Vector3(55.0f, 50.0f, 55.0f));
        leftLockCollider1->SetCollisionLayer(CollisionLayer::Object);
        leftLockCollider1->SetFriction(0.0f);
        RegistObstacleCollider(leftLockCollider1);

        auto leftRock2 = wallLeft->CreateGameObject<GameObject>("leftRock2Collider");
        leftRock2->transform.SetLocalPosition(Vector3(300.0f, -33.0f, -160.0f));
        leftRock2->transform.SetLocalRotation(Vector3(0.0f, 20.0f, -0.0f));
        leftRock2->AddComponent<Rp3dRigidbody>(1000.0f, BodyType::STATIC, false);
        auto leftRockCollider2 = leftRock2->AddComponent<Rp3dBoxCollider>();
        leftRockCollider2->SetSize(Vector3(55.0f, 50.0f, 55.0f));
        leftRockCollider2->SetCollisionLayer(CollisionLayer::Object);
        leftRockCollider2->SetFriction(0.0f);
        RegistObstacleCollider(leftRockCollider2);

        auto leftWall1 = wallLeft->CreateGameObject<GameObject>("LeftWall1");
        leftWall1->transform.SetLocalPosition(Vector3(-435.0f, 300.0f, 140.0f));
        leftWall1->transform.SetLocalRotation(Vector3(0.0f, 45.0f, 0.0f));
        leftWall1->transform.SetLocalScale(Vector3(0.7f));
        leftWall1->AddComponent<MeshRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Winter_Floating_Islend"));
        auto leftWall2 = wallLeft->CreateGameObject<GameObject>("LeftWall2");
        leftWall2->transform.SetLocalPosition(Vector3(-10.0f, 200.0f, 150.0f));
        leftWall2->transform.SetLocalRotation(Vector3(0.0f, -30.0f, 0.0f));
        leftWall2->transform.SetLocalScale(Vector3(0.7f));
        leftWall2->AddComponent<MeshRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Winter_Floating_Islend"));
        auto leftWall3 = wallLeft->CreateGameObject<GameObject>("LeftWall3");
        leftWall3->transform.SetLocalPosition(Vector3(400.0f, 150.0f, 60.0f));
        leftWall3->transform.SetLocalRotation(Vector3(0.0f, 100.0f, 0.0f));
        leftWall3->transform.SetLocalScale(Vector3(0.5f));
        leftWall3->AddComponent<MeshRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Winter_Floating_Islend"));

        // Environments
        // -------------------------------------------------------------------------------------------------------
        // Trees
        auto treesA = CreateGameObject<GameObject>("TreesA");
        treesA->transform.SetLocalPosition(Vector3(-350.0f, -50.0f, 0.0f));
        createTree(treesA, "treeA1", Vector3(-10.0f, 0.0f, -30.0f), Vector3(0.3f));
        createTree(treesA, "treeA2", Vector3(-100.0f, 0.0f, 80.0f), Vector3(0.3f));
        createTree(treesA, "treeA3", Vector3(-180.0f, 0.0f, -200.0f), Vector3(0.3f));
        createTree(treesA, "treeA4", Vector3(-75.0f, 0.0f, -300.0f), Vector3(0.3f));
        createTree(treesA, "treeA5", Vector3(30.0f, 0.0f, -160.0f), Vector3(0.3f));
        createTree(treesA, "treeA6", Vector3(-60.0f, 0.0f, -430.0f), Vector3(0.3f));
        createTree(treesA, "treeA7", Vector3(-180.0f, 0.0f, 175.0f), Vector3(0.3f));
        createTree(treesA, "treeA8", Vector3(-20.0f, 0.0f, 320.0f), Vector3(0.3f));
        createTree(treesA, "treeA9", Vector3(8.0f, 0.0f, 145.0f), Vector3(0.3f));

        auto treesB = CreateGameObject<GameObject>("TreesB");
        treesB->transform.SetLocalPosition(Vector3(350.0f, -50.0f, 0.0f));
        createTree(treesB, "treeB1", Vector3(70.0f, 0.0f, -30.0f), Vector3(0.3f));
        createTree(treesB, "treeB2", Vector3(-20.0f, 0.0f, -140.0f), Vector3(0.3f));
        createTree(treesB, "treeB3", Vector3(65.0f, 0.0f, -350.0f), Vector3(0.3f));
        createTree(treesB, "treeB4", Vector3(115.0f, 0.0f, -200.0f), Vector3(0.3f));
        createTree(treesB, "treeB5", Vector3(-40.0f, 0.0f, -450.0f), Vector3(0.3f));
        createTree(treesB, "treeB6", Vector3(-150.0f, 0.0f, 350.0f), Vector3(0.3f));
        createTree(treesB, "treeB7", Vector3(-10.0f, 0.0f, 385.0f), Vector3(0.3f));
        createTree(treesB, "treeB8", Vector3(120.0f, 0.0f, 180.0f), Vector3(0.3f));

        auto treesC = CreateGameObject<GameObject>("TreesC");
        treesC->transform.SetLocalPosition(Vector3(0.0f, -50.0f, 350.0f));
        createTree(treesC, "treeC1", Vector3(70.0f, 0.0f, -15.0f), Vector3(0.3f));
        createTree(treesC, "treeC2", Vector3(-40.0f, 0.0f, 135.0f), Vector3(0.3f));
        createTree(treesC, "treeC3", Vector3(-250.0f, 0.0f, 30.0f), Vector3(0.3f));
        createTree(treesC, "treeC4", Vector3(-110.0f, 0.0f, -10.0f), Vector3(0.3f));

        auto treesD = CreateGameObject<GameObject>("TreesD");
        treesD->transform.SetLocalPosition(Vector3(0.0f, -50.0f, -350.0f));
        createTree(treesD, "treeD1", Vector3(130.0f, 0.0f, -50.0f), Vector3(0.3f));
        createTree(treesD, "treeD2", Vector3(5.0f, 0.0f, 20.0f), Vector3(0.3f));
        createTree(treesD, "treeD3", Vector3(-220.0f, 0.0f, 60.0f), Vector3(0.3f));
        createTree(treesD, "treeD4", Vector3(-130.0f, 0.0f, -80.0f), Vector3(0.3f));

        // Objects
        auto starTree = CreateGameObject<GameObject>("StarTree");
        starTree->transform.SetLocalPosition(Vector3(0.0f, -50.0f, 0.0f));
        starTree->transform.SetLocalRotation(Vector3(0.0f, 0.0f, 0.0f));
        starTree->transform.SetLocalScale(Vector3(0.2f));
        starTree->AddComponent<MeshRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Chibi_Tree_C"));
        starTree->AddComponent<Rp3dRigidbody>(1000.0f, BodyType::STATIC, false);
        auto starTreeCollider1 = starTree->AddComponent<Rp3dBoxCollider>();
        starTreeCollider1->SetCollisionLayer(CollisionLayer::Object);
        starTreeCollider1->SetLocalPosition(Vector3(0.0f, 15.0f, 0.0f));
        starTreeCollider1->SetSize(Vector3(10.0f, 30.0f, 10.0f));
        starTreeCollider1->SetFriction(0.0f);
        RegistObstacleCollider(starTreeCollider1);
        auto starTreeCollider2 = starTree->AddComponent<Rp3dBoxCollider>();
        starTreeCollider2->SetCollisionLayer(CollisionLayer::Object);
        starTreeCollider2->SetLocalPosition(Vector3(0.0f, 65.0f, 0.0f));
        starTreeCollider2->SetSize(Vector3(50.0f, 70.0f, 50.0f));
        starTreeCollider2->SetFriction(0.0f);
        // RegistObstacleCollider(starTreeCollider2);
        auto starLight = starTree->CreateGameObject<GameObject>("PointLight");
        starLight->transform.SetLocalPosition(Vector3(0.0f, 550.0f, 0.0f));
        starLight->AddComponent<MeshRenderer>()->SetMesh(ResourceManager::GetInstance().GetMesh("Primitive_cube"));
        auto starPointLight = starLight->AddComponent<Light>();
        starPointLight->type = LightType::Point;
        starPointLight->color = Vector4(100.0f, 100.0f, 0.0f, 100.0f);

        auto gifts = CreateGameObject<GameObject>("Gifts");
        gifts->transform.SetLocalPosition(Vector3(0.0f, -42.0f, 0.0f));
        createGift(gifts, "GiftA", 0, Vector3(-80.0f, 0.0f, -30.0f), Vector3(0.0f, 30.0f, 0.0f), Vector3(0.3f));
        createGift(gifts, "GiftB", 1, Vector3(-10.0f, 0.0f, -135.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.3f));
        createGift(gifts, "GiftC", 2, Vector3(45.0f, 0.0f, -145.0f), Vector3(0.0f, 90.0f, 0.0f), Vector3(0.3f));
        createGift(gifts, "GiftD", 0, Vector3(-180.0f, 0.0f, 65.0f), Vector3(0.0f, -10.0f, 0.0f), Vector3(0.3f));
        createGift(gifts, "GiftE", 1, Vector3(140.0f, 0.0f, -30.0f), Vector3(0.0f, -20.0f, 0.0f), Vector3(0.3f));

        auto bigGifts = CreateGameObject<GameObject>("BigGifts");
        bigGifts->transform.SetLocalPosition(Vector3(45.0f, -37.0f, 100.0f));
        auto giftA = bigGifts->CreateGameObject<GameObject>("GiftA");
        giftA->SetTag(ObjectTag::Object);
        giftA->transform.SetLocalPosition(Vector3(0.0f, 0.0f, 0.0f));
        giftA->transform.SetLocalRotation(Vector3(0.0f, 10.0f, 0.0f));
        giftA->transform.SetLocalScale(Vector3(0.5f));
        giftA->AddComponent<MeshRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Chibi_Gift_A"));
        giftA->AddComponent<Rp3dRigidbody>(10.0f, BodyType::STATIC, false);
        auto giftACollider = giftA->AddComponent<Rp3dBoxCollider>();
        giftACollider->SetCollisionLayer(CollisionLayer::Object);
        giftACollider->SetLocalPosition(Vector3(0.0f, -4.0f, 0.0f));
        giftACollider->SetSize(Vector3(22.0f, 20.5f, 22.0f));
        giftACollider->SetFriction(0.0f);
        RegistObstacleCollider(giftACollider);

        auto giftB = bigGifts->CreateGameObject<GameObject>("GiftB");
        giftB->SetTag(ObjectTag::Object);
        giftB->transform.SetLocalPosition(Vector3(26.0f, 0.0f, 5.0f));
        giftB->transform.SetLocalRotation(Vector3(0.0f, -5.0f, 0.0f));
        giftB->transform.SetLocalScale(Vector3(0.5f));
        giftB->AddComponent<MeshRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Chibi_Gift_B"));
        giftB->AddComponent<Rp3dRigidbody>(10.0f, BodyType::STATIC, false);
        auto giftBCollider = giftB->AddComponent<Rp3dBoxCollider>();
        giftBCollider->SetCollisionLayer(CollisionLayer::Object);
        giftBCollider->SetLocalPosition(Vector3(0.0f, -4.0f, 0.0f));
        giftBCollider->SetSize(Vector3(22.0f, 20.5f, 22.0f));
        giftBCollider->SetFriction(0.0f);
        RegistObstacleCollider(giftBCollider);

        createGift(bigGifts, "GiftC", 0, Vector3(18.0f, -5.0f, -15.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.3f));
        createGift(bigGifts, "GiftD", 2, Vector3(14.0f, 15.5f, 5.0f), Vector3(0.0f, -10.0f, 0.0f), Vector3(0.3f));
	}

    void DemoScene1::createTree(GameObject* parent, const std::string& name, const Vector3& pos, const Vector3& scale)
    {
        auto tree = parent->CreateGameObject<GameObject>(name);
        tree->transform.SetLocalPosition(pos);
        tree->transform.SetLocalScale(scale);
        tree->AddComponent<MeshRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Chibi_Tree_B"));
        tree->AddComponent<Rp3dRigidbody>(1000.0f, BodyType::STATIC, false);
        auto treeCollider1 = tree->AddComponent<Rp3dBoxCollider>();
        treeCollider1->SetCollisionLayer(CollisionLayer::Wall);
        treeCollider1->SetFriction(0.0f);
        treeCollider1->SetLocalPosition(Vector3(0.0f, 20.0f, 0.0f));
        treeCollider1->SetSize(Vector3(15.0f, 40.0f, 15.0f));
        RegistObstacleCollider(treeCollider1);
        auto treeCollider2 = tree->AddComponent<Rp3dBoxCollider>();
        treeCollider2->SetCollisionLayer(CollisionLayer::Wall);
        treeCollider2->SetFriction(0.0f);
        treeCollider2->SetLocalPosition(Vector3(0.0f, 90.0f, 0.0f));
        treeCollider2->SetSize(Vector3(85.0f, 100.0f, 85.0f));
        // RegistObstacleCollider(treeCollider2);
    }

    void DemoScene1::createGift(GameObject* parent, const std::string& name, int type, const Vector3& pos, const Vector3& rot, const Vector3& scale)
    {
        auto gift = parent->CreateGameObject<GameObject>(name);
        gift->SetTag(ObjectTag::Object);
        gift->transform.SetLocalPosition(pos);
        gift->transform.SetLocalRotation(rot);
        gift->transform.SetLocalScale(scale);
        
        std::shared_ptr<Model> model;
        switch (type)
        {
        case 0: model = ResourceManager::GetInstance().GetModel("Chibi_Gift_A"); break;
        case 1: model = ResourceManager::GetInstance().GetModel("Chibi_Gift_B"); break;
        case 2: model = ResourceManager::GetInstance().GetModel("Chibi_Gift_C"); break;
        default: model = ResourceManager::GetInstance().GetModel("Chibi_Gift_A"); break;
        }
        gift->AddComponent<MeshRenderer>()->SetMesh(model);
        gift->AddComponent<Rp3dRigidbody>(10.0f, BodyType::STATIC, false);
        auto giftCollider = gift->AddComponent<Rp3dBoxCollider>();
        giftCollider->SetCollisionLayer(CollisionLayer::Object);
        giftCollider->SetFriction(0.0f);
        giftCollider->SetLocalPosition(Vector3(0.0f, -1.5f, 0.0f));
        giftCollider->SetSize(Vector3(12.5f, 12.5f, 12.5f));
        RegistObstacleCollider(giftCollider);
    }
}