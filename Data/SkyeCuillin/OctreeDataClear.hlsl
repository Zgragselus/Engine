#include "Voxel.hlsli"

cbuffer InputData : register(b0)
{
	unsigned int mNodes;				// Total number of nodes to clear
}

RWTexture3D<float4> mOctreeData : register(u0);

[numthreads(1024, 1, 1)]
void OctreeDataClear(uint GI : SV_GroupIndex, uint3 DTid : SV_DispatchThreadID)
{
	uint threadId = DTid.x;
	if (threadId >= mNodes)
	{
		return;
	}

	int3 texCoord = VoxelToTexture(threadId);

	for (int i = -1; i < 2; i++)
	{
		for (int j = -1; j < 2; j++)
		{
			for (int k = -1; k < 2; k++)
			{
				mOctreeData[texCoord + int3(i, j, k)] = 0;
			}
		}
	}
}