#include "Voxel.hlsli"

cbuffer InputData : register(b0)
{
	unsigned int mItems;
	unsigned int mLevel;
	unsigned int mDimensions;
}

StructuredBuffer<unsigned int> mVoxelPositions : register(t0);
StructuredBuffer<unsigned int> mOctreeNodes : register(t1);
RWTexture3D<float4> mOctreeData : register(u0);

uint GetOctreeNode(int4 position)
{
	uint nodeId = 0;
	unsigned int node = mOctreeNodes[nodeId];
	uint flag = 1;
	uint resolution = mDimensions;
	uint4 nodeMin = uint4(0, 0, 0, 0);
	uint4 nodeMax = uint4(mDimensions, mDimensions, mDimensions, 0);

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
		node = mOctreeNodes[nodeId];
	}

	if (flag == 0)
	{
		return uint(-1);
	}
	else
	{
		return nodeId;
	}
}

[numthreads(256, 1, 1)]
void OctreeDataInterior(uint GI : SV_GroupIndex, uint3 DTid : SV_DispatchThreadID)
{
	uint threadId = DTid.x;
	if (threadId >= mItems)
	{
		return;
	}

	uint4 position = UnpackRGB10A2(mVoxelPositions[threadId]);
	uint baseNode = GetOctreeNode(position);

	if (baseNode == uint(-1))
	{
		return;
	}

	uint firstChild = mOctreeNodes[baseNode];

	if ((firstChild & 0x80000000) == 0)
	{
		return;
	}

	firstChild = firstChild & 0x7FFFFFFF;

	int3 node = VoxelToTexture(baseNode);
	int3 child0 = VoxelToTexture(firstChild);
	int3 child1 = VoxelToTexture(firstChild + 1);
	int3 child2 = VoxelToTexture(firstChild + 2);
	int3 child3 = VoxelToTexture(firstChild + 3);
	int3 child4 = VoxelToTexture(firstChild + 4);
	int3 child5 = VoxelToTexture(firstChild + 5);
	int3 child6 = VoxelToTexture(firstChild + 6);
	int3 child7 = VoxelToTexture(firstChild + 7);

	mOctreeData[node] = (mOctreeData[child0] + mOctreeData[child0 + int3(1, 0, 0)] + mOctreeData[child1] +
		mOctreeData[child0 + int3(0, 1, 0)] + mOctreeData[child0 + int3(1, 1, 0)] + mOctreeData[child1 + int3(0, 1, 0)] +
		mOctreeData[child2] + mOctreeData[child2 + int3(1, 0, 0)] + mOctreeData[child3] +
		mOctreeData[child0 + int3(0, 1, 1)] + mOctreeData[child0 + int3(1, 0, 1)] + mOctreeData[child1 + int3(0, 0, 1)] +
		mOctreeData[child0 + int3(0, 1, 1)] + mOctreeData[child0 + int3(1, 1, 1)] + mOctreeData[child1 + int3(0, 1, 1)] +
		mOctreeData[child2 + int3(0, 0, 1)] + mOctreeData[child2 + int3(1, 0, 1)] + mOctreeData[child3 + int3(0, 0, 1)] +
		mOctreeData[child4] + mOctreeData[child4 + int3(1, 0, 0)] + mOctreeData[child5] +
		mOctreeData[child4 + int3(0, 1, 0)] + mOctreeData[child4 + int3(1, 1, 0)] + mOctreeData[child5 + int3(0, 1, 0)] +
		mOctreeData[child6] + mOctreeData[child6 + int3(1, 0, 0)] + mOctreeData[child7]) / 27.0f;

	mOctreeData[node + int3(-1, 0, 0)] = (mOctreeData[child0 + int3(-1, 0, 0)] + mOctreeData[child0 + int3(-1, 1, 0)] + mOctreeData[child0 + int3(-1, 0, 1)] + mOctreeData[child0 + int3(-1, 1, 1)] +
		mOctreeData[child2 + int3(-1, 0, 0)] + mOctreeData[child2 + int3(-1, 0, 1)] +
		mOctreeData[child4 + int3(-1, 0, 0)] + mOctreeData[child4 + int3(-1, 1, 0)] +
		mOctreeData[child6 + int3(-1, 0, 0)]) / 9.0f;

	mOctreeData[node + int3(1, 0, 0)] = (mOctreeData[child1 + int3(1, 0, 0)] + mOctreeData[child1 + int3(1, 1, 0)] + mOctreeData[child1 + int3(1, 0, 1)] + mOctreeData[child1 + int3(1, 1, 1)] +
		mOctreeData[child3 + int3(1, 0, 0)] + mOctreeData[child3 + int3(1, 0, 1)] +
		mOctreeData[child5 + int3(1, 0, 0)] + mOctreeData[child5 + int3(1, 1, 0)] +
		mOctreeData[child7 + int3(1, 0, 0)]) / 9.0f;

	mOctreeData[node + int3(0, -1, 0)] = (mOctreeData[child0 + int3(0, -1, 0)] + mOctreeData[child0 + int3(1, -1, 0)] + mOctreeData[child0 + int3(0, -1, 1)] + mOctreeData[child0 + int3(1, -1, 1)] +
		mOctreeData[child1 + int3(0, -1, 0)] + mOctreeData[child1 + int3(0, -1, 1)] + 
		mOctreeData[child4 + int3(0, -1, 0)] + mOctreeData[child4 + int3(1, -1, 0)] + 
		mOctreeData[child5 + int3(0, -1, 0)]) / 9.0f;

	mOctreeData[node + int3(0, 1, 0)] = (mOctreeData[child2 + int3(0, 1, 0)] + mOctreeData[child2 + int3(1, 1, 0)] + mOctreeData[child2 + int3(0, 1, 1)] + mOctreeData[child2 + int3(1, 1, 1)] +
		mOctreeData[child3 + int3(0, 1, 0)] + mOctreeData[child3 + int3(0, 1, 1)] +
		mOctreeData[child6 + int3(0, 1, 0)] + mOctreeData[child6 + int3(1, 1, 0)] +
		mOctreeData[child7 + int3(0, 1, 0)]) / 9.0f;

	mOctreeData[node + int3(-1, -1, 0)] = (mOctreeData[child0 + int3(-1, -1, 0)] + mOctreeData[child0 + int3(-1, -1, 1)] + mOctreeData[child4 + int3(-1, -1, 0)]) / 3.0f;
	mOctreeData[node + int3(0, -1, -1)] = (mOctreeData[child0 + int3(0, -1, -1)] + mOctreeData[child0 + int3(1, -1, -1)] + mOctreeData[child1 + int3(0, -1, -1)]) / 3.0f;
	mOctreeData[node + int3(1, -1, 0)] = (mOctreeData[child1 + int3(1, -1, 0)] + mOctreeData[child1 + int3(1, -1, 1)] + mOctreeData[child5 + int3(1, -1, 0)]) / 3.0f;
	mOctreeData[node + int3(0, -1, 1)] = (mOctreeData[child4 + int3(0, -1, 1)] + mOctreeData[child4 + int3(1, -1, 1)] + mOctreeData[child5 + int3(0, -1, 1)]) / 3.0f;

	mOctreeData[node + int3(-1, 1, 0)] = (mOctreeData[child2 + int3(-1, 1, 0)] + mOctreeData[child2 + int3(-1, 1, 1)] + mOctreeData[child6 + int3(-1, 1, 0)]) / 3.0f;
	mOctreeData[node + int3(0, 1, -1)] = (mOctreeData[child2 + int3(0, 1, -1)] + mOctreeData[child2 + int3(1, 1, -1)] + mOctreeData[child3 + int3(0, 1, -1)]) / 3.0f;
	mOctreeData[node + int3(1, 1, 0)] = (mOctreeData[child3 + int3(1, 1, 0)] + mOctreeData[child3 + int3(1, 1, 1)] + mOctreeData[child7 + int3(1, 1, 0)]) / 3.0f;
	mOctreeData[node + int3(0, 1, 1)] = (mOctreeData[child6 + int3(0, 1, 1)] + mOctreeData[child6 + int3(1, 1, 1)] + mOctreeData[child7 + int3(0, 1, 1)]) / 3.0f;

	mOctreeData[node + int3(-1, 0, -1)] = (mOctreeData[child0 + int3(-1, 0, -1)] + mOctreeData[child0 + int3(-1, 1, -1)] + mOctreeData[child2 + int3(-1, 0, -1)]) / 3.0f;
	mOctreeData[node + int3(1, 0, -1)] = (mOctreeData[child1 + int3(1, 0, -1)] + mOctreeData[child1 + int3(1, 1, -1)] + mOctreeData[child3 + int3(1, 0, -1)]) / 3.0f;
	mOctreeData[node + int3(-1, 0, 1)] = (mOctreeData[child4 + int3(-1, 0, 1)] + mOctreeData[child4 + int3(-1, 1, 1)] + mOctreeData[child6 + int3(-1, 0, 1)]) / 3.0f;
	mOctreeData[node + int3(1, 0, 1)] = (mOctreeData[child5 + int3(1, 0, 1)] + mOctreeData[child5 + int3(1, 1, 1)] + mOctreeData[child7 + int3(1, 0, 1)]) / 3.0f;

	mOctreeData[node + int3(-1, -1, -1)] = mOctreeData[child0 + int3(-1, -1, -1)];
	mOctreeData[node + int3(1, -1, -1)] = mOctreeData[child1 + int3(1, -1, -1)];
	mOctreeData[node + int3(-1, -1, 1)] = mOctreeData[child4 + int3(-1, -1, 1)];
	mOctreeData[node + int3(1, -1, 1)] = mOctreeData[child5 + int3(1, -1, 1)];
	mOctreeData[node + int3(-1, 1, -1)] = mOctreeData[child2 + int3(-1, 1, -1)];
	mOctreeData[node + int3(1, 1, -1)] = mOctreeData[child3 + int3(1, 1, -1)];
	mOctreeData[node + int3(-1, 1, 1)] = mOctreeData[child6 + int3(-1, 1, 1)];
	mOctreeData[node + int3(1, 1, 1)] = mOctreeData[child7 + int3(1, 1, 1)];

	/*mOctreeData[node + int3(-1, -1, -1)] = 0.125f * (mOctreeData[child0 + int3(-1, -1, -1)] + mOctreeData[child0 + int3(0, -1, -1)] + mOctreeData[child0 + int3(-1, 0, -1)] + mOctreeData[child0 + int3(0, 0, -1)] +
		mOctreeData[child0 + int3(-1, -1, 0)] + mOctreeData[child0 + int3(0, -1, 0)] + mOctreeData[child0 + int3(-1, 0, 0)] + mOctreeData[child0 + int3(0, 0, 0)]);
	mOctreeData[node + int3(1, -1, -1)] = 0.125f * (mOctreeData[child1 + int3(0, -1, -1)] + mOctreeData[child1 + int3(1, -1, -1)] + mOctreeData[child1 + int3(0, 0, -1)] + mOctreeData[child1 + int3(1, 0, -1)] +
		mOctreeData[child1 + int3(0, -1, 0)] + mOctreeData[child1 + int3(1, -1, 0)] + mOctreeData[child1 + int3(0, 0, 0)] + mOctreeData[child1 + int3(1, 0, 0)]);
	mOctreeData[node + int3(-1, 1, -1)] = 0.125f * (mOctreeData[child2 + int3(-1, 0, -1)] + mOctreeData[child2 + int3(0, 0, -1)] + mOctreeData[child2 + int3(-1, 1, -1)] + mOctreeData[child2 + int3(0, 1, -1)] +
		mOctreeData[child2 + int3(-1, 0, 0)] + mOctreeData[child2 + int3(0, 0, 0)] + mOctreeData[child2 + int3(-1, 1, 0)] + mOctreeData[child2 + int3(0, 1, 0)]);
	mOctreeData[node + int3(1, 1, -1)] = 0.125f * (mOctreeData[child3 + int3(0, 0, -1)] + mOctreeData[child3 + int3(1, 0, -1)] + mOctreeData[child3 + int3(0, 1, -1)] + mOctreeData[child3 + int3(1, 1, -1)] +
		mOctreeData[child3 + int3(0, 0, 0)] + mOctreeData[child3 + int3(1, 0, 0)] + mOctreeData[child3 + int3(0, 1, 0)] + mOctreeData[child3 + int3(1, 1, 0)]);
	mOctreeData[node + int3(-1, -1, 1)] = 0.125f * (mOctreeData[child4 + int3(-1, -1, 0)] + mOctreeData[child4 + int3(0, -1, 0)] + mOctreeData[child4 + int3(-1, 0, 0)] + mOctreeData[child4 + int3(0, 0, 0)] +
		mOctreeData[child4 + int3(-1, -1, 1)] + mOctreeData[child4 + int3(0, -1, 1)] + mOctreeData[child4 + int3(-1, 0, 1)] + mOctreeData[child4 + int3(0, 0, 1)]);
	mOctreeData[node + int3(1, -1, 1)] = 0.125f * (mOctreeData[child5 + int3(0, -1, 0)] + mOctreeData[child5 + int3(1, -1, 0)] + mOctreeData[child5 + int3(0, 0, 0)] + mOctreeData[child5 + int3(1, 0, 0)] +
		mOctreeData[child5 + int3(0, -1, 1)] + mOctreeData[child5 + int3(1, -1, 1)] + mOctreeData[child5 + int3(0, 0, 1)] + mOctreeData[child5 + int3(1, 0, 1)]);
	mOctreeData[node + int3(-1, 1, 1)] = 0.125f * (mOctreeData[child6 + int3(-1, 0, 0)] + mOctreeData[child6 + int3(0, 0, 0)] + mOctreeData[child6 + int3(-1, 1, 0)] + mOctreeData[child6 + int3(0, 1, 0)] +
		mOctreeData[child6 + int3(-1, 0, 1)] + mOctreeData[child6 + int3(0, 0, 1)] + mOctreeData[child6 + int3(-1, 1, 1)] + mOctreeData[child6 + int3(0, 1, 1)]);
	mOctreeData[node + int3(1, 1, 1)] = 0.125f * (mOctreeData[child7 + int3(0, 0, 0)] + mOctreeData[child7 + int3(1, 0, 0)] + mOctreeData[child7 + int3(0, 1, 0)] + mOctreeData[child7 + int3(1, 1, 0)] +
		mOctreeData[child7 + int3(0, 0, 1)] + mOctreeData[child7 + int3(1, 0, 1)] + mOctreeData[child7 + int3(0, 1, 1)] + mOctreeData[child7 + int3(1, 1, 1)]);

	mOctreeData[node + int3(0, -1, -1)] = 0.125f * (mOctreeData[child0 + int3(1, -1, -1)] + mOctreeData[child0 + int3(1, 0, -1)] + mOctreeData[child0 + int3(1, -1, 0)] + mOctreeData[child0 + int3(1, 0, 0)] + 
		mOctreeData[child1 + int3(-1, -1, -1)] + mOctreeData[child1 + int3(-1, 0, -1)] + mOctreeData[child1 + int3(-1, -1, 0)] + mOctreeData[child1 + int3(-1, 0, 0)]);
	mOctreeData[node + int3(-1, 0, -1)] = 0.125f * (mOctreeData[child0 + int3(-1, 1, -1)] + mOctreeData[child0 + int3(0, 1, -1)] + mOctreeData[child0 + int3(-1, 1, 0)] + mOctreeData[child0 + int3(0, 1, 0)] +
		mOctreeData[child2 + int3(-1, -1, -1)] + mOctreeData[child2 + int3(0, -1, -1)] + mOctreeData[child2 + int3(-1, -1, 0)] + mOctreeData[child2 + int3(0, -1, 0)]);
	mOctreeData[node + int3(1, 0, -1)] = 0.125f * (mOctreeData[child1 + int3(0, 1, -1)] + mOctreeData[child1 + int3(1, 1, -1)] + mOctreeData[child1 + int3(0, 1, 0)] + mOctreeData[child1 + int3(1, 1, 0)] +
		mOctreeData[child3 + int3(0, -1, -1)] + mOctreeData[child3 + int3(1, -1, -1)] + mOctreeData[child3 + int3(0, -1, 0)] + mOctreeData[child3 + int3(1, -1, 0)]);
	mOctreeData[node + int3(0, 1, -1)] = 0.125f * (mOctreeData[child2 + int3(1, 0, -1)] + mOctreeData[child2 + int3(1, 1, -1)] + mOctreeData[child2 + int3(1, 0, 0)] + mOctreeData[child2 + int3(1, 1, 0)] +
		mOctreeData[child3 + int3(-1, 0, -1)] + mOctreeData[child3 + int3(-1, 1, -1)] + mOctreeData[child3 + int3(-1, 0, 0)] + mOctreeData[child3 + int3(-1, 1, 0)]);
	
	mOctreeData[node + int3(0, -1, 1)] = 0.125f * (mOctreeData[child4 + int3(1, -1, 0)] + mOctreeData[child4 + int3(1, 0, 0)] + mOctreeData[child4 + int3(1, -1, 1)] + mOctreeData[child4 + int3(1, 0, 1)] +
		mOctreeData[child5 + int3(-1, -1, 0)] + mOctreeData[child5 + int3(-1, 0, 0)] + mOctreeData[child5 + int3(-1, -1, 1)] + mOctreeData[child5 + int3(-1, 0, 1)]);
	mOctreeData[node + int3(-1, 0, 1)] = 0.125f * (mOctreeData[child4 + int3(-1, 1, 0)] + mOctreeData[child4 + int3(0, 1, 0)] + mOctreeData[child4 + int3(-1, 1, 1)] + mOctreeData[child4 + int3(0, 1, 1)] +
		mOctreeData[child6 + int3(-1, -1, 0)] + mOctreeData[child6 + int3(0, -1, 0)] + mOctreeData[child6 + int3(-1, -1, 1)] + mOctreeData[child6 + int3(0, -1, 1)]);
	mOctreeData[node + int3(1, 0, 1)] = 0.125f * (mOctreeData[child5 + int3(0, 1, 0)] + mOctreeData[child5 + int3(1, 1, 0)] + mOctreeData[child5 + int3(0, 1, 1)] + mOctreeData[child5 + int3(1, 1, 1)] +
		mOctreeData[child7 + int3(0, -1, 0)] + mOctreeData[child7 + int3(1, -1, 0)] + mOctreeData[child7 + int3(0, -1, 1)] + mOctreeData[child7 + int3(1, -1, 1)]);
	mOctreeData[node + int3(0, 1, 1)] = 0.125f * (mOctreeData[child6 + int3(1, 0, 0)] + mOctreeData[child6 + int3(1, 1, 0)] + mOctreeData[child6 + int3(1, 0, 1)] + mOctreeData[child6 + int3(1, 1, 1)] +
		mOctreeData[child7 + int3(-1, 0, 0)] + mOctreeData[child7 + int3(-1, 1, 0)] + mOctreeData[child7 + int3(-1, 0, 1)] + mOctreeData[child7 + int3(-1, 1, 1)]);

	mOctreeData[node + int3(-1, -1, 0)] = 0.125f * (mOctreeData[child0 + int3(-1, -1, 1)] + mOctreeData[child0 + int3(0, -1, 1)] + mOctreeData[child0 + int3(-1, 0, 1)] + mOctreeData[child0 + int3(0, 0, 1)] +
		mOctreeData[child4 + int3(-1, -1, -1)] + mOctreeData[child4 + int3(0, -1, -1)] + mOctreeData[child4 + int3(-1, 0, -1)] + mOctreeData[child4 + int3(0, 0, -1)]);
	mOctreeData[node + int3(1, -1, 0)] = 0.125f * (mOctreeData[child1 + int3(0, -1, 1)] + mOctreeData[child1 + int3(1, -1, 1)] + mOctreeData[child1 + int3(0, 0, 1)] + mOctreeData[child1 + int3(1, 0, 1)] +
		mOctreeData[child5 + int3(0, -1, -1)] + mOctreeData[child5 + int3(1, -1, -1)] + mOctreeData[child5 + int3(0, 0, -1)] + mOctreeData[child5 + int3(1, 0, -1)]);
	mOctreeData[node + int3(-1, 1, 0)] = 0.125f * (mOctreeData[child2 + int3(-1, 0, 1)] + mOctreeData[child2 + int3(0, 0, 1)] + mOctreeData[child2 + int3(-1, 1, 1)] + mOctreeData[child2 + int3(0, 1, 1)] +
		mOctreeData[child6 + int3(-1, 0, -1)] + mOctreeData[child6 + int3(0, 0, -1)] + mOctreeData[child6 + int3(-1, 1, -1)] + mOctreeData[child6 + int3(0, 1, -1)]);
	mOctreeData[node + int3(1, 1, 0)] = 0.125f * (mOctreeData[child3 + int3(0, 0, 1)] + mOctreeData[child3 + int3(1, 0, 1)] + mOctreeData[child3 + int3(0, 1, 1)] + mOctreeData[child3 + int3(1, 1, 1)] +
		mOctreeData[child7 + int3(0, 0, -1)] + mOctreeData[child7 + int3(1, 0, -1)] + mOctreeData[child7 + int3(0, 1, -1)] + mOctreeData[child7 + int3(1, 1, -1)]);

	mOctreeData[node] = 0.125f * (mOctreeData[child0 + int3(1, 1, 1)] + mOctreeData[child1 + int3(-1, 1, 1)] + mOctreeData[child2 + int3(1, -1, 1)] + mOctreeData[child3 + int3(-1, -1, 1)] +
		mOctreeData[child4 + int3(1, 1, -1)] + mOctreeData[child5 + int3(-1, 1, -1)] + mOctreeData[child6 + int3(1, -1, -1)] + mOctreeData[child7 + int3(-1, -1, -1)]);

	mOctreeData[node + int3(0, 0, -1)] = 0.125f * (mOctreeData[child0 + int3(1, 1, -1)] + mOctreeData[child0 + int3(1, 1, 0)] + mOctreeData[child1 + int3(-1, 1, -1)] + mOctreeData[child1 + int3(-1, 1, 0)] + 
		mOctreeData[child2 + int3(1, -1, -1)] + mOctreeData[child2 + int3(1, -1, 0)] + mOctreeData[child3 + int3(-1, -1, -1)] + mOctreeData[child3 + int3(-1, -1, 0)]);
	mOctreeData[node + int3(0, 0, 1)] = 0.125f * (mOctreeData[child4 + int3(1, 1, 0)] + mOctreeData[child4 + int3(1, 1, 1)] + mOctreeData[child5 + int3(-1, 1, 0)] + mOctreeData[child5 + int3(-1, 1, 1)] +
		mOctreeData[child6 + int3(1, -1, 0)] + mOctreeData[child6 + int3(1, -1, 1)] + mOctreeData[child7 + int3(-1, -1, 0)] + mOctreeData[child7 + int3(-1, -1, 1)]);
	mOctreeData[node + int3(-1, 0, 0)] = 0.125f * (mOctreeData[child0 + int3(-1, 1, 1)] + mOctreeData[child0 + int3(0, 1, 1)] + mOctreeData[child2 + int3(-1, 0, 1)] + mOctreeData[child2 + int3(0, 0, 1)] +
		mOctreeData[child4 + int3(-1, 1, -1)] + mOctreeData[child4 + int3(0, 1, -1)] + mOctreeData[child6 + int3(-1, 0, -1)] + mOctreeData[child6 + int3(0, 0, -1)]);
	mOctreeData[node + int3(1, 0, 0)] = 0.125f * (mOctreeData[child1 + int3(0, 1, 1)] + mOctreeData[child1 + int3(1, 1, 1)] + mOctreeData[child3 + int3(0, 0, 1)] + mOctreeData[child3 + int3(1, 0, 1)] +
		mOctreeData[child5 + int3(0, 1, -1)] + mOctreeData[child5 + int3(1, 1, -1)] + mOctreeData[child7 + int3(0, 0, -1)] + mOctreeData[child7 + int3(1, 0, -1)]);
	mOctreeData[node + int3(0, -1, 0)] = 0.125f * (mOctreeData[child0 + int3(1, -1, 1)] + mOctreeData[child0 + int3(1, 0, 1)] + mOctreeData[child1 + int3(-1, -1, 1)] + mOctreeData[child1 + int3(-1, 0, 1)] +
		mOctreeData[child4 + int3(1, -1, -1)] + mOctreeData[child4 + int3(1, 0, -1)] + mOctreeData[child5 + int3(-1, -1, -1)] + mOctreeData[child5 + int3(-1, 0, -1)]);
	mOctreeData[node + int3(0, 1, 0)] = 0.125f * (mOctreeData[child2 + int3(1, 0, 1)] + mOctreeData[child2 + int3(1, 1, 1)] + mOctreeData[child3 + int3(-1, 0, 1)] + mOctreeData[child3 + int3(-1, 1, 1)] +
		mOctreeData[child6 + int3(1, 0, -1)] + mOctreeData[child6 + int3(1, 1, -1)] + mOctreeData[child7 + int3(-1, 0, -1)] + mOctreeData[child7 + int3(-1, 1, -1)]);*/
}