#include "Voxel.hlsli"

cbuffer InputData : register(b0)
{
	int mFactor;
	unsigned int mDimensions;
	unsigned int mLevel;
	unsigned int mMode;
}

StructuredBuffer<unsigned int> mVoxelsData : register(t0);
StructuredBuffer<unsigned int> mOctreeNodes : register(t1);
RWTexture3D<float4> mOctreeData : register(u0);

uint GetOctreeNode(int4 position)
{
	uint nodeId = 0;
	unsigned int node = 0;
	uint flag = 1;
	uint resolution = mDimensions;
	uint4 nodeMin = uint4(0, 0, 0, 0);
	uint4 nodeMax = uint4(mDimensions, mDimensions, mDimensions, 0);

	for (uint i = 0; i < mLevel; i++)
	{
		node = mOctreeNodes[nodeId];
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

void Interpolate(uint baseNode, uint node, uint axis)
{
	int3 baseIdx = VoxelToTexture(baseNode);
	int3 nextIdx = VoxelToTexture(node);

	switch (axis)
	{
	case 0:
		/*mOctreeData[baseIdx + int3(1, -1, -1)] = (mOctreeData[baseIdx + int3(1, -1, -1)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(-1, -1, -1)])) * 0.5f;
		mOctreeData[baseIdx + int3(1, 0, -1)] = (mOctreeData[baseIdx + int3(1, 0, -1)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(-1, 0, -1)])) * 0.5f;
		mOctreeData[baseIdx + int3(1, 1, -1)] = (mOctreeData[baseIdx + int3(1, 1, -1)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(-1, 1, -1)])) * 0.5f;
		mOctreeData[baseIdx + int3(1, -1, 0)] = (mOctreeData[baseIdx + int3(1, -1, 0)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(-1, -1, 0)])) * 0.5f;
		mOctreeData[baseIdx + int3(1, 0, 0)] = (mOctreeData[baseIdx + int3(1, 0, 0)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(-1, 0, 0)])) * 0.5f;
		mOctreeData[baseIdx + int3(1, 1, 0)] = (mOctreeData[baseIdx + int3(1, 1, 0)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(-1, 1, 0)])) * 0.5f;
		mOctreeData[baseIdx + int3(1, -1, 1)] = (mOctreeData[baseIdx + int3(1, -1, 1)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(-1, -1, 1)])) * 0.5f;
		mOctreeData[baseIdx + int3(1, 0, 1)] = (mOctreeData[baseIdx + int3(1, 0, 1)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(-1, 0, 1)])) * 0.5f;
		mOctreeData[baseIdx + int3(1, 1, 1)] = (mOctreeData[baseIdx + int3(1, 1, 1)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(-1, 1, 1)])) * 0.5f;*/
		mOctreeData[baseIdx + int3(1, -1, -1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(-1, -1, -1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(1, -1, -1)] + mOctreeData[nextIdx + int3(-1, -1, -1)]) * 0.5f);
		mOctreeData[baseIdx + int3(1, 0, -1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(-1, 0, -1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(1, 0, -1)] + mOctreeData[nextIdx + int3(-1, 0, -1)]) * 0.5f);
		mOctreeData[baseIdx + int3(1, 1, -1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(-1, 1, -1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(1, 1, -1)] + mOctreeData[nextIdx + int3(-1, 1, -1)]) * 0.5f);
		mOctreeData[baseIdx + int3(1, -1, 0)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(-1, -1, 0)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(1, -1, 0)] + mOctreeData[nextIdx + int3(-1, -1, 0)]) * 0.5f);
		mOctreeData[baseIdx + int3(1, 0, 0)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(-1, 0, 0)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(1, 0, 0)] + mOctreeData[nextIdx + int3(-1, 0, 0)]) * 0.5f);
		mOctreeData[baseIdx + int3(1, 1, 0)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(-1, 1, 0)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(1, 1, 0)] + mOctreeData[nextIdx + int3(-1, 1, 0)]) * 0.5f);
		mOctreeData[baseIdx + int3(1, -1, 1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(-1, -1, 1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(1, -1, 1)] + mOctreeData[nextIdx + int3(-1, -1, 1)]) * 0.5f);
		mOctreeData[baseIdx + int3(1, 0, 1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(-1, 0, 1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(1, 0, 1)] + mOctreeData[nextIdx + int3(-1, 0, 1)]) * 0.5f);
		mOctreeData[baseIdx + int3(1, 1, 1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(-1, 1, 1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(1, 1, 1)] + mOctreeData[nextIdx + int3(-1, 1, 1)]) * 0.5f);
		break;

	case 1:
		/*mOctreeData[baseIdx + int3(-1, 1, -1)] = (mOctreeData[baseIdx + int3(-1, 1, -1)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(-1, -1, -1)])) * 0.5f;
		mOctreeData[baseIdx + int3(0, 1, -1)] = (mOctreeData[baseIdx + int3(0, 1, -1)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(0, -1, -1)])) * 0.5f;
		mOctreeData[baseIdx + int3(1, 1, -1)] = (mOctreeData[baseIdx + int3(1, 1, -1)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(0, -1, -1)])) * 0.5f;
		mOctreeData[baseIdx + int3(-1, 1, 0)] = (mOctreeData[baseIdx + int3(-1, 1, 0)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(-1, -1, 0)])) * 0.5f;
		mOctreeData[baseIdx + int3(0, 1, 0)] = (mOctreeData[baseIdx + int3(0, 1, 0)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(0, -1, 0)])) * 0.5f;
		mOctreeData[baseIdx + int3(1, 1, 0)] = (mOctreeData[baseIdx + int3(1, 1, 0)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(1, -1, 0)])) * 0.5f;
		mOctreeData[baseIdx + int3(-1, 1, 1)] = (mOctreeData[baseIdx + int3(-1, 1, 1)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(-1, -1, 1)])) * 0.5f;
		mOctreeData[baseIdx + int3(0, 1, 1)] = (mOctreeData[baseIdx + int3(0, 1, 1)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(0, -1, 1)])) * 0.5f;
		mOctreeData[baseIdx + int3(1, 1, 1)] = (mOctreeData[baseIdx + int3(1, 1, 1)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(1, -1, 1)])) * 0.5f;*/
		mOctreeData[baseIdx + int3(-1, 1, -1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(-1, -1, -1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(-1, 1, -1)] + mOctreeData[nextIdx + int3(-1, -1, -1)]) * 0.5f);
		mOctreeData[baseIdx + int3(0, 1, -1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(0, -1, -1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(0, 1, -1)] + mOctreeData[nextIdx + int3(0, -1, -1)]) * 0.5f);
		mOctreeData[baseIdx + int3(1, 1, -1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(1, -1, -1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(1, 1, -1)] +  mOctreeData[nextIdx + int3(1, -1, -1)]) * 0.5f);
		mOctreeData[baseIdx + int3(-1, 1, 0)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(-1, -1, 0)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(-1, 1, 0)] + mOctreeData[nextIdx + int3(-1, -1, 0)]) * 0.5f);
		mOctreeData[baseIdx + int3(0, 1, 0)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(0, -1, 0)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(0, 1, 0)] + mOctreeData[nextIdx + int3(0, -1, 0)]) * 0.5f);
		mOctreeData[baseIdx + int3(1, 1, 0)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(1, -1, 0)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(1, 1, 0)] + mOctreeData[nextIdx + int3(1, -1, 0)]) * 0.5f);
		mOctreeData[baseIdx + int3(-1, 1, 1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(-1, -1, 1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(-1, 1, 1)] + mOctreeData[nextIdx + int3(-1, -1, 1)]) * 0.5f);
		mOctreeData[baseIdx + int3(0, 1, 1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(0, -1, 1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(0, 1, 1)] + mOctreeData[nextIdx + int3(0, -1, 1)]) * 0.5f);
		mOctreeData[baseIdx + int3(1, 1, 1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(1, -1, 1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(1, 1, 1)] + mOctreeData[nextIdx + int3(1, -1, 1)]) * 0.5f);
		break;

	case 2:
		/*mOctreeData[baseIdx + int3(-1, -1, 1)] = (mOctreeData[baseIdx + int3(-1, -1, 1)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(-1, -1, -1)])) * 0.5f;
		mOctreeData[baseIdx + int3(0, -1, 1)] = (mOctreeData[baseIdx + int3(0, -1, 1)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(0, -1, -1)])) * 0.5f;
		mOctreeData[baseIdx + int3(1, -1, 1)] = (mOctreeData[baseIdx + int3(1, -1, 1)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(1, -1, -1)])) * 0.5f;
		mOctreeData[baseIdx + int3(-1, 0, 1)] = (mOctreeData[baseIdx + int3(-1, 0, 1)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(-1, 0, -1)])) * 0.5f;
		mOctreeData[baseIdx + int3(0, 0, 1)] = (mOctreeData[baseIdx + int3(0, 0, 1)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(0, 0, -1)])) * 0.5f;
		mOctreeData[baseIdx + int3(1, 0, 1)] = (mOctreeData[baseIdx + int3(1, 0, 1)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(1, 0, -1)])) * 0.5f;
		mOctreeData[baseIdx + int3(-1, 1, 1)] = (mOctreeData[baseIdx + int3(-1, 1, 1)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(-1, 1, -1)])) * 0.5f;
		mOctreeData[baseIdx + int3(0, 1, 1)] = (mOctreeData[baseIdx + int3(0, 1, 1)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(0, 1, -1)])) * 0.5f;
		mOctreeData[baseIdx + int3(1, 1, 1)] = (mOctreeData[baseIdx + int3(1, 1, 1)] + (node == uint(-1) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(1, 1, -1)])) * 0.5f;*/
		mOctreeData[baseIdx + int3(-1, -1, 1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(-1, -1, -1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(-1, -1, 1)] + mOctreeData[nextIdx + int3(-1, -1, -1)]) * 0.5f);
		mOctreeData[baseIdx + int3(0, -1, 1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(0, -1, -1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(0, -1, 1)] + mOctreeData[nextIdx + int3(0, -1, -1)]) * 0.5f);
		mOctreeData[baseIdx + int3(1, -1, 1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(1, -1, -1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(1, -1, 1)] + mOctreeData[nextIdx + int3(1, -1, -1)]) * 0.5f);
		mOctreeData[baseIdx + int3(-1, 0, 1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(-1, 0, -1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(-1, 0, 1)] + mOctreeData[nextIdx + int3(-1, 0, -1)]) * 0.5f);
		mOctreeData[baseIdx + int3(0, 0, 1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(0, 0, -1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(0, 0, 1)] + mOctreeData[nextIdx + int3(0, 0, -1)]) * 0.5f);
		mOctreeData[baseIdx + int3(1, 0, 1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(1, 0, -1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(1, 0, 1)] + mOctreeData[nextIdx + int3(1, 0, -1)]) * 0.5f);
		mOctreeData[baseIdx + int3(-1, 1, 1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(-1, 1, -1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(-1, 1, 1)] + mOctreeData[nextIdx + int3(-1, 1, -1)]) * 0.5f);
		mOctreeData[baseIdx + int3(0, 1, 1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(0, 1, -1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(0, 1, 1)] + mOctreeData[nextIdx + int3(0, 1, -1)]) * 0.5f);
		mOctreeData[baseIdx + int3(1, 1, 1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(1, 1, -1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : (mOctreeData[baseIdx + int3(1, 1, 1)] + mOctreeData[nextIdx + int3(1, 1, -1)]) * 0.5f);
		break;
	}
}

