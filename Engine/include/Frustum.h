#pragma once

#include "MathHelper.h"
#include "Camera.h"
#include "GameObject.h"

#include <array>

namespace Engine
{
	struct Plane
	{
		Vector3 normal = { 0.0f, 1.0f, 0.0f };		// unit vector
		float distance = 0.0f;	// distance with origin

		Plane() = default;
		Plane(const Vector3& p, const Vector3 norm) : normal(Normalize(norm)), distance(Dot(normal, p)) { }
		float GetSignedDistanceToPlane(const Vector3& point) const
		{
			return Dot(normal, point) - distance;
		}
		Plane(float a, float b, float c, float d)
		{
			Vector3 n(a, b, c);

			float len = Length(n);

			n /= len;
			d /= len;

			normal = n;
			distance = -d;
		}
	};

	inline Vector3 IntersectPlanes(const Plane& p1, const Plane& p2, const Plane& p3)
	{
		const Vector3& n1 = p1.normal;
		const Vector3& n2 = p2.normal;
		const Vector3& n3 = p3.normal;

		Vector3 cross23 = Cross(n2, n3);
		Vector3 cross31 = Cross(n3, n1);
		Vector3 cross12 = Cross(n1, n2);

		float denom = Dot(n1, cross23);

		assert(fabs(denom) > 0.0001f);

		return (cross23 * p1.distance + cross31 * p2.distance + cross12 * p3.distance) / denom;
	}

	struct Frustum
	{
		Plane topFace;
		Plane bottomFace;

		Plane rightFace;
		Plane leftFace;

		Plane farFace;
		Plane nearFace;

		std::array<Vector3, 8> GetCorners() const
		{
			return
			{
				// Near
				IntersectPlanes(nearFace, leftFace,  topFace),
				IntersectPlanes(nearFace, topFace,   rightFace),
				IntersectPlanes(nearFace, bottomFace,leftFace),
				IntersectPlanes(nearFace, rightFace, bottomFace),

				// Far
				IntersectPlanes(farFace,  topFace,   leftFace),
				IntersectPlanes(farFace,  rightFace, topFace),
				IntersectPlanes(farFace,  leftFace,  bottomFace),
				IntersectPlanes(farFace,  bottomFace,rightFace)
			};
		}

		Vector4 planes[6];
	};

	inline Frustum createFrustumFromCamera(const Camera& camera)
	{
		// calculate viewProjection matrix
		Matrix4x4 vpMat =  camera.GetViewMatrix() * camera.GetProjectionMatrix();

		// get frustum
		Frustum frustum;

		frustum.nearFace = { vpMat.m[0][2], vpMat.m[1][2], vpMat.m[2][2], vpMat.m[3][2] };
		frustum.farFace = { vpMat.m[0][3] - vpMat.m[0][2], vpMat.m[1][3] - vpMat.m[1][2], vpMat.m[2][3] - vpMat.m[2][2], vpMat.m[3][3] - vpMat.m[3][2] };
		frustum.leftFace = { vpMat.m[0][3] + vpMat.m[0][0], vpMat.m[1][3] + vpMat.m[1][0], vpMat.m[2][3] + vpMat.m[2][0], vpMat.m[3][3] + vpMat.m[3][0] };
		frustum.rightFace = { vpMat.m[0][3] - vpMat.m[0][0], vpMat.m[1][3] - vpMat.m[1][0], vpMat.m[2][3] - vpMat.m[2][0], vpMat.m[3][3] - vpMat.m[3][0] };
		frustum.topFace = { vpMat.m[0][3] - vpMat.m[0][1], vpMat.m[1][3] - vpMat.m[1][1], vpMat.m[2][3] - vpMat.m[2][1], vpMat.m[3][3] - vpMat.m[3][1] };
		frustum.bottomFace = { vpMat.m[0][3] + vpMat.m[0][1], vpMat.m[1][3] + vpMat.m[1][1], vpMat.m[2][3] + vpMat.m[2][1], vpMat.m[3][3] + vpMat.m[3][1] };

		return frustum;
	}
}