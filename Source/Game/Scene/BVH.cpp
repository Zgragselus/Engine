#include "BVH.h"
#include <algorithm>

using namespace Engine;

BVH::BVH(BVH::SplitMethod method)
{
	mMethod = method;
}

BVH::~BVH()
{

}

void BVH::Build(std::vector<RenderNode>& nodes)
{
	std::vector<InfoNode> info;
	for (size_t i = 0; i < nodes.size(); i++)
	{
		InfoNode n = InfoNode((int)i, nodes[i].mBounds);
		info.push_back(n);
	}

	mNodes.clear();
	mData.clear();

	mData.reserve(info.size());

	int totalNodes = 0;
	BuildNode* root = RecursiveBuild(nodes, info, 0, (int)info.size(), &totalNodes);

	int offset = 0;
	mNodes.resize(totalNodes);
	FlattenTree(root, &offset);

	RecursiveRelease(root);
}

BVH::BuildNode* BVH::RecursiveBuild(std::vector<RenderNode>& nodes, std::vector<BVH::InfoNode>& primInfo, int start, int end, int* totalNodes)
{
	BuildNode* node = new BuildNode();
	(*totalNodes)++;

	AABB bounds;
	for (int i = start; i < end; i++)
	{
		bounds.Union(primInfo[i].mBounds);
	}

	int numPrims = end - start;
	if (numPrims <= 2)
	{
		int firstPrimOffset = (int)mData.size();
		for (int i = start; i < end; i++)
		{
			mData.push_back(nodes[primInfo[i].mId]);
		}
		node->InitLeaf(firstPrimOffset, numPrims, bounds);
		return node;
	}
	else
	{
		AABB centroidBounds;
		for (int i = start; i < end; i++)
		{
			centroidBounds.Expand(primInfo[i].mCentroid);
		}

		int dim = centroidBounds.MaximumExtent();
		
		int mid = (start + end) / 2;

		if (centroidBounds.mMax[dim] - centroidBounds.mMin[dim] < 0.001f)
		{
			int firstPrimOffset = (int)mData.size();
			for (int i = start; i < end; i++)
			{
				mData.push_back(nodes[primInfo[i].mId]);
			}
			node->InitLeaf(firstPrimOffset, numPrims, bounds);
			return node;
		}
		else
		{
			switch (mMethod)
			{
			case SplitMethod::SPLIT_MIDDLE: {
				float pmid = (centroidBounds.mMin[dim] + centroidBounds.mMax[dim]) / 2.0f;
				InfoNode* midPtr = std::partition(&primInfo[start], &primInfo[end - 1] + 1,
					[dim, pmid](const InfoNode& n) { return n.mCentroid[dim] < pmid; });
				mid = (int)(midPtr - &primInfo[0]);
				if (mid != start && mid != end)
				{
					break;
				}
			}

			case SplitMethod::SPLIT_EQUALCOUNTS:
				mid = (start + end) / 2;
				std::nth_element(&primInfo[start], &primInfo[mid], &primInfo[end - 1] + 1,
					[dim](const InfoNode& a, const InfoNode& b) { return a.mCentroid[dim] < b.mCentroid[dim]; });
				break;

			case SplitMethod::SPLIT_SAH:
				if (numPrims <= 2)
				{
					mid = (start + end) / 2;
					std::nth_element(&primInfo[start], &primInfo[mid], &primInfo[end - 1] + 1,
						[dim](const InfoNode& a, const InfoNode& b) { return a.mCentroid[dim] < b.mCentroid[dim]; });
				}
				else
				{
					const int numBuckets = 12;
					Bucket buckets[numBuckets];

					for (int i = start; i < end; i++)
					{
						int b = (int)(numBuckets * centroidBounds.Offset(primInfo[i].mCentroid)[dim]);
						if (b == numBuckets)
						{
							b = numBuckets - 1;
						}

						buckets[b].mCount++;
						buckets[b].mBounds.Union(primInfo[i].mBounds);
					}

					float cost[numBuckets - 1];
					for (int i = 0; i < numBuckets - 1; i++)
					{
						AABB b0, b1;
						int count0 = 0, count1 = 0;

						for (int j = 0; j < i; j++)
						{
							b0.Union(buckets[j].mBounds);
							count0 += buckets[j].mCount;
						}

						for (int j = i + 1; j < numBuckets; j++)
						{
							b1.Union(buckets[j].mBounds);
							count1 += buckets[j].mCount;
						}

						cost[i] = 1.0f + (count0 * b0.SurfaceArea() + count1 * b1.SurfaceArea());
					}

					float minCost = cost[0];
					int minSplitBucket = 0;
					for (int i = 1; i < numBuckets - 1; i++)
					{
						if (cost[i] < minCost)
						{
							minCost = cost[i];
							minSplitBucket = i;
						}
					}

					float leafCost = (float)numPrims;
					if (numPrims > 2 || minCost < leafCost)
					{
						InfoNode* midPtr = std::partition(&primInfo[start], &primInfo[end - 1] + 1,
							[numBuckets, centroidBounds, minSplitBucket, dim](const InfoNode& n)
							{
								int b = (int)(numBuckets * centroidBounds.Offset(n.mCentroid)[dim]);
								if (b == numBuckets)
								{
									b = numBuckets - 1;
								}
								return b < minSplitBucket;
							});
					}
				}
				break;
			}

			node->InitInterior(dim,
				RecursiveBuild(nodes, primInfo, start, mid, totalNodes),
				RecursiveBuild(nodes, primInfo, mid, end, totalNodes));
		}
	}

	return node;
}

