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
			if (start >= end)
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
			if (count <= 1)
			{
				for (int i = start; i < end; ++i)
				{
					node->collider = colliders[i];
					colliders[i]->bvhNode = node.get(); // set collider's BVH node pointer
				}

				return node;
			}

			// // Longest centroid axis
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
			float minV = minCenter[axis];
			float maxV = maxCenter[axis];

			// for all centroid is same
			if (std::abs(maxV - minV) < 1e-6f)
			{
				int mid = start + count / 2;

				node->left = BuildBVH(colliders, start, mid);
				node->right = BuildBVH(colliders, mid, end);

				if (node->left) node->left->parent = node.get();
				if (node->right) node->right->parent = node.get();

				return node;
			}

			// binned surface area heuristic
			Bin bins[BinCount];
			float inv = (maxV - minV > 0.0f) ? 1.0f / (maxV - minV) : 0.0f;
			// fill bins
			for (int i = start; i < end; ++i)
			{
				auto* c = colliders[i];
				float v = c->GetBounds().center[axis];

				int binIdx = static_cast<int>((v - minV) * inv * BinCount);
				binIdx = std::clamp(binIdx, 0, BinCount - 1);

				if (bins[binIdx].count == 0)
				{
					bins[binIdx].bounds = c->GetBounds();
				}
				else
				{
					bins[binIdx].bounds.MergeBounds(c->GetBounds());
				}

				bins[binIdx].count++;
			}

			float bestCost = FLT_MAX;
			int bestSplit = 0;

			// surface area heuristic
			for (int i = 1; i < BinCount; ++i)
			{
				AABB left, right;
				int leftCount = 0, rightCount = 0;
				bool leftValid = false, rightValid = false;

				for (int j = 0; j < i; ++j)
				{
					if (bins[j].count == 0) continue;
					if (!leftValid)
					{
						left = bins[j].bounds;
						leftValid = true;
					}
					else
					{
						left.MergeBounds(bins[j].bounds);
					}

					leftCount += bins[j].count;
				}

				for (int j = i; j < BinCount; ++j)
				{
					if (bins[j].count == 0) continue;
					if (!rightValid)
					{
						right = bins[j].bounds;
						rightValid = true;
					}
					else
					{
						right.MergeBounds(bins[j].bounds);
					}

					rightCount += bins[j].count;
				}

				if (leftCount == 0 || rightCount == 0) continue;

				float cost = left.SurfaceArea() * leftCount + right.SurfaceArea() * rightCount;

				if (cost < bestCost)
				{
					bestCost = cost;
					bestSplit = i;
				}
			}

			auto midIt = std::partition(colliders.begin() + start, colliders.begin() + end,
				[&](Collider* c)
				{
					float v = c->GetBounds().center[axis];
					int binIdx = std::clamp(static_cast<int>((v - minV) * inv * BinCount), 0, BinCount - 1);
					return binIdx < bestSplit;
				});

			int mid = static_cast<int>(midIt - colliders.begin());

			// block partitionning failed
			if (mid == start || mid == end)
			{
				mid = start + count / 2;
			}

			// build child nodes recursively
			node->left = BuildBVH(colliders, start, mid);
			node->right = BuildBVH(colliders, mid, end);
			// set parent pointers for child nodes
			if (node->left)
			{
				node->left->parent = node.get();
			}
			if (node->right)
			{
				node->right->parent = node.get();
			}

			return node;
		}

		void UpdateBounds(BVHNode* node)
		{
			if (node->IsLeaf()) return;

			node->bounds = AABB::MergeBounds(node->left->bounds, node->right->bounds);
		}

		void RefitUpwards(std::unique_ptr<BVHNode>& root, BVHNode* node)
		{
			while (node)
			{
				UpdateBounds(node);
				node = Balance(root, node);
				node = node->parent;
			}
		}

		void DestroyLeaf(std::unique_ptr<BVHNode>& root, BVHNode* leaf)
		{
			// return if leaf is null or has no parent (i.e., it's the root)
			if (leaf == nullptr)
			{
				return;
			}

			// clear colliders BVH node pointer
			leaf->collider->bvhNode = nullptr;

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

			RefitUpwards(root, grandParent);
		}

		std::unique_ptr<BVHNode> RemoveLeaf(std::unique_ptr<BVHNode>& root, BVHNode* leaf)
		{
			// if node is unvalid, return nullptr
			if (leaf == nullptr)
			{
				return nullptr;
			}
			// if node is root, move root ownership
			if (root.get() == leaf)
			{
				auto result = std::move(root);
				return result;
			}

			BVHNode* parent = leaf->parent;
			BVHNode* grandParent = parent->parent;
			// get node's owner
			std::unique_ptr<BVHNode> siblingPtr;
			std::unique_ptr<BVHNode> leafPtr;
			if (parent->left.get() == leaf)
			{
				leafPtr = std::move(parent->left);
				siblingPtr = std::move(parent->right);
			}
			else
			{
				leafPtr = std::move(parent->right);
				siblingPtr = std::move(parent->left);
			}

			// if parent is root, sibling becomes new root
			if (!grandParent)
			{
				siblingPtr->parent = nullptr;
				root = std::move(siblingPtr);
			}
			else
			{
				auto parentPtr = GetOwnerPtr(parent);
				siblingPtr->parent = grandParent;
				*parentPtr = std::move(siblingPtr);
				RefitUpwards(root, grandParent);
			}

			leafPtr->parent = nullptr;
			return leafPtr;
		}

		void InsertLeaf(std::unique_ptr<BVHNode>& root, std::unique_ptr<BVHNode> leaf)
		{
			// if root is null, leaf is new root
			if (!root)
			{
				root = std::move(leaf);
				root->parent = nullptr;
				return;
			}

			// It is the sibling of the location where the leaf will be inserted
			BVHNode* sibling = root.get();
			// find best location based on SAH
			while (!sibling->IsLeaf())
			{
				float leftCost = AABB::MergeBounds(sibling->left->bounds, leaf->bounds).SurfaceArea() - sibling->left->bounds.SurfaceArea();
				float rightCost = AABB::MergeBounds(sibling->right->bounds, leaf->bounds).SurfaceArea() - sibling->right->bounds.SurfaceArea();
				sibling = leftCost < rightCost ? sibling->left.get() : sibling->right.get();
			}

			// create new parent node
			auto newParent = std::make_unique<BVHNode>();
			newParent->bounds = AABB::MergeBounds(sibling->bounds, leaf->bounds);

			// if sibling is root, newParent is new root
			BVHNode* oldParent = sibling->parent;
			if (!oldParent)
			{
				leaf->parent = newParent.get();
				root->parent = newParent.get();

				newParent->parent = nullptr;
				newParent->left = std::move(leaf);
				newParent->right = std::move(root);

				root = std::move(newParent);

				RefitUpwards(root, root.get());
			}
			else
			{
				auto siblingPtr = GetOwnerPtr(sibling);	// get sibling owner ptr before changing sibling's parent

				leaf->parent = newParent.get();
				sibling->parent = newParent.get();

				newParent->parent = oldParent;
				newParent->left = std::move(leaf);
				newParent->right = std::move(*siblingPtr);

				*siblingPtr = std::move(newParent);

				RefitUpwards(root, oldParent);
			}
		}

		BVHNode* Balance(std::unique_ptr<BVHNode>& root, BVHNode* node)
		{
			static int g_rotationCount = 0;
			RotationType rot = FindBestRotation(node);
			switch (rot)
			{
				case RotationType::BL:
				{
					// LOG_INFO("rotation Count : %d", ++g_rotationCount);  
					return RotateBL(root, node);
				}
				case RotationType::BR:
				{
					// LOG_INFO("rotation Count : %d", ++g_rotationCount); 
					return RotateBR(root, node);
				}
				case RotationType::CL:
				{
					// LOG_INFO("rotation Count : %d", ++g_rotationCount);
					return RotateCL(root, node);
				}
				case RotationType::CR:
				{
					// LOG_INFO("rotation Count : %d", ++g_rotationCount); 
					return RotateCR(root, node);
				}
				default: return node;
			}
		}

		BVHNode* RotateBL(std::unique_ptr<BVHNode>& root, BVHNode* node)
		{
			BVHNode* parent = node->parent;
			auto owner = (node == root.get()) ? &root : GetOwnerPtr(node);

			// get nodes owner
			auto aPtr = std::move(*owner);

			auto bPtr = std::move(aPtr->left);
			auto cPtr = std::move(aPtr->right);

			auto dPtr = std::move(bPtr->left);
			auto ePtr = std::move(bPtr->right);

			// rotate
			aPtr->left = std::move(ePtr);
			aPtr->right = std::move(cPtr);

			aPtr->left->parent = aPtr.get();
			aPtr->right->parent = aPtr.get();

			bPtr->left = std::move(dPtr);
			bPtr->right = std::move(aPtr);

			bPtr->left->parent = bPtr.get();
			bPtr->right->parent = bPtr.get();

			// set new A node
			bPtr->parent = parent;
			*owner = std::move(bPtr);

			// update bounds
			BVHNode* newSubtreeRoot = owner->get();
			UpdateBounds(newSubtreeRoot->right.get());
			UpdateBounds(newSubtreeRoot);

			return newSubtreeRoot;
		}
		BVHNode* RotateBR(std::unique_ptr<BVHNode>& root, BVHNode* node)
		{
			BVHNode* parent = node->parent;
			auto owner = (node == root.get()) ? &root : GetOwnerPtr(node);

			// get nodes owner
			auto aPtr = std::move(*owner);

			auto bPtr = std::move(aPtr->left);
			auto cPtr = std::move(aPtr->right);

			auto dPtr = std::move(bPtr->left);
			auto ePtr = std::move(bPtr->right);

			// rotate
			aPtr->left = std::move(dPtr);
			aPtr->right = std::move(cPtr);

			aPtr->left->parent = aPtr.get();
			aPtr->right->parent = aPtr.get();

			bPtr->left = std::move(ePtr);
			bPtr->right = std::move(aPtr);

			bPtr->left->parent = bPtr.get();
			bPtr->right->parent = bPtr.get();

			// set new A node
			bPtr->parent = parent;
			*owner = std::move(bPtr);

			// update bounds
			BVHNode* newSubtreeRoot = owner->get();
			UpdateBounds(newSubtreeRoot->right.get());
			UpdateBounds(newSubtreeRoot);

			return newSubtreeRoot;
		}
		BVHNode* RotateCL(std::unique_ptr<BVHNode>& root, BVHNode* node)
		{
			BVHNode* parent = node->parent;
			auto owner = (node == root.get()) ? &root : GetOwnerPtr(node);

			// get nodes owner
			auto aPtr = std::move(*owner);

			auto bPtr = std::move(aPtr->left);
			auto cPtr = std::move(aPtr->right);

			auto fPtr = std::move(cPtr->left);
			auto gPtr = std::move(cPtr->right);

			// rotate
			aPtr->left = std::move(bPtr);
			aPtr->right = std::move(gPtr);

			aPtr->left->parent = aPtr.get();
			aPtr->right->parent = aPtr.get();

			cPtr->left = std::move(aPtr);
			cPtr->right = std::move(fPtr);

			cPtr->left->parent = cPtr.get();
			cPtr->right->parent = cPtr.get();

			// set new A node
			cPtr->parent = parent;
			*owner = std::move(cPtr);

			// update bounds
			BVHNode* newSubtreeRoot = owner->get();
			UpdateBounds(newSubtreeRoot->left.get());
			UpdateBounds(newSubtreeRoot);

			return newSubtreeRoot;
		}
		BVHNode* RotateCR(std::unique_ptr<BVHNode>& root, BVHNode* node)
		{
			BVHNode* parent = node->parent;
			auto owner = (node == root.get()) ? &root : GetOwnerPtr(node);

			// get nodes owner
			auto aPtr = std::move(*owner);

			auto bPtr = std::move(aPtr->left);
			auto cPtr = std::move(aPtr->right);

			auto fPtr = std::move(cPtr->left);
			auto gPtr = std::move(cPtr->right);

			// rotate
			aPtr->left = std::move(bPtr);
			aPtr->right = std::move(fPtr);

			aPtr->left->parent = aPtr.get();
			aPtr->right->parent = aPtr.get();

			cPtr->left = std::move(aPtr);
			cPtr->right = std::move(gPtr);

			cPtr->left->parent = cPtr.get();
			cPtr->right->parent = cPtr.get();

			// set new A node
			cPtr->parent = parent;
			*owner = std::move(cPtr);

			// update bounds
			BVHNode* newSubtreeRoot = owner->get();
			UpdateBounds(newSubtreeRoot->left.get());
			UpdateBounds(newSubtreeRoot);

			return newSubtreeRoot;
		}

		RotationType FindBestRotation(BVHNode* node)
		{
			if (node->IsLeaf()) return RotationType::None;
			// Internal BVH nodes always have two children.
			assert(node->left);
			assert(node->right);

			float bestCost = FLT_MAX;
			RotationType bestRotation = RotationType::None;

			float nodeCost = node->bounds.SurfaceArea();
			// Function for verifying best cost
			auto TestRotation = [&](float currentCost, float newCost, RotationType type)
				{
					if (newCost < currentCost * RotationThreshold && newCost < bestCost)
					{
						bestCost = newCost;
						bestRotation = type;
					}
				};
			if (!node->left->IsLeaf())
			{
				// curent cost for BL, BR
				float leftCost = nodeCost + node->left->bounds.SurfaceArea();
				// check cost
				TestRotation(leftCost, CostBL(node), RotationType::BL);	// BL
				TestRotation(leftCost, CostBR(node), RotationType::BR);	// BR
			}

			if (!node->right->IsLeaf())
			{
				float rightCost = nodeCost + node->right->bounds.SurfaceArea();
				// check cost
				TestRotation(rightCost, CostCL(node), RotationType::CL);	// CL
				TestRotation(rightCost, CostCR(node), RotationType::CR);	// CR
			}

			return bestRotation;
		}

		float CostBL(BVHNode* A)
		{
			assert(!A->left->IsLeaf());

			auto B = A->left.get();
			auto C = A->right.get();
			auto D = B->left.get();
			auto E = B->right.get();

			AABB newA = AABB::MergeBounds(E->bounds, C->bounds);
			AABB newRoot = AABB::MergeBounds(D->bounds, newA);
			return newRoot.SurfaceArea() + newA.SurfaceArea();
		}
		float CostBR(BVHNode* A)
		{
			assert(!A->left->IsLeaf());

			auto B = A->left.get();
			auto C = A->right.get();
			auto D = B->left.get();
			auto E = B->right.get();

			AABB newA = AABB::MergeBounds(D->bounds, C->bounds);
			AABB newRoot = AABB::MergeBounds(E->bounds, newA);
			return newRoot.SurfaceArea() + newA.SurfaceArea();
		}
		float CostCL(BVHNode* A)
		{
			assert(!A->right->IsLeaf());

			auto B = A->left.get();
			auto C = A->right.get();
			auto F = C->left.get();
			auto G = C->right.get();

			AABB newA = AABB::MergeBounds(G->bounds, B->bounds);
			AABB newRoot = AABB::MergeBounds(F->bounds, newA);
			return newRoot.SurfaceArea() + newA.SurfaceArea();
		}
		float CostCR(BVHNode* A)
		{
			assert(!A->right->IsLeaf());

			auto B = A->left.get();
			auto C = A->right.get();
			auto F = C->left.get();
			auto G = C->right.get();

			AABB newA = AABB::MergeBounds(F->bounds, B->bounds);
			AABB newRoot = AABB::MergeBounds(G->bounds, newA);
			return newRoot.SurfaceArea() + newA.SurfaceArea();
		}
		
		float ComputeAreaSum(const BVHNode* node)
		{
			if (!node) return 0.0f;

			float area = node->bounds.SurfaceArea();

			area += ComputeAreaSum(node->left.get());
			area += ComputeAreaSum(node->right.get());

			return area;
		}

		float ComputeAreaRatio(const BVHNode* root)
		{
			if (!root) return 0.0f;

			float totalArea = ComputeAreaSum(root);
			return totalArea / root->bounds.SurfaceArea();
		}
	}
}