#pragma once

#include "BoundingVolume.h"

#include <memory>
#include <vector>

namespace Engine
{
	namespace BVH
	{
		inline constexpr float FatMargin = 1.0f;
		inline constexpr int BinCount = 12;
		inline float RotationThreshold = 0.95f;

		// --------------------------------------------------------------------------------------
		//  tree structure example
		//												   A
		//												/	   \
		//											  B       C
		//											/    \	  /    \
		//										  D    E  F    G
		//	if rotation type is BL it means  rotatiting to right relative B and B's right children node insert to A's left children node
		// --------------------------------------------------------------------------------------
		enum class RotationType
		{
			None,
			BL,
			BR,
			CL,
			CR
		};

		struct Bin
		{
			AABB bounds;
			int count = 0;
		};

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
	
		// bvh utility
		AABB CreateFatAABB(const AABB& original);
		void UpdateBounds(BVHNode* node);
		std::unique_ptr<BVHNode> BuildBVH(std::vector<Collider*>& colliders, int start, int end);
		void RefitUpwards(std::unique_ptr<BVHNode>& root, BVHNode* node);	//  refit node's bounds
		void DestroyLeaf(std::unique_ptr<BVHNode>& root, BVHNode* leaf);	// destroy leaf node
		std::unique_ptr<BVHNode> RemoveLeaf(std::unique_ptr<BVHNode>& root, BVHNode* leaf);	// temporarily delete a node
		void InsertLeaf(std::unique_ptr<BVHNode>& root, std::unique_ptr<BVHNode> leaf);	// insert leaf node. Use SAH to find the best insertion location.
		// rotation utility
		BVHNode* Balance(std::unique_ptr<BVHNode>& root, BVHNode* node);	// make tree balanced using tree rotation based on SAH
		// get cost after rotation
		float CostBL(BVHNode* A);
		float CostBR(BVHNode* A);
		float CostCL(BVHNode* A);
		float CostCR(BVHNode* A);
		RotationType FindBestRotation(BVHNode* node);
		// perform rotation
		BVHNode* RotateBL(std::unique_ptr<BVHNode>& root, BVHNode* node);
		BVHNode* RotateBR(std::unique_ptr<BVHNode>& root, BVHNode* node);
		BVHNode* RotateCL(std::unique_ptr<BVHNode>& root, BVHNode* node);
		BVHNode* RotateCR(std::unique_ptr<BVHNode>& root, BVHNode* node);
		// rebuild utility
		float ComputeAreaSum(const BVHNode* node);
		float ComputeAreaRatio(const BVHNode* root);

		inline std::unique_ptr<BVHNode>* GetOwnerPtr(BVHNode* node)
		{
			if (node == nullptr)
			{
				return nullptr; // root node has no owner pointer
			}

			assert(node->parent != nullptr);

			BVHNode* parent = node->parent;
			if (parent->left.get() == node) return &parent->left;
			if (parent->right.get() == node) return &parent->right;

			return nullptr;
		}
	}
}