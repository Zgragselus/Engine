#include "Voxel.hlsli"

cbuffer InputData : register(b0)
{
	unsigned int mVoxels;				// How many voxels are going to be injected into the octree
	unsigned int mDimensions;
	unsigned int mLevel;
}

StructuredBuffer<unsigned int> mVoxelsData : register(t0);
StructuredBuffer<unsigned int> mVoxelsColor : register(t1);
StructuredBuffer<unsigned int> mOctreeNodes : register(t2);
RWTexture3D<float4> mOctreeData : register(u0);

void StoreAverage(float4 color, uint coords, uint child)
{
	int3 texCoord = VoxelToTexture(coords);

	switch (child)
	{
	case 0:
		mOctreeData[texCoord + int3(-1, -1, -1)] = float4(color.xyz, 1.0f);
		break;

	case 1:
		mOctreeData[texCoord + int3(1, -1, -1)] = float4(color.xyz, 1.0f);
		break;

	case 2:
		mOctreeData[texCoord + int3(-1, 1, -1)] = float4(color.xyz, 1.0f);
		break;

	case 3:
		mOctreeData[texCoord + int3(1, 1, -1)] = float4(color.xyz, 1.0f);
		break;

	case 4:
		mOctreeData[texCoord + int3(-1, -1, 1)] = float4(color.xyz, 1.0f);
		break;

	case 5:
		mOctreeData[texCoord + int3(1, -1, 1)] = float4(color.xyz, 1.0f);
		break;

	case 6:
		mOctreeData[texCoord + int3(-1, 1, 1)] = float4(color.xyz, 1.0f);
		break;

	case 7:
		mOctreeData[texCoord + int3(1, 1, 1)] = float4(color.xyz, 1.0f);
		break;
	}
}

[numthreads(1024, 1, 1)]
void OctreeDataFill(uint GI : SV_GroupIndex, uint3 DTid : SV_DispatchThreadID)
{
	uint threadId = DTid.x;
	if (threadId >= mVoxels)
	{
		return;
	}

	uint4 position = UnpackRGB10A2(mVoxelsData[threadId]);
	float4 color = UnpackRGBA(mVoxelsColor[threadId]);

	uint nodeId = 0;
	unsigned int node = mOctreeNodes[nodeId];
	uint flag = 1;
	uint resolution = mDimensions;
	uint4 nodeMin = uint4(0, 0, 0, 0);
	uint4 nodeMax = uint4(mDimensions, mDimensions, mDimensions, 0);
	uint childOffset = 0;
	uint4 offset = uint4(0, 0, 0, 0);

	for (uint i = 0; i < mLevel; i++)
	{
		childOffset = 0;
		resolution /= 2;

		if ((node & 0x80000000) == 0)
		{
			flag = 0;
			break;
		}
		
		offset = position - nodeMin;
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

	resolution /= 2;

	childOffset = 0;

	offset = position - nodeMin;
	if (offset.x >= resolution)
	{
		childOffset += 1;
	}

	if (offset.y >= resolution)
	{
		childOffset += 2;
	}

	if (offset.z >= resolution)
	{
		childOffset += 4;
	}

	StoreAverage(color, nodeId, childOffset);
}