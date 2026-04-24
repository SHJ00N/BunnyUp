#pragma once

#include "MathHelper.h"

namespace Engine
{
	struct BoneInfo
	{
		/*id is index in finalBoneMatrices*/
		int id = -1;

		/*offset matrix transforms vertex from model space to bone space*/
		Matrix4x4 offset;
	};
}