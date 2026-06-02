#pragma once

#include "BoundingVolume.h"

#include <memory>
#include <vector>

namespace Engine
{
	namespace BVH
	{
		constexpr float FatMargin = 1.0f;
		struct BVHNode
		{
			// AABB that bounds all colliders in this node and its children
			AABB bounds;

			BVHNode* parent = nullptr;
			std::unique_ptr<BVHNode> left;
			std::unique_ptr<BVHNode> right;

			Collider* collider = nullptr;	// only set for leaf nodes
			bool IsLeaf() const
			{
				return collider != nullptr;
			}
		};
	
		AABB CreateFatAABB(const AABB& original);
		std::unique_ptr<BVHNode> BuildBVH(std::vector<Collider*>& colliders, int start, int end);
		void RefitBVH(BVHNode* node);
		void RemoveLeaf(std::unique_ptr<BVHNode>& root, BVHNode* leaf);

		inline std::unique_ptr<BVHNode>* GetOwnerPtr(BVHNode* node)
		{
			if (node == nullptr || node->parent == nullptr)
			{
				return nullptr; // root node has no owner pointer
			}

			BVHNode* parent = node->parent;
			if (parent->left.get() == node) return &parent->left;
			if (parent->right.get() == node) return &parent->right;

			return nullptr;
		}
	}
}