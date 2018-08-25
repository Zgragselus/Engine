#include "Voxel.hlsli"

cbuffer InputData : register(b0)
{
	unsigned int mVoxelCount;
	unsigned int mLevel;
	unsigned int mDimensions;
}

RWStructuredBuffer<unsigned int> mVoxelPositions : register(u0);
RWStructuredBuffer<unsigned int> mOctreeNodes : register(u1);

[numthreads(1024, 1, 1)]
void OctreeNodeFlag(uint GI : SV_GroupIndex, uint3 DTid : SV_DispatchThreadID)
{
	uint threadId = DTid.x;
	if (threadId >= mVoxelCount)
	{
		return;
	}
	
	uint nodeId = 0;
	unsigned int node = mOctreeNodes.Load(nodeId);
	uint flag = 1;
	uint resolution = mDimensions;
	uint4 nodeMin = uint4(0, 0, 0, 0);
	uint4 nodeMax = uint4(mDimensions, mDimensions, mDimensions, 0);

	uint4 position = UnpackRGB10A2(mVoxelPositions.Load(threadId));

	for (uint i = 0; i < mLevel; i++)
	{
		resolution /= 2;

		if ((node & 0x80000000) == 0)
		{
			flag = 0;
			break;
		}

		uint childOffset = 0;
		uint4 offset = position - nodeMin;
		if (offset.x >= resolution)
		{
			nodeMin.x += resolution;
			childOffset += 1;
		}
		else
		{
			nodeMax.x -= resolution;
		}

		if (offset.y >= resolution)
		{
			nodeMin.y += resolution;
			childOffset += 2;
		}
		else
		{
			nodeMax.y -= resolution;
		}

		if (offset.z >= resolution)
		{
			nodeMin.z += resolution;
			childOffset += 4;
		}
		else
		{
			nodeMax.z -= resolution;
		}

		nodeId = (node & 0x7FFFFFFF) + childOffset;
		node = mOctreeNodes.Load(nodeId);
	}

	if (flag == 1)
	{
		mOctreeNodes[nodeId] = 0x80000000;
	}
}