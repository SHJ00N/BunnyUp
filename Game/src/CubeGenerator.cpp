#include "CubeGenerator.h"
#include "CubeController.h"

namespace Game
{
    void CubeGenerator::Update(float dt)
    {
        m_accumulatedTimer += dt;

        if (m_accumulatedTimer >= 0.5f && m_cubes.size() < 20)
        {
            auto* scene = ownerGameObject->scene;

            auto cube = scene->CreateGameObject<GameObject>("Cube" + std::to_string(m_cubes.size()));
            cube->AddComponent<CubeController>();
            cube->transform.SetLocalScale(Vector3(10.0f));
            auto cubeRenderer = cube->AddComponent<MeshRenderer>();
            cubeRenderer->SetMesh(ResourceManager::GetInstance().GetMesh("Primitive_cube"));

            auto cubeRigidbody = cube->AddComponent<Rp3dRigidbody>(1.0f, BodyType::DYNAMIC, true);
            auto cubeCollider = cube->AddComponent<Rp3dBoxCollider>(Vector3(11.0f));

            m_cubes.push_back(cube);
            m_accumulatedTimer = 0.0f;
        }

        if (m_cubes.size() >= 20 && m_accumulatedTimer >= 20.0f)
        {
            for (auto* cube : m_cubes)
            {
                cube->Destroy();
            }

            m_cubes.clear();
            m_accumulatedTimer = 0.0f;
        }
    }
}