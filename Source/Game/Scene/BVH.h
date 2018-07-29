#ifndef __BVH__H__
#define __BVH__H__

#include "RenderNode.h"
#include "Graphics/Common/Camera/Camera.h"

namespace Engine
{
	class BVH
	{
	public:
		enum class SplitMethod
		{
			SPLIT_EQUALCOUNTS,
			SPLIT_MIDDLE,
			SPLIT_SAH
		};

	private:
		struct __declspec(align(16)) Node
		{
			AABB mBounds;
			union
			{
				int mPrimitiveOffset;
				int mSecondChildOffset;
			};
			int mNumPrimitives;
			int mPad[2];

			ALIGNED_NEW_DELETE("Game::Scene::BVH::Node")
		};

		struct __declspec(align(16)) BuildNode
		{
			AABB mBounds;
			BuildNode* mChildren[2];
			int mSplitAxis;
			int mFirstPrimOffset;
			int mNumPrimitives;
			int mPad;

			void InitLeaf(int first, int n, const AABB& bounds)
			{
				mBounds = bounds;
				mNumPrimitives = n;
				mFirstPrimOffset = first;
				mChildren[0] = mChildren[1] = nullptr;
			}

			void InitInterior(int axis, BuildNode* c0, BuildNode* c1)
			{
				mSplitAxis = axis;
				mBounds = AABB();
				mBounds.Union(c0->mBounds);
				mBounds.Union(c1->mBounds);
				mNumPrimitives = 0;
				mFirstPrimOffset = 0;
				mChildren[0] = c0;
				mChildren[1] = c1;
			}

			ALIGNED_NEW_DELETE("Game::Scene::BVH::BuildNode")
		};

		struct __declspec(align(16)) InfoNode
		{
			AABB mBounds;
			float4 mCentroid;
			int mId;
			int mPad[3];

			InfoNode()
			{

			}

			InfoNode(int id, const AABB& bounds)
			{
				mBounds = bounds;
				mId = id;
				mCentroid = mBounds.mMin * 0.5f + mBounds.mMax * 0.5f;
			}

			ALIGNED_NEW_DELETE("Game::Scene::BVH::InfoNode")
		};

		struct __declspec(align(16)) Bucket
		{
			AABB mBounds;
			int mCount;
			int mPad[3];

			Bucket()
			{
				mBounds = AABB();
				mCount = 0;
			}
		};

		SplitMethod mMethod;
		std::vector<Node> mNodes;
		std::vector<RenderNode> mData;

		BuildNode* RecursiveBuild(std::vector<RenderNode>& nodes, std::vector<InfoNode>& primInfo, int start, int end, int* totalNodes);
		int FlattenTree(BuildNode* node, int* offset);
		void RecursiveRelease(BuildNode* node);

	public:
		BVH(SplitMethod method);
		~BVH();
		void Build(std::vector<RenderNode>& nodes);
		std::vector<RenderNode> Render(Camera* camera);
		std::vector<RenderNode> Render(float4 position, float4 direction, float4 up, float fov, float aspect, float nearPlane, float farPlane);
		const AABB& GetBounds() const
		{
			return mNodes[0].mBounds;
		}

		ALIGNED_NEW_DELETE("Game::Scene::BVH")
	};
}

#endif