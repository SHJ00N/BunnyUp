#pragma once

#include "MathHelper.h"
#include "Camera.h"
#include "GameObject.h"

namespace Engine
{
	struct Plane
	{
		Vector3 normal = { 0.0f, 1.0f, 0.0f };		// unit vector
		float distance = 0.0f;	// distance with origin

		Plane() = default;
		Plane(const Vector3& p, const Vector3 norm) : normal(Normalize(norm)), distance(Dot(normal, p)) { }
		Plane(float a, float b, float c, float d)
		{
			Vector3 norm = { a, b, c };
			float len = Length(norm);

			norm /= len;
			d /= len;
			
			normal = norm;
			distance = d;
		}

		float GetSignedDistanceToPlane(const Vector3& point) const
		{
			return Dot(normal, point) - distance;
		}
	};

	struct Frustum
	{
		Plane topFace;
		Plane bottomFace;

		Plane rightFace;
		Plane leftFace;

		Plane farFace;
		Plane nearFace;
	};

	inline Frustum createFrustumFromCamera(const Camera& camera)
	{
		// calculate viewProjection matrix
		Matrix4x4 vpMat =  camera.GetViewMatrix() * camera.GetProjectionMatrix();

		// get frustum
		Frustum frustum;

		//frustum.leftFace       = Plane(vpMat.m30 + vpMat.m00, vpMat.m31 + vpMat.m01, vpMat.m32 + vpMat.m02, vpMat.m33 + vpMat.m03);
		//frustum.rightFace     = Plane(vpMat.m30 - vpMat.m00, vpMat.m31 - vpMat.m01, vpMat.m32 - vpMat.m02, vpMat.m33 - vpMat.m03);
		//frustum.topFace       = Plane(vpMat.m30 - vpMat.m10, vpMat.m31 - vpMat.m11, vpMat.m32 - vpMat.m12, vpMat.m33 - vpMat.m13);
		//frustum.bottomFace = Plane(vpMat.m30 + vpMat.m10, vpMat.m31 + vpMat.m11, vpMat.m32 + vpMat.m12, vpMat.m33 + vpMat.m13);
		//frustum.nearFace     = Plane(vpMat.m20, vpMat.m21, vpMat.m22, vpMat.m23);
		//frustum.farFace        = Plane(vpMat.m30 - vpMat.m20, vpMat.m31 - vpMat.m21, vpMat.m32 - vpMat.m22, vpMat.m33 - vpMat.m23);

		auto& transform = camera.ownerGameObject->transform;
		Vector3 cameraPos = transform.GetWorldPosition();
		Vector3 cameraFront = transform.GetForward();
		// near/far plane
		frustum.nearFace = { cameraPos + camera.nearPlane * cameraFront, cameraFront };
		frustum.farFace = { cameraPos + camera.farPlane * cameraFront, cameraFront * -1.0f };
		// left/right/top/bottom plane
		frustum.leftFace = { cameraPos, Vector3(vpMat.m[0][3] + vpMat.m[0][0], vpMat.m[1][3] + vpMat.m[1][0], vpMat.m[2][3] + vpMat.m[2][0]) };
		frustum.rightFace = { cameraPos, Vector3(vpMat.m[0][3] - vpMat.m[0][0], vpMat.m[1][3] - vpMat.m[1][0], vpMat.m[2][3] - vpMat.m[2][0]) };
		frustum.topFace = { cameraPos, Vector3(vpMat.m[0][3] - vpMat.m[0][1], vpMat.m[1][3] - vpMat.m[1][1], vpMat.m[2][3] - vpMat.m[2][1]) };
		frustum.bottomFace = { cameraPos, Vector3(vpMat.m[0][3] + vpMat.m[0][1], vpMat.m[1][3] + vpMat.m[1][1], vpMat.m[2][3] + vpMat.m[2][1]) };

		return frustum;
	}
}