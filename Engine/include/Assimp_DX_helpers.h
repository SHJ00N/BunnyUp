#pragma once

#include<assimp/quaternion.h>
#include<assimp/vector3.h>
#include<assimp/matrix4x4.h>

#include "MathHelper.h"

namespace Engine
{
	class AssimpDXHelpers
	{
	public:

		static inline Matrix4x4 ConvertMatrixToDXFormat(const aiMatrix4x4& from)
		{
			Matrix4x4 to;
			// row-major
			to.m00 = from.a1; to.m01 = from.b1; to.m02 = from.c1; to.m03 = from.d1;
			to.m10 = from.a2; to.m11 = from.b2; to.m12 = from.c2; to.m13 = from.d2;
			to.m20 = from.a3; to.m21 = from.b3; to.m22 = from.c3; to.m23 = from.d3;
			to.m30 = from.a4; to.m31 = from.b4; to.m32 = from.c4; to.m33 = from.d4;
			return to;
		}

		static inline Vector3 GetDXVec(const aiVector3D& vec)
		{
			return Vector3(vec.x, vec.y, vec.z);
		}
		static inline Quaternion GetDXQuat(const aiQuaternion& pOrientation)
		{
			return Quaternion(pOrientation.x, pOrientation.y, pOrientation.z, pOrientation.w);
		}
	};
}