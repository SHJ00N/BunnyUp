#include "PrimitiveMeshFactory.h"

namespace Engine
{
	PrimitiveMeshData<VertexPNUT> PrimitiveMeshFactory::CreateQuad()
	{
        // Create quad geometry
        std::vector<VertexPNUT> Vertices =
        {
            {Vector3{-0.5f, -0.5f,  0.0f}, Vector3{ 0.0f,  0.0f,  1.0f}, Vector2{0, 1}, Vector4{ 1.0f,  0.0f,  0.0f,  1.0f}},
            {Vector3{-0.5f,  0.5f,  0.0f}, Vector3{ 0.0f,  0.0f,  1.0f}, Vector2{0, 0}, Vector4{ 1.0f,  0.0f,  0.0f,  1.0f}},
            {Vector3{ 0.5f,  0.5f,  0.0f}, Vector3{ 0.0f,  0.0f,  1.0f}, Vector2{1, 0}, Vector4{ 1.0f,  0.0f,  0.0f,  1.0f}},
            {Vector3{ 0.5f, -0.5f,  0.0f}, Vector3{ 0.0f,  0.0f,  1.0f}, Vector2{1, 1}, Vector4{ 1.0f,  0.0f,  0.0f,  1.0f}}
        };
        // Create index buffer based on counter-clock wise
        std::vector<uint32_t> Indices =
        {
            0, 2, 1,
            2, 0, 3
        };

        PrimitiveMeshData<VertexPNUT> data;
        data.vertices = Vertices;
        data.indices = Indices;
        return data;
	}