void Copy(uint baseNode, uint node, uint axis)
{
	int3 baseIdx = VoxelToTexture(baseNode);
	int3 nextIdx = VoxelToTexture(node);

	switch (axis)
	{
	case 0:
		mOctreeData[baseIdx + int3(-1, -1, -1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(1, -1, -1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(1, -1, -1)]);
		mOctreeData[baseIdx + int3(-1, 0, -1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(1, 0, -1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(1, 0, -1)]);
		mOctreeData[baseIdx + int3(-1, 1, -1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(1, 1, -1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(1, 1, -1)]);
		mOctreeData[baseIdx + int3(-1, -1, 0)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(1, -1, 0)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(1, -1, 0)]);
		mOctreeData[baseIdx + int3(-1, 0, 0)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(1, 0, 0)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(1, 0, 0)]);
		mOctreeData[baseIdx + int3(-1, 1, 0)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(1, 1, 0)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(1, 1, 0)]);
		mOctreeData[baseIdx + int3(-1, -1, 1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(1, -1, 1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(1, -1, 1)]);
		mOctreeData[baseIdx + int3(-1, 0, 1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(1, 0, 1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(1, 0, 1)]);
		mOctreeData[baseIdx + int3(-1, 1, 1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(1, 1, 1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(1, 1, 1)]);
		break;

	case 1:
		mOctreeData[baseIdx + int3(-1, -1, -1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(-1, 1, -1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(-1, 1, -1)]);
		mOctreeData[baseIdx + int3(0, -1, -1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(0, 1, -1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(0, 1, -1)]);
		mOctreeData[baseIdx + int3(1, -1, -1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(1, 1, -1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(1, 1, -1)]);
		mOctreeData[baseIdx + int3(-1, -1, 0)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(-1, 1, 0)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(-1, 1, 0)]);
		mOctreeData[baseIdx + int3(0, -1, 0)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(0, 1, 0)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(0, 1, 0)]);
		mOctreeData[baseIdx + int3(1, -1, 0)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(1, 1, 0)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(1, 1, 0)]);
		mOctreeData[baseIdx + int3(-1, -1, 1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(-1, 1, 1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(-1, 1, 1)]);
		mOctreeData[baseIdx + int3(0, -1, 1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(0, 1, 1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(0, 1, 1)]);
		mOctreeData[baseIdx + int3(1, -1, 1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(1, 1, 1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(1, 1, 1)]);
		break;

	case 2:
		mOctreeData[baseIdx + int3(-1, -1, -1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(-1, -1, 1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(-1, -1, 1)]);
		mOctreeData[baseIdx + int3(0, -1, -1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(0, -1, 1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(0, -1, 1)]);
		mOctreeData[baseIdx + int3(1, -1, -1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(1, -1, 1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(1, -1, 1)]);
		mOctreeData[baseIdx + int3(-1, 0, -1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(-1, 0, 1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(-1, 0, 1)]);
		mOctreeData[baseIdx + int3(0, 0, -1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(0, 0, 1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(0, 0, 1)]);
		mOctreeData[baseIdx + int3(1, 0, -1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(1, 0, 1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(1, 0, 1)]);
		mOctreeData[baseIdx + int3(-1, 1, -1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(-1, 1, 1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(-1, 1, 1)]);
		mOctreeData[baseIdx + int3(0, 1, -1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(0, 1, 1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(0, 1, 1)]);
		mOctreeData[baseIdx + int3(1, 1, -1)] = ((node == uint(-1) || mOctreeData[nextIdx + int3(1, 1, 1)] == float4(0.0f, 0.0f, 0.0f, 0.0f)) ? float4(0.0f, 0.0f, 0.0f, 0.0f) : mOctreeData[nextIdx + int3(1, 1, 1)]);
		break;
	}
}

[numthreads(8, 8, 8)]
void OctreeDataFilter(uint GI : SV_GroupIndex, uint3 DTid : SV_DispatchThreadID)
{
	uint4 position = uint4(DTid * mFactor, 0);
	uint baseNode = GetOctreeNode(position);

	if (baseNode == uint(-1))
	{
		return;
	}

	if (mMode == 0)
	{
		uint right = GetOctreeNode(position + int4(mFactor, 0, 0, 0));
		Interpolate(baseNode, right, 0);
	}
	else if (mMode == 1)
	{
		uint left = GetOctreeNode(position + int4(-mFactor, 0, 0, 0));
		Copy(baseNode, left, 0);
	}
	else if (mMode == 2)
	{
		uint up = GetOctreeNode(position + int4(0, mFactor, 0, 0));
		Interpolate(baseNode, up, 1);
	}
	else if (mMode == 3)
	{
		uint down = GetOctreeNode(position + int4(0, -mFactor, 0, 0));
		Copy(baseNode, down, 1);
	}
	else if (mMode == 4)
	{
		uint back = GetOctreeNode(position + int4(0, 0, mFactor, 0));
		Interpolate(baseNode, back, 2);
	}
	else if (mMode == 5)
	{
		uint front = GetOctreeNode(position + int4(0, 0, -mFactor, 0));
		Copy(baseNode, front, 2);
	}
	else if (mMode == 6)
	{
		int3 baseIdx = VoxelToTexture(baseNode);
		mOctreeData[baseIdx + int3(0, -1, -1)] = (mOctreeData[baseIdx + int3(-1, -1, -1)] + mOctreeData[baseIdx + int3(1, -1, -1)]) * 0.5f;
		mOctreeData[baseIdx + int3(0, 1, -1)] = (mOctreeData[baseIdx + int3(-1, 1, -1)] + mOctreeData[baseIdx + int3(1, 1, -1)]) * 0.5f;
		mOctreeData[baseIdx + int3(-1, 0, -1)] = (mOctreeData[baseIdx + int3(-1, -1, -1)] + mOctreeData[baseIdx + int3(-1, 1, -1)]) * 0.5f;
		mOctreeData[baseIdx + int3(1, 0, -1)] = (mOctreeData[baseIdx + int3(1, -1, -1)] + mOctreeData[baseIdx + int3(1, 1, -1)]) * 0.5f;
		mOctreeData[baseIdx + int3(0, -1, 1)] = (mOctreeData[baseIdx + int3(-1, -1, 1)] + mOctreeData[baseIdx + int3(1, -1, 1)]) * 0.5f;
		mOctreeData[baseIdx + int3(0, 1, 1)] = (mOctreeData[baseIdx + int3(-1, 1, 1)] + mOctreeData[baseIdx + int3(1, 1, 1)]) * 0.5f;
		mOctreeData[baseIdx + int3(-1, 0, 1)] = (mOctreeData[baseIdx + int3(-1, -1, 1)] + mOctreeData[baseIdx + int3(-1, 1, 1)]) * 0.5f;
		mOctreeData[baseIdx + int3(1, 0, 1)] = (mOctreeData[baseIdx + int3(1, -1, 1)] + mOctreeData[baseIdx + int3(1, 1, 1)]) * 0.5f;
		mOctreeData[baseIdx + int3(-1, -1, 0)] = (mOctreeData[baseIdx + int3(-1, -1, -1)] + mOctreeData[baseIdx + int3(-1, -1, 1)]) * 0.5f;
		mOctreeData[baseIdx + int3(1, -1, 0)] = (mOctreeData[baseIdx + int3(1, -1, -1)] + mOctreeData[baseIdx + int3(1, -1, 1)]) * 0.5f;
		mOctreeData[baseIdx + int3(1, 1, 0)] = (mOctreeData[baseIdx + int3(1, 1, -1)] + mOctreeData[baseIdx + int3(1, 1, 1)]) * 0.5f;
		mOctreeData[baseIdx + int3(-1, 1, 0)] = (mOctreeData[baseIdx + int3(-1, 1, -1)] + mOctreeData[baseIdx + int3(-1, 1, 1)]) * 0.5f;
		mOctreeData[baseIdx + int3(0, 0, -1)] = (mOctreeData[baseIdx + int3(-1, -1, -1)] + mOctreeData[baseIdx + int3(1, -1, -1)] + mOctreeData[baseIdx + int3(-1, 1, -1)] + mOctreeData[baseIdx + int3(1, 1, -1)]) * 0.25f;
		mOctreeData[baseIdx + int3(0, 0, 1)] = (mOctreeData[baseIdx + int3(-1, -1, 1)] + mOctreeData[baseIdx + int3(1, -1, 1)] + mOctreeData[baseIdx + int3(-1, 1, 1)] + mOctreeData[baseIdx + int3(1, 1, 1)]) * 0.25f;
		mOctreeData[baseIdx + int3(0, -1, 0)] = (mOctreeData[baseIdx + int3(-1, -1, -1)] + mOctreeData[baseIdx + int3(1, -1, -1)] + mOctreeData[baseIdx + int3(-1, -1, 1)] + mOctreeData[baseIdx + int3(1, -1, 1)]) * 0.25f;
		mOctreeData[baseIdx + int3(0, 1, 0)] = (mOctreeData[baseIdx + int3(-1, 1, -1)] + mOctreeData[baseIdx + int3(1, 1, -1)] + mOctreeData[baseIdx + int3(-1, 1, 1)] + mOctreeData[baseIdx + int3(1, 1, 1)]) * 0.25f;
		mOctreeData[baseIdx + int3(-1, 0, 0)] = (mOctreeData[baseIdx + int3(-1, -1, -1)] + mOctreeData[baseIdx + int3(-1, 1, -1)] + mOctreeData[baseIdx + int3(-1, -1, 1)] + mOctreeData[baseIdx + int3(-1, 1, 1)]) * 0.25f;
		mOctreeData[baseIdx + int3(1, 0, 0)] = (mOctreeData[baseIdx + int3(1, -1, -1)] + mOctreeData[baseIdx + int3(1, 1, -1)] + mOctreeData[baseIdx + int3(1, -1, 1)] + mOctreeData[baseIdx + int3(1, 1, 1)]) * 0.25f;
		mOctreeData[baseIdx] = (mOctreeData[baseIdx + int3(-1, -1, -1)] + mOctreeData[baseIdx + int3(1, -1, -1)] + mOctreeData[baseIdx + int3(-1, 1, -1)] + mOctreeData[baseIdx + int3(1, 1, -1)] + 
			mOctreeData[baseIdx + int3(-1, -1, 1)] + mOctreeData[baseIdx + int3(1, -1, 1)] + mOctreeData[baseIdx + int3(-1, 1, 1)] + mOctreeData[baseIdx + int3(1, 1, 1)]) * 0.125f;
	}
	else
	{
		int3 baseIdx = VoxelToTexture(baseNode);
		uint left = GetOctreeNode(position + int4(-mFactor, 0, 0, 0));
		uint right = GetOctreeNode(position + int4(mFactor, 0, 0, 0));
		uint up = GetOctreeNode(position + int4(0, mFactor, 0, 0));
		uint down = GetOctreeNode(position + int4(0, -mFactor, 0, 0));
		uint back = GetOctreeNode(position + int4(0, 0, mFactor, 0));
		uint front = GetOctreeNode(position + int4(0, 0, -mFactor, 0));

		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2; j++)
			{
				if (left == uint(-1)) mOctreeData[baseIdx + int3(-1, i, j)] = float4(0.0f, 0.0f, 0.0f, 0.0f);
				if (right == uint(-1)) mOctreeData[baseIdx + int3(1, i, j)] = float4(0.0f, 0.0f, 0.0f, 0.0f);
				if (up == uint(-1)) mOctreeData[baseIdx + int3(i, 1, j)] = float4(0.0f, 0.0f, 0.0f, 0.0f);
				if (down == uint(-1)) mOctreeData[baseIdx + int3(i, -1, j)] = float4(0.0f, 0.0f, 0.0f, 0.0f);
				if (back == uint(-1)) mOctreeData[baseIdx + int3(i, j, 1)] = float4(0.0f, 0.0f, 0.0f, 0.0f);
				if (front == uint(-1)) mOctreeData[baseIdx + int3(i, j, -1)] = float4(0.0f, 0.0f, 0.0f, 0.0f);
			}
		}
	}
}