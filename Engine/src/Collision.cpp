#include "Collision.h"
#include "Log.h"

#include <algorithm>

namespace Engine
{
	bool Collision::Intersects(Collider* colliderA, Collider* colliderB)
	{
        const ColliderType typeA = colliderA->GetColliderType();
        const ColliderType typeB = colliderB->GetColliderType();

        // box to box
        if (typeA == ColliderType::OBB && typeB == ColliderType::OBB)
        {
            return IntersectsBoxToBox(static_cast<BoxCollider*>(colliderA), static_cast<BoxCollider*>(colliderB));
        }

        // box to sphere
        if (typeA == ColliderType::OBB && typeB == ColliderType::Sphere)
        {
            return IntersectsBoxToSphere(static_cast<BoxCollider*>(colliderA), static_cast<SphereCollider*>(colliderB));
        }
        if (typeA == ColliderType::Sphere && typeB == ColliderType::OBB)
        {
            return IntersectsBoxToSphere(static_cast<BoxCollider*>(colliderB), static_cast<SphereCollider*>(colliderA));
        }

        // sphere to sphere
        if (typeA == ColliderType::Sphere && typeB == ColliderType::Sphere)
        {
            return IntersectsSphereToSphere(static_cast<SphereCollider*>(colliderA), static_cast<SphereCollider*>(colliderB));
        }

        return false;
	}

	bool Collision::IntersectsBoxToBox(BoxCollider* boxA, BoxCollider* boxB)
	{
        // colliders world center
        const Vector3 centerA = boxA->GetWorldCenter();
        const Vector3 centerB = boxB->GetWorldCenter();
        // colliders world extents
        const Vector3 extentsA = boxA->GetWorldExtents();
        const Vector3 extentsB = boxB->GetWorldExtents();

        // colliderA axes
        const Vector3 rightA = boxA->ownerGameObject->transform.GetRight();
        const Vector3 upA = boxA->ownerGameObject->transform.GetUp();
        const Vector3 forwardA = boxA->ownerGameObject->transform.GetForward();
        // colliderB axes
        const Vector3 rightB = boxB->ownerGameObject->transform.GetRight();
        const Vector3 upB = boxB->ownerGameObject->transform.GetUp();
        const Vector3 forwardB = boxB->ownerGameObject->transform.GetForward();
        // axes for SAT
        std::array<Vector3, 15> axes =
        {
            rightA,
            upA,
            forwardA,
            rightB,
            upB,
            forwardB,
            Cross(rightA, rightB),
            Cross(rightA, upB),
            Cross(rightA, forwardB),
            Cross(upA, rightB),
            Cross(upA, upB),
            Cross(upA, forwardB),
            Cross(forwardA, rightB),
            Cross(forwardA, upB),
            Cross(forwardA, forwardB)
        };

        // Check intersects with Separating Axis Theorem
        for (Vector3 axis : axes)
        {
            // Ignores zero vector operations
            if (LengthSq(axis) < 1e-6f)
            {
                continue;
            }
            axis = Normalize(axis);
            // distance between centers about axis
            float distance = std::abs(Dot(centerB - centerA, axis));
            // radius about axis
            float radiusA = std::abs(Dot(rightA, axis)) * extentsA.x + std::abs(Dot(upA, axis)) * extentsA.y + std::abs(Dot(forwardA, axis)) * extentsA.z;
            float radiusB = std::abs(Dot(rightB, axis)) * extentsB.x + std::abs(Dot(upB, axis)) * extentsB.y + std::abs(Dot(forwardB, axis)) * extentsB.z;

            if (distance > radiusA + radiusB)
            {
                return false;
            }
        }

        return true;
	}

	bool Collision::IntersectsBoxToSphere(BoxCollider* box, SphereCollider* sphere)
	{
        const Vector3 boxCenter = box->GetWorldCenter();
        const Vector3 extents = box->GetWorldExtents();

        const Vector3 sphereCenter = sphere->GetWorldCenter();
        const float sphereRadius = sphere->GetWorldRadius();

        const Vector3 right = box->ownerGameObject->transform.GetRight();
        const Vector3 up = box->ownerGameObject->transform.GetUp();
        const Vector3 forward = box->ownerGameObject->transform.GetForward();

        // Sphere center relative to OBB center
        const Vector3 d = sphereCenter - boxCenter;

        // Convert sphere center to OBB local coordinates
        const float localX = Dot(d, right);
        const float localY = Dot(d, up);
        const float localZ = Dot(d, forward);

        // Closest point on box in local coordinates
        const float closestX = std::clamp(localX, -extents.x, extents.x);
        const float closestY = std::clamp(localY, -extents.y, extents.y);
        const float closestZ = std::clamp(localZ, -extents.z, extents.z);

        // Closest point back to world
        const Vector3 closestPoint = boxCenter + right * closestX + up * closestY + forward * closestZ;

        return LengthSq(sphereCenter - closestPoint) <= sphereRadius * sphereRadius;
	}

	bool Collision::IntersectsSphereToSphere(SphereCollider* sphereA, SphereCollider* sphereB)
	{
        const Vector3 centerA = sphereA->GetWorldCenter();
        const Vector3 centerB = sphereB->GetWorldCenter();

        const float radiusA = sphereA->GetWorldRadius();
        const float radiusB = sphereB->GetWorldRadius();

        const float radiusSum = radiusA + radiusB;

        return LengthSq(centerB - centerA) <= radiusSum * radiusSum;
	}
}