void BVH::RecursiveRelease(BuildNode* node)
{
	if (node->mChildren[0])
	{
		RecursiveRelease(node->mChildren[0]);
	}

	if (node->mChildren[1])
	{
		RecursiveRelease(node->mChildren[1]);
	}

	delete node;
}

int BVH::FlattenTree(BuildNode* node, int* offset)
{
	Node* linearNode = &mNodes[*offset];
	linearNode->mBounds = node->mBounds;

	int temp = (*offset)++;

	if (node->mNumPrimitives > 0)
	{
		linearNode->mPrimitiveOffset = node->mFirstPrimOffset;
		linearNode->mNumPrimitives = node->mNumPrimitives;
	}
	else
	{
		linearNode->mNumPrimitives = 0;
		FlattenTree(node->mChildren[0], offset);
		linearNode->mSecondChildOffset = FlattenTree(node->mChildren[1], offset);
	}

	return temp;
}

std::vector<RenderNode> BVH::Render(Camera* camera)
{
	std::vector<RenderNode> result;
	Frustum* f = camera->GetFrustum();

	int node = 0;
	int stackPos = 0;
	int nodes[64] = { 0 };

	while (true)
	{
		Node& n = mNodes[node];
		if (f->TestAABB(n.mBounds) != Frustum::OUTSIDE)
		{
			if (n.mNumPrimitives > 0)
			{
				for (int i = n.mPrimitiveOffset; i < n.mPrimitiveOffset + n.mNumPrimitives; i++)
				{
					result.push_back(mData[i]);
				}

				if (stackPos > 0)
				{
					stackPos--;
					node = nodes[stackPos];
				}
				else
				{
					break;
				}
			}
			else
			{
				nodes[stackPos] = n.mSecondChildOffset;
				stackPos++;

				node = node + 1;
			}
		}
		else
		{
			if (stackPos > 0)
			{
				stackPos--;
				node = nodes[stackPos];
			}
			else
			{
				break;
			}
		}
	}

	return result;
}

std::vector<RenderNode> BVH::Render(float4 position, float4 direction, float4 up, float fov, float aspect, float nearPlane, float farPlane)
{
	std::vector<RenderNode> result;
	Frustum f;
	f.InitPerspective(position, direction, up, fov, aspect, nearPlane, farPlane);

	int node = 0;
	int stackPos = 0;
	int nodes[64] = { 0 };

	while (true)
	{
		Node& n = mNodes[node];
		if (f.TestAABB(n.mBounds) != Frustum::OUTSIDE)
		{
			if (n.mNumPrimitives > 0)
			{
				for (int i = n.mPrimitiveOffset; i < n.mPrimitiveOffset + n.mNumPrimitives; i++)
				{
					result.push_back(mData[i]);
				}

				if (stackPos > 0)
				{
					stackPos--;
					node = nodes[stackPos];
				}
				else
				{
					break;
				}
			}
			else
			{
				nodes[stackPos] = n.mSecondChildOffset;
				stackPos++;

				node = node + 1;
			}
		}
		else
		{
			if (stackPos > 0)
			{
				stackPos--;
				node = nodes[stackPos];
			}
			else
			{
				break;
			}
		}
	}

	return result;
}