#include "BVHNode.h"
#include "Collider.h"

#include <algorithm>

namespace Engine
{
	namespace BVH
	{
		AABB CreateFatAABB(const AABB& original)
		{
			AABB fatAABB = original;
			fatAABB.extents += FatMargin; // add fat margin to extents
			return fatAABB;
		}

		std::unique_ptr<BVHNode> BuildBVH(std::vector<Collider*>& colliders, int start, int end)
		{
			if(start >= end)
			{
				return nullptr;
			}

			// Create a new BVHNode
			auto node = std::make_unique<BVHNode>();

			// Compute the bounding box for this node and check min/max center of colliders to determine the splitting axis
			AABB bounds = colliders[start]->GetBounds();
			Vector3 minCenter = bounds.center;
			Vector3 maxCenter = bounds.center;
			for (int i = start + 1; i < end; ++i)
			{
				const auto& colliderBounds = colliders[i]->GetBounds();
				bounds.MergeBounds(colliderBounds);
				// Update min/max center for splitting axis selection
				minCenter.x = std::min(minCenter.x, colliderBounds.center.x);
				minCenter.y = std::min(minCenter.y, colliderBounds.center.y);
				minCenter.z = std::min(minCenter.z, colliderBounds.center.z);
				maxCenter.x = std::max(maxCenter.x, colliderBounds.center.x);
				maxCenter.y = std::max(maxCenter.y, colliderBounds.center.y);
				maxCenter.z = std::max(maxCenter.z, colliderBounds.center.z);
			}
			node->bounds = CreateFatAABB(bounds);

			// If this node contains only one collider, it's a leaf node
			int count = end - start;
			if (count == 1)
			{
				node->collider = colliders[start];
				node->collider->bvhNode = node.get(); // set collider's BVH node pointer
				return node;
			}

			// sort colliders by the center of their bounds along the selected axis
			Vector3 extent = maxCenter - minCenter;
			int axis = 0;
			if (extent.y > extent[axis])
			{
				axis = 1;
			}
			if (extent.z > extent[axis])
			{
				axis = 2;
			}

			std::sort(colliders.begin() + start, colliders.begin() + end, [axis](Collider* a, Collider* b)
			{
				return a->GetBounds().center[axis] < b->GetBounds().center[axis];
			});
			
			// build child nodes recursively
			int mid = (start + end) / 2;
			node->left = BuildBVH(colliders, start, mid);
			node->right = BuildBVH(colliders, mid, end);
			// set parent pointers for child nodes
			if(node->left)
			{
				node->left->parent = node.get();
			}
			if(node->right)
			{
				node->right->parent = node.get();
			}

			return node;
		}

		void RefitBVH(BVHNode* node)
		{
			while (node)
			{
				if (!node->IsLeaf())
				{
					node->bounds = AABB::MergeBounds(node->left->bounds, node->right->bounds);
				}
				node = node->parent;
			}
		}

		void RemoveLeaf(std::unique_ptr<BVHNode>& root, BVHNode* leaf)
		{
			// return if leaf is null or has no parent (i.e., it's the root)
			if(leaf == nullptr)
			{
				return;
			}
			
			leaf->collider->bvhNode = nullptr; // clear collider's BVH node pointer

			// if leaf is root, simply remove it
			if (leaf == root.get())
			{
				root.reset();
				return;
			}

			BVHNode* parent = leaf->parent;
			BVHNode* grandParent = parent->parent;
			// get sibling node
			std::unique_ptr<BVHNode> sibling = parent->left.get() == leaf ? std::move(parent->right) : std::move(parent->left);

			// if parent is root, sibling becomes new root
			if (!grandParent)
			{
				sibling->parent = nullptr;
				root = std::move(sibling);
				return;
			}

			auto parentPtr = GetOwnerPtr(parent);
			sibling->parent = grandParent;
			*parentPtr = std::move(sibling);
			 RefitBVH(parentPtr->get());
		}
	}
}