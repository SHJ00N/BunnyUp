#pragma once

#include "Frustum.h"
#include "Transform.h"
#include "Log.h"

#include <array>
#include <vector>

namespace Engine
{
	struct BoundingVolume
	{
		virtual bool IsOnFrustum(const Frustum& camFrustum, const Transform& transform) const = 0;
		virtual bool IsOnOrForwardPlane(const Plane& plane) const = 0;
	};

	struct AABB : public BoundingVolume
	{
		Vector3 center = { 0.0f, 0.0f, 0.0f };
		Vector3 extents = { 0.0f, 0.0f, 0.0f };

		AABB(const Vector3& min, const Vector3& max)
			: BoundingVolume{}, center{ (max + min) * 0.5f }, extents{ (max - min) * 0.5f }
		{
		}

		AABB(const Vector3& inCenter, float iI, float iJ, float iK)
			: BoundingVolume{}, center{ inCenter }, extents{ iI, iJ, iK }
		{
		}
		
		// Generate AABB from mesh
		AABB(const std::vector<Vector3>& positions)
		{
			Vector3 minAABB(FLT_MAX, FLT_MAX, FLT_MAX);
			Vector3 maxAABB(-FLT_MAX, -FLT_MAX, -FLT_MAX);
			for (auto&& pos : positions)
			{
				minAABB.x = min(minAABB.x, pos.x);
				minAABB.y = min(minAABB.y, pos.y);
				minAABB.z = min(minAABB.z, pos.z);

				maxAABB.x = max(maxAABB.x, pos.x);
				maxAABB.y = max(maxAABB.y, pos.y);
				maxAABB.z = max(maxAABB.z, pos.z);
			}
			
			center = { (maxAABB + minAABB) * 0.5f };
			extents = { maxAABB.x - center.x, maxAABB.y - center.y, maxAABB.z - center.z };
		}

		// vertices for rendering aabb bound
		std::array<Vector3, 8> GetVertices() const
		{
			std::array<Vector3, 8> vertice;
			vertice[0] = { center.x - extents.x, center.y - extents.y, center.z - extents.z };
			vertice[1] = { center.x + extents.x, center.y - extents.y, center.z - extents.z };
			vertice[2] = { center.x - extents.x, center.y + extents.y, center.z - extents.z };
			vertice[3] = { center.x + extents.x, center.y + extents.y, center.z - extents.z };
			vertice[4] = { center.x - extents.x, center.y - extents.y, center.z + extents.z };
			vertice[5] = { center.x + extents.x, center.y - extents.y, center.z + extents.z };
			vertice[6] = { center.x - extents.x, center.y + extents.y, center.z + extents.z };
			vertice[7] = { center.x + extents.x, center.y + extents.y, center.z + extents.z };
			return vertice;
		}

		Vector3 GetMin() const
		{
			return center - extents;
		}

		Vector3 GetMax() const
		{
			return center + extents;
		}

		bool IsOnOrForwardPlane(const Plane& plane) const final
		{
			// Compute the projection interval radius of b onto L(t) = b.c + t * p.n
			const float r = extents.x * std::abs(plane.normal.x) + extents.y * std::abs(plane.normal.y) +
				extents.z * std::abs(plane.normal.z);

			return -r <= plane.GetSignedDistanceToPlane(center);
		}

		bool IsOnFrustum(const Frustum& camFrustum, const Transform& transform) const final
		{
			//Get global scale thanks to our transform
			const auto& worldMatrix = transform.GetWorldMatrix();
			const Vector4 gCenterV4 = worldMatrix * Vector4(center.x, center.y, center.z, 1.f);
			const Vector3 globalCenter = { gCenterV4.x, gCenterV4.y, gCenterV4.z };

			// Scaled orientation
			const Vector3 right = Vector3(worldMatrix.m00, worldMatrix.m01, worldMatrix.m02) * extents.x;
			const Vector3 up = Vector3(worldMatrix.m10, worldMatrix.m11, worldMatrix.m12) * extents.y;
			const Vector3 forward = Vector3(worldMatrix.m20, worldMatrix.m21, worldMatrix.m22) * extents.z;

			const float newIi = std::abs(Dot(Vector3{ 1.f, 0.f, 0.f }, right)) +
				std::abs(Dot(Vector3{ 1.f, 0.f, 0.f }, up)) +
				std::abs(Dot(Vector3{ 1.f, 0.f, 0.f }, forward));

			const float newIj = std::abs(Dot(Vector3{ 0.f, 1.f, 0.f }, right)) +
				std::abs(Dot(Vector3{ 0.f, 1.f, 0.f }, up)) +
				std::abs(Dot(Vector3{ 0.f, 1.f, 0.f }, forward));

			const float newIk = std::abs(Dot(Vector3{ 0.f, 0.f, 1.f }, right)) +
				std::abs(Dot(Vector3{ 0.f, 0.f, 1.f }, up)) +
				std::abs(Dot(Vector3{ 0.f, 0.f, 1.f }, forward));

			const AABB globalAABB(globalCenter, newIi, newIj, newIk);

			//float d1 = globalAABB.IsOnOrForwardPlane(camFrustum.leftFace);
			//float d2 = globalAABB.IsOnOrForwardPlane(camFrustum.rightFace);
			//float d3 = globalAABB.IsOnOrForwardPlane(camFrustum.topFace);
			//float d4 = globalAABB.IsOnOrForwardPlane(camFrustum.bottomFace);
			//float d5 = globalAABB.IsOnOrForwardPlane(camFrustum.nearFace);
			//float d6 = globalAABB.IsOnOrForwardPlane(camFrustum.farFace);
			//LOG_INFO("name : %s\nleft : %f, right : %f, top : %f, bottom : %f, near : %f, far : %f\n", transform.owner->GetName().c_str(), d1, d2, d3, d4, d5, d6);
			//LOG_INFO("extens : %f, %f, %f", globalAABB.extents.x, globalAABB.extents.y, globalAABB.extents.z);

			return (
				globalAABB.IsOnOrForwardPlane(camFrustum.leftFace) &&
				globalAABB.IsOnOrForwardPlane(camFrustum.rightFace) &&
				globalAABB.IsOnOrForwardPlane(camFrustum.topFace) &&
				globalAABB.IsOnOrForwardPlane(camFrustum.bottomFace) &&
				globalAABB.IsOnOrForwardPlane(camFrustum.nearFace) &&
				globalAABB.IsOnOrForwardPlane(camFrustum.farFace)
			);
		}
	};
}