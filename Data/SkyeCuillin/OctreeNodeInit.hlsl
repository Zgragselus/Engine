#include "Voxel.hlsli"

cbuffer InputData : register(b0)
{
	unsigned int mNodes;				// How many nodes there was allocated and has to be initialized
	unsigned int mNodeOffset;			// Offset to first node that was allocated and needs initialization (in octree buffer)
}

RWStructuredBuffer<unsigned int> mOctreeNodes : register(u0);

[numthreads(1024, 1, 1)]
void OctreeNodeInit(uint GI : SV_GroupIndex, uint3 DTid : SV_DispatchThreadID)
{
	uint threadId = DTid.x;
	if (threadId >= mNodes)
	{
		return;
	}

	mOctreeNodes[mNodeOffset + threadId] = 0;
}