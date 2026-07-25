#include <EngineSystem.h>

#include "DemoScene2.h"
#include "CubeController.h"
#include "CubeGenerator.h"
#include "Effect/EffectPoolManager.h"
#include "Input/PlayerInputManager.h"
#include "Enemy/NavigationManager.h"

namespace Game
{
	DemoScene2::DemoScene2()
	{
	}

	DemoScene2::~DemoScene2()
	{
	}

	void DemoScene2::SceneEnter()
	{
        SetEnvironmentMap(ResourceManager::GetInstance().GetEnvironmentMap("Sky_EnvMap"));

        auto floor = CreateGameObject<GameObject>("Floor");
        auto floorRenderer = floor->AddComponent<MeshRenderer>();
        floorRenderer->SetMesh(ResourceManager::GetInstance().GetMesh("Primitive_quad"));
        floorRenderer->SetMaterial(0, ResourceManager::GetInstance().GetMaterial("Textured_material"));
        floorRenderer->GetMaterial(0)->SetTexture(0, ResourceManager::GetInstance().GetTexture("T_Snow_Ground"));
        floorRenderer->GetMaterial(0)->SetSampler(0, SamplerStateManager::GetInstance().GetSampler(SamplerType::LinearClamp));

        floor->transform.SetLocalRotation(Vector3(90.0f, 0.0f, 0.0f));
        floor->transform.SetLocalScale(Vector3(500.0f, 500.0f, 1.0f));
        floor->transform.SetLocalPosition(Vector3(0.0f, -100.0f, 0.0f));

        auto floorRigidbody = floor->AddComponent<Rp3dRigidbody>(1.0f, BodyType::STATIC, false);
        auto floorCollider = floor->AddComponent<Rp3dBoxCollider>();
        floorCollider->SetSize(Vector3(500.0f, 500.0f, 1.0f));

        // test
        auto A = CreateGameObject<GameObject>("Hit_Effect_A");
        A->transform.SetLocalScale(Vector3(100.0f));
        auto ARenderer = A->AddComponent<EffectRenderer>(EffectType::Billboard);
        ARenderer->SetSpriteSheet(ResourceManager::GetInstance().GetTexture("AHit_Effect"), 4, 4);
        auto AEffect = A->AddComponent<EffectComponent>();
        AEffect->SetLoop(true);

        auto B = CreateGameObject<GameObject>("Hit_Effect_B");
        B->transform.SetLocalScale(Vector3(100.0f));
        auto BRenderer = B->AddComponent<EffectRenderer>(EffectType::Billboard);
        BRenderer->SetSpriteSheet(ResourceManager::GetInstance().GetTexture("BHit_Effect"), 4, 4);
        auto BEffect = B->AddComponent<EffectComponent>();
        BEffect->SetLoop(true);

        auto C = CreateGameObject<GameObject>("Smoke_Effect");
        C->transform.SetLocalScale(Vector3(100.0f));
        auto CRenderer = C->AddComponent<EffectRenderer>(EffectType::Normal);
        CRenderer->SetSpriteSheet(ResourceManager::GetInstance().GetTexture("Dust_Effect"), 5, 5);
        auto CEffect = C->AddComponent<EffectComponent>();
        CEffect->SetLoop(true);
	}
}