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

		AABB() = default;

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
				minAABB.x = minAABB.x < pos.x ? minAABB.x : pos.x;
				minAABB.y = minAABB.y < pos.y ? minAABB.y : pos.y;
				minAABB.z = minAABB.z < pos.z ? minAABB.z : pos.z;

				maxAABB.x = maxAABB.x > pos.x ? maxAABB.x : pos.x;
				maxAABB.y = maxAABB.y > pos.y ? maxAABB.y : pos.y;
				maxAABB.z = maxAABB.z > pos.z ? maxAABB.z : pos.z;
			}
			
			center = (maxAABB + minAABB) * 0.5f;
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

		void MergeBounds(const AABB& other)
		{
			Vector3 minAABB = GetMin();
			Vector3 maxAABB = GetMax();
			Vector3 otherMin = other.GetMin();
			Vector3 otherMax = other.GetMax();
			minAABB.x = minAABB.x < otherMin.x ? minAABB.x : otherMin.x;
			minAABB.y = minAABB.y < otherMin.y ? minAABB.y : otherMin.y;
			minAABB.z = minAABB.z < otherMin.z ? minAABB.z : otherMin.z;
			maxAABB.x = maxAABB.x > otherMax.x ? maxAABB.x : otherMax.x;
			maxAABB.y = maxAABB.y > otherMax.y ? maxAABB.y : otherMax.y;
			maxAABB.z = maxAABB.z > otherMax.z ? maxAABB.z : otherMax.z;
			center = (maxAABB + minAABB) * 0.5f;
			extents = (maxAABB - minAABB) * 0.5f;
		}

		static AABB MergeBounds(const AABB& a, const AABB& b)
		{
			Vector3 minAABB = a.GetMin();
			Vector3 maxAABB = a.GetMax();
			Vector3 otherMin = b.GetMin();
			Vector3 otherMax = b.GetMax();
			minAABB.x = minAABB.x < otherMin.x ? minAABB.x : otherMin.x;
			minAABB.y = minAABB.y < otherMin.y ? minAABB.y : otherMin.y;
			minAABB.z = minAABB.z < otherMin.z ? minAABB.z : otherMin.z;
			maxAABB.x = maxAABB.x > otherMax.x ? maxAABB.x : otherMax.x;
			maxAABB.y = maxAABB.y > otherMax.y ? maxAABB.y : otherMax.y;
			maxAABB.z = maxAABB.z > otherMax.z ? maxAABB.z : otherMax.z;
			return AABB(minAABB, maxAABB);
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

			return (
				globalAABB.IsOnOrForwardPlane(camFrustum.leftFace) &&
				globalAABB.IsOnOrForwardPlane(camFrustum.rightFace) &&
				globalAABB.IsOnOrForwardPlane(camFrustum.topFace) &&
				globalAABB.IsOnOrForwardPlane(camFrustum.bottomFace) &&
				globalAABB.IsOnOrForwardPlane(camFrustum.nearFace) &&
				globalAABB.IsOnOrForwardPlane(camFrustum.farFace)
			);
		}

		bool IsIntersectsAABB(const AABB& other) const
		{
			return (
				std::abs(center.x - other.center.x) <= (extents.x + other.extents.x) &&
				std::abs(center.y - other.center.y) <= (extents.y + other.extents.y) &&
				std::abs(center.z - other.center.z) <= (extents.z + other.extents.z)
				);
		}

		bool Contains(const AABB& other) const
		{
			return (
				std::abs(center.x - other.center.x) + other.extents.x <= extents.x &&
				std::abs(center.y - other.center.y) + other.extents.y <= extents.y &&
				std::abs(center.z - other.center.z) + other.extents.z <= extents.z
				);
		}
	};
}