	PrimitiveMeshData<VertexPNUT> PrimitiveMeshFactory::CreateCube()
	{
        // Create cube geometry
        std::vector<VertexPNUT> Vertices =
        {
            //  -X
            {Vector3{-0.5f, -0.5f,  0.5f}, Vector3{-1.0f,  0.0f,  0.0f}, Vector2{0, 1}, Vector4{ 0.0f,  0.0f, -1.0f,  1.0f}},
            {Vector3{-0.5f,  0.5f,  0.5f}, Vector3{-1.0f,  0.0f,  0.0f}, Vector2{0, 0}, Vector4{ 0.0f,  0.0f, -1.0f,  1.0f}},
            {Vector3{-0.5f,  0.5f, -0.5f}, Vector3{-1.0f,  0.0f,  0.0f}, Vector2{1, 0}, Vector4{ 0.0f,  0.0f, -1.0f,  1.0f}},
            {Vector3{-0.5f, -0.5f, -0.5f}, Vector3{-1.0f,  0.0f,  0.0f}, Vector2{1, 1}, Vector4{ 0.0f,  0.0f, -1.0f,  1.0f}},
            // +X
            {Vector3{ 0.5f, -0.5f, -0.5f}, Vector3{ 1.0f,  0.0f,  0.0f}, Vector2{0, 1}, Vector4{ 0.0f,  0.0f,  1.0f,  1.0f}},
            {Vector3{ 0.5f,  0.5f, -0.5f}, Vector3{ 1.0f,  0.0f,  0.0f}, Vector2{0, 0}, Vector4{ 0.0f,  0.0f,  1.0f,  1.0f}},
            {Vector3{ 0.5f,  0.5f,  0.5f}, Vector3{ 1.0f,  0.0f,  0.0f}, Vector2{1, 0}, Vector4{ 0.0f,  0.0f,  1.0f,  1.0f}},
            {Vector3{ 0.5f, -0.5f,  0.5f}, Vector3{ 1.0f,  0.0f,  0.0f}, Vector2{1, 1}, Vector4{ 0.0f,  0.0f,  1.0f,  1.0f}},
            //  -Y
            {Vector3{-0.5f, -0.5f, -0.5f}, Vector3{ 0.0f, -1.0f,  0.0f}, Vector2{0, 1}, Vector4{ 1.0f,  0.0f,  0.0f,  1.0f}},
            {Vector3{-0.5f, -0.5f,  0.5f}, Vector3{ 0.0f, -1.0f,  0.0f}, Vector2{0, 0}, Vector4{ 1.0f,  0.0f,  0.0f,  1.0f}},
            {Vector3{ 0.5f, -0.5f,  0.5f}, Vector3{ 0.0f, -1.0f,  0.0f}, Vector2{1, 0}, Vector4{ 1.0f,  0.0f,  0.0f,  1.0f}},
            {Vector3{ 0.5f, -0.5f, -0.5f}, Vector3{ 0.0f, -1.0f,  0.0f}, Vector2{1, 1}, Vector4{ 1.0f,  0.0f,  0.0f,  1.0f}},
            // +Y
            {Vector3{-0.5f,  0.5f, -0.5f}, Vector3{ 0.0f,  1.0f,  0.0f}, Vector2{0, 1}, Vector4{ 1.0f,  0.0f,  0.0f,  1.0f}},
            {Vector3{-0.5f,  0.5f,  0.5f}, Vector3{ 0.0f,  1.0f,  0.0f}, Vector2{0, 0}, Vector4{ 1.0f,  0.0f,  0.0f,  1.0f}},
            {Vector3{ 0.5f,  0.5f,  0.5f}, Vector3{ 0.0f,  1.0f,  0.0f}, Vector2{1, 0}, Vector4{ 1.0f,  0.0f,  0.0f,  1.0f}},
            {Vector3{ 0.5f,  0.5f, -0.5f}, Vector3{ 0.0f,  1.0f,  0.0f}, Vector2{1, 1}, Vector4{ 1.0f,  0.0f,  0.0f,  1.0f}},
            //  -Z
            {Vector3{-0.5f, -0.5f, -0.5f}, Vector3{ 0.0f,  0.0f, -1.0f}, Vector2{0, 1}, Vector4{ 1.0f,  0.0f,  0.0f,  1.0f}},
            {Vector3{-0.5f,  0.5f, -0.5f}, Vector3{ 0.0f,  0.0f, -1.0f}, Vector2{0, 0}, Vector4{ 1.0f,  0.0f,  0.0f,  1.0f}},
            {Vector3{ 0.5f,  0.5f, -0.5f}, Vector3{ 0.0f,  0.0f, -1.0f}, Vector2{1, 0}, Vector4{ 1.0f,  0.0f,  0.0f,  1.0f}},
            {Vector3{ 0.5f, -0.5f, -0.5f}, Vector3{ 0.0f,  0.0f, -1.0f}, Vector2{1, 1}, Vector4{ 1.0f,  0.0f,  0.0f,  1.0f}},
            // +Z
            {Vector3{ 0.5f, -0.5f,  0.5f}, Vector3{ 0.0f,  0.0f,  1.0f}, Vector2{0, 1}, Vector4{ 1.0f,  0.0f,  0.0f,  1.0f}},
            {Vector3{ 0.5f,  0.5f,  0.5f}, Vector3{ 0.0f,  0.0f,  1.0f}, Vector2{0, 0}, Vector4{ 1.0f,  0.0f,  0.0f,  1.0f}},
            {Vector3{-0.5f,  0.5f,  0.5f}, Vector3{ 0.0f,  0.0f,  1.0f}, Vector2{1, 0}, Vector4{ 1.0f,  0.0f,  0.0f,  1.0f}},
            {Vector3{-0.5f, -0.5f,  0.5f}, Vector3{ 0.0f,  0.0f,  1.0f}, Vector2{1, 1}, Vector4{ 1.0f,  0.0f,  0.0f,  1.0f}}
        };

        // Create index buffer based on counter-clock wise
        std::vector<uint32_t> Indices =
        {
            //  -X (0 ~ 3)
            0, 2, 1,  2, 0, 3,
            // +X (4 ~ 7)
            4, 6, 5,  6, 4, 7,
            //  -Y (8 ~ 11)
            8, 9, 10,  10, 11, 8,
            // +Y (12 ~ 15)
            12, 14, 13,  14, 12, 15,
            //  -Z (16 ~ 19)
            16, 19, 18,  18, 17, 16,
            // +Z (20 ~ 23)
            20, 22, 21,  22, 20, 23
        };

        PrimitiveMeshData<VertexPNUT> data;
        data.vertices = Vertices;
        data.indices = Indices;
        return data;
	}

	PrimitiveMeshData<VertexPU> PrimitiveMeshFactory::CreateFullScreenQuad()
	{
        // Create quad geometry
        std::vector<VertexPU> Vertices =
        {
            {Vector3{-1.0f, -1.0f,  0.0f}, Vector2{0, 1}},
            {Vector3{-1.0f,  1.0f,  0.0f}, Vector2{0, 0}},
            {Vector3{ 1.0f,  1.0f,  0.0f}, Vector2{1, 0}},
            {Vector3{ 1.0f, -1.0f,  0.0f}, Vector2{1, 1}}
        };
        // Create index buffer based on counter-clock wise
        std::vector<uint32_t> Indices =
        {
            0, 2, 1,
            2, 0, 3
        };

        PrimitiveMeshData<VertexPU> data;
        data.vertices = Vertices;
        data.indices = Indices;
        return data;
	}
}