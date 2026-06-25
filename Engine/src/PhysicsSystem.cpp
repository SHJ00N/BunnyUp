#include "pch.h"
#include "PhysicsSystem.h"
#include "Rp3dRigidbody.h"
#include "Log.h"
#include "MathHelper.h"
#include "DebugRenderer.h"

namespace Engine
{
	PhysicsSystem::PhysicsSystem() : m_world(nullptr), DebugRenderEnabled(false)
	{
	}

	PhysicsSystem::~PhysicsSystem()
	{
		// prevent memory leak
		Shutdown();
	}

	void PhysicsSystem::Initialize()
	{
		m_rigidbodyCreateListenerID = EventBus::GetInstance().Subscribe<RigidbodyCreateEvent>(
			[this](const RigidbodyCreateEvent& e)
			{
				if (e.body)
				{
					m_bodies[e.body->GetId()] = e.body;
				}
			}
		);

		m_rigidbodyDestroyListenerID = EventBus::GetInstance().Subscribe<RigidbodyDestroyEvent>(
			[this](const RigidbodyDestroyEvent& e)
			{
				if (e.body)
				{
					m_bodies.erase(e.body->GetId());
				}
			}
		);

		// already exist
		if (m_world) return;

		// create a physics world
		rp3d::PhysicsWorld::WorldSettings settings;
		settings.defaultVelocitySolverNbIterations = 20;
		settings.isSleepingEnabled = false;
		settings.gravity = rp3d::Vector3(0, -9.81, 0);
		m_world = m_common.createPhysicsWorld(settings);

		// setting world
		// Change the number of iterations of the velocity solver. default value is 10
		m_world->setNbIterationsVelocitySolver(10);
		// Change the number of iterations of the position solver. default value is 5
		m_world->setNbIterationsPositionSolver(5);
		// Set state the sleeping technique. default is true
		m_world->enableSleeping(true);
	}

	void PhysicsSystem::Shutdown()
	{
		EventBus::GetInstance().Unsubscribe<RigidbodyCreateEvent>(m_rigidbodyCreateListenerID);
		EventBus::GetInstance().Unsubscribe<RigidbodyDestroyEvent>(m_rigidbodyDestroyListenerID);

		// world is null
		if (!m_world) return;
		// destory the world
		m_common.destroyPhysicsWorld(m_world);
		m_world = nullptr;
	}

	void PhysicsSystem::Update(float fdt)
	{
		if (!m_world) return;

		// synchronizing game object transform to rp3d rigidBody transform
		for (auto body : m_bodies)
		{
			auto rigidbody = body.second;
			if (rigidbody && rigidbody->GetType() == BodyType::KINEMATIC)
			{
				body.second->SyncTransformToPhysics();
			}
		}
		
		m_world->update(fdt);

		// synchronizing rp3d rigidBody transform to game object transform
		for (auto body : m_bodies)
		{
			auto rigidbody = body.second;
			if (rigidbody && rigidbody->GetType() == BodyType::DYNAMIC)
			{
				body.second->SyncPhysicsToTransform();
			}
		}
	}

	void PhysicsSystem::SetDebugDraw(bool value)
	{
		if (!m_world) return;

		DebugRenderEnabled = value;
		m_world->setIsDebugRenderingEnabled(value);
		rp3d::DebugRenderer& debugRenderer = m_world->getDebugRenderer();
		debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLISION_SHAPE, value);
		debugRenderer.setIsDebugItemDisplayed(rp3d::DebugRenderer::DebugItem::COLLIDER_BROADPHASE_AABB, value);
	}

	void PhysicsSystem::DrawDebug()
	{
		if (!m_world || !DebugRenderEnabled) return;

		const auto& debugRenderer = m_world->getDebugRenderer();

		const auto& lines = debugRenderer.getLines();
		for (const auto& line : lines)
		{
			uint32_t color1 = line.color1;
			float r1 = ((color1 >> 24) & 0xFF) / 255.0f;
			float g1 = ((color1 >> 16) & 0xFF) / 255.0f;
			float b1 = ((color1 >> 8) & 0xFF) / 255.0f;
			float a1 = ((color1 >> 0) & 0xFF) / 255.0f;

			uint32_t color2 = line.color2;
			float r2 = ((color2 >> 24) & 0xFF) / 255.0f;
			float g2 = ((color2 >> 16) & 0xFF) / 255.0f;
			float b2 = ((color2 >> 8) & 0xFF) / 255.0f;
			float a2 = ((color2 >> 0) & 0xFF) / 255.0f;


			DebugRenderer::GetInstance().AddLine(
				Vector3(line.point1.x, line.point1.y, line.point1.z),
				Vector3(line.point2.x, line.point2.y, line.point2.z),
				Vector4(r1, g1, b1, a1),
				Vector4(r2, g2, b2, a2));
		}

		const auto& triangles = debugRenderer.getTriangles();
		for (const auto& triangle : triangles)
		{
			uint32_t color1 = triangle.color1;
			float r1 = ((color1 >> 24) & 0xFF) / 255.0f;
			float g1 = ((color1 >> 16) & 0xFF) / 255.0f;
			float b1 = ((color1 >> 8) & 0xFF) / 255.0f;
			float a1 = ((color1 >> 0) & 0xFF) / 255.0f;

			uint32_t color2 = triangle.color2;
			float r2 = ((color2 >> 24) & 0xFF) / 255.0f;
			float g2 = ((color2 >> 16) & 0xFF) / 255.0f;
			float b2 = ((color2 >> 8) & 0xFF) / 255.0f;
			float a2 = ((color2 >> 0) & 0xFF) / 255.0f;

			uint32_t color3 = triangle.color3;
			float r3 = ((color3 >> 24) & 0xFF) / 255.0f;
			float g3 = ((color3 >> 16) & 0xFF) / 255.0f;
			float b3 = ((color3 >> 8) & 0xFF) / 255.0f;
			float a3 = ((color3 >> 0) & 0xFF) / 255.0f;

			DebugRenderer::GetInstance().AddTriangle(
				Vector3(triangle.point1.x, triangle.point1.y, triangle.point1.z),
				Vector3(triangle.point2.x, triangle.point2.y, triangle.point2.z),
				Vector3(triangle.point3.x, triangle.point3.y, triangle.point3.z),
				Vector4(r1, g1, b1, 0.2f),
				Vector4(r2, g2, b2, 0.2f),
				Vector4(r3, g3, b3, 0.2f));
		}
	}
}