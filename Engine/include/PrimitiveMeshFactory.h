#pragma once

#include "VertexType.h"

#include <vector>

namespace Engine
{
	template<typename VertexType>
	struct PrimitiveMeshData
	{
		std::vector<VertexType> vertices;
		std::vector<uint32_t> indices;
	};

	class PrimitiveMeshFactory
	{
	public:
		static PrimitiveMeshData<VertexPNUT> CreateQuad();
		static PrimitiveMeshData<VertexPNUT> CreateFloorQuad();
		static PrimitiveMeshData<VertexPNUT> CreateCube();
		static PrimitiveMeshData<VertexPU> CreateFullScreenQuad();
	};
}