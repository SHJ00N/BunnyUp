#include <EngineSystem.h>

#include "DemoScene1.h"
#include "CameraController.h"
#include "Player/PlayerController.h"
#include "CubeController.h"
#include "Input/PlayerInputManager.h"
#include "Player/PlayerFoot.h"

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

        auto playerInputManager = CreateGameObject<GameObject>("PlayerInputManager");
        playerInputManager->AddComponent<PlayerInputManager>();

        // Player
        // -------------------------------------------------------------------------------------------------------
		auto bunny = CreateGameObject<GameObject>("Bunny");
        bunny->SetTag(ObjectTag::Player);
        bunny->AddComponent<Game::PlayerController>();
        bunny->transform.SetLocalPosition(Vector3(-20.0f, 0.0f, 0.0f));
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

        // Player foot
        auto bunnyFoot = bunny->CreateGameObject<GameObject>("BunnyFoot");
        bunnyFoot->AddComponent<PlayerFoot>();
        bunnyFoot->SetTag(ObjectTag::Player);
        auto footBody = bunnyFoot->AddComponent<Rp3dRigidbody>(1.0f, BodyType::KINEMATIC, false);
        auto footCollider = bunnyFoot->AddComponent<Rp3dCapsuleCollider>(3.0f, 1.0f);
        footCollider->SetLocalRotation(RotationPitchYawRoll(90.0f, 0.0f, 90.0f));
        footCollider->SetLocalPosition(Vector3(0.0f, 2.0f, 0.0f));
        footCollider->SetTrigger(true);

        // Camera
        // -------------------------------------------------------------------------------------------------------
		auto camera2 = CreateGameObject<GameObject>("Culling Debug Camera");
		camera2->AddComponent<Camera>();
		camera2->AddComponent<CameraController>();
		camera2->transform.SetLocalPosition(Vector3(0.0f, 15.0f, -80.0f));
        camera2->transform.SetLocalScale(Vector3(0.75f, 0.75f, 1.0f));
        auto camera2Renderer = camera2->AddComponent<MeshRenderer>();
        camera2Renderer->SetMesh(ResourceManager::GetInstance().GetMesh("Primitive_cube"));

        // Floor
        // -------------------------------------------------------------------------------------------------------
        auto floor = CreateGameObject<GameObject>("Floor");
        floor->SetTag(ObjectTag::Ground);
        floor->transform.SetLocalRotation(Vector3(90.0f, 0.0f, 0.0f));
        floor->transform.SetLocalScale(Vector3(500.0f, 500.0f, 1.0f));
        floor->transform.SetLocalPosition(Vector3(0.0f, -50.0f, 0.0f));

        auto floorRenderer = floor->AddComponent<MeshRenderer>();
        floorRenderer->SetMesh(ResourceManager::GetInstance().GetMesh("Primitive_quad"));
        floorRenderer->SetMaterial(0, ResourceManager::GetInstance().GetMaterial("Textured_material"));
        floorRenderer->GetMaterial(0)->SetTexture(0, ResourceManager::GetInstance().GetTexture("T_Snow_Ground"));

        auto floorRigidbody = floor->AddComponent<Rp3dRigidbody>(1000.0f, BodyType::STATIC, false);
        auto floorCollider = floor->AddComponent<Rp3dBoxCollider>();
        floorCollider->SetSize(Vector3(500.0f, 500.0f, 10.0f));
        floorCollider->SetCollisionLayer(CollisionLayer::Ground);
        floorCollider->SetBounciness(0.0f);

        // Snow Floor
        auto snowFloor = CreateGameObject<GameObject>("WinterFell");
        snowFloor->SetTag(ObjectTag::Ground);
        snowFloor->transform.SetLocalScale(Vector3(0.2f));
        snowFloor->AddComponent<MeshRenderer>()->SetMesh(ResourceManager::GetInstance().GetModel("Winter_Barrel_03"));

        //auto cube1 = CreateGameObject<GameObject>("Cube1");
        //auto cube1Renderer = cube1->AddComponent<MeshRenderer>();
        //cube1Renderer->SetMesh(ResourceManager::GetInstance().GetMesh("Primitive_cube"));
        //cube1Renderer->GetMaterial(0)->SetColor(Vector4(1.0f, 1.0f, 5.0f, 1.0f));
        //auto cube1PointLight = cube1->AddComponent<Light>();
        //cube1PointLight->type = LightType::Point;
        //cube1PointLight->color = Vector4(1.0f, 1.0f, 5.0f, 100.0f);

        //cube1->transform.SetLocalPosition(Vector3(20.0f, 5.0f, 10.0f));
        //cube1->transform.SetLocalScale(Vector3(2.5f, 2.5f, 2.5f));

        //auto cube2 = CreateGameObject<GameObject>("Cube2");
        //auto cube2Renderer = cube2->AddComponent<MeshRenderer>();
        //cube2Renderer->SetMesh(ResourceManager::GetInstance().GetMesh("Primitive_cube"));
        //cube2Renderer->GetMaterial(0)->SetColor(Vector4(5.0f, 1.0f, 1.0f, 1.0f));
        //auto cube2PointLight = cube2->AddComponent<Light>();
        //cube2PointLight->type = LightType::Point;
        //cube2PointLight->color = Vector4(5.0f, 1.0f, 1.0f, 100.0f);

        //cube2->transform.SetLocalPosition(Vector3(-20.0f, 5.0f, -10.0f));
        //cube2->transform.SetLocalScale(Vector3(2.5f, 2.5f, 2.5f));

        //auto cube3 = CreateGameObject<GameObject>("PointLight");
        //auto cube3Renderer = cube3->AddComponent<MeshRenderer>();
        //cube3Renderer->SetMesh(ResourceManager::GetInstance().GetMesh("Primitive_cube"));
        //cube3Renderer->GetMaterial(0)->SetColor(Vector4(1.0f, 5.0f, 1.0f, 1.0f));
        //auto cube3PointLight = cube3->AddComponent<Light>();
        //cube3PointLight->type = LightType::Point;
        //cube3PointLight->color = Vector4(1.0f, 5.0f, 1.0f, 1000.0f);
        //cube3->transform.SetLocalPosition(Vector3(0.0f, 15.0f, 10.0f));
        //cube3->transform.SetLocalScale(Vector3(5.0f, 5.0f, 5.0f));

        auto directionalLight = CreateGameObject<GameObject>("DirectionalLight");
        auto directionalLightRenderer = directionalLight->AddComponent<MeshRenderer>();
        // directionalLightRenderer->SetMesh(ResourceManager::GetInstance().GetMesh("Primitive_cube"));
        auto directionalLightComponent = directionalLight->AddComponent<Light>();
        directionalLightComponent->type = LightType::Directional;
        directionalLightComponent->color = Vector4(1.0f, 1.0f, 0.8f, 1.0f);
        directionalLight->transform.SetLocalPosition(Vector3(0.0f, 30.0f, 0.0f));
        directionalLight->transform.SetLocalRotation(Vector3(50.0f, 0.0f, 0.0f));

        auto lightingCube = CreateGameObject<GameObject>("Cube1");
        auto lightingCubeRenderer = lightingCube->AddComponent<MeshRenderer>();
        lightingCubeRenderer->SetMesh(ResourceManager::GetInstance().GetMesh("Primitive_cube"));
        lightingCube->transform.SetLocalPosition(Vector3(20.0f, 10.0f, 0.0f));
        lightingCube->transform.SetLocalScale(Vector3(10.0f, 10.0f, 10.0f));
        auto lightingCubeRigidbody = lightingCube->AddComponent<Rp3dRigidbody>(10.0f, BodyType::KINEMATIC, true);
        auto lightingCubeCollider = lightingCube->AddComponent<Rp3dBoxCollider>(Vector3(12.0f, 12.0f, 12.0f));
        lightingCubeCollider->SetBounciness(0.1f);

        auto lightingCube1 = CreateGameObject<GameObject>("Cube2");
        lightingCube1->SetTag(ObjectTag::Object);
        auto lightingCubeRenderer1 = lightingCube1->AddComponent<MeshRenderer>();
        lightingCubeRenderer1->SetMesh(ResourceManager::GetInstance().GetMesh("Primitive_cube"));
        lightingCube1->transform.SetLocalPosition(Vector3(0.0f, 10.0f, 0.0f));
        lightingCube1->transform.SetLocalScale(Vector3(10.0f, 10.0f, 10.0f));
        lightingCube1->transform.SetLocalRotation(Vector3(0.0f, 0.0f, 0.0f));
        auto lightingCube1Rigidbody = lightingCube1->AddComponent<Rp3dRigidbody>(10.0f, BodyType::DYNAMIC, true);
        auto lightingCube1Collider = lightingCube1->AddComponent<Rp3dBoxCollider>(Vector3(10.0f, 10.0f, 10.0f));
        lightingCube1Collider->SetBounciness(0.0f);
	}
}