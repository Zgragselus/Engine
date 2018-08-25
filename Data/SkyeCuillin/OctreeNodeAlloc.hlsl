#include "Voxel.hlsli"

cbuffer InputData : register(b0)
{
	unsigned int mNodes;				// How many nodes there is to process at current level (=8)
	unsigned int mNodeOffset;			// Offset to first node to process (in octree buffer) (=1)
	unsigned int mAllocationOffset;		// Offset to first free space (in octree buffer) - e.g. from where next level can be allocated (=9)
}

RWStructuredBuffer<unsigned int> mOctreeNodes : register(u0);
RWByteAddressBuffer mOctreeCounter : register(u1);

[numthreads(1024, 1, 1)]
void OctreeNodeAlloc(uint GI : SV_GroupIndex, uint3 DTid : SV_DispatchThreadID)
{
	uint threadId = DTid.x;
	if (threadId >= mNodes)
	{
		return;
	}

	uint nodeId = mNodeOffset + threadId;
	uint node = mOctreeNodes.Load(nodeId);
	if ((node & 0x80000000) != 0)
	{
		uint firstChild = 0;
		mOctreeCounter.InterlockedAdd(0, 1, firstChild);
		firstChild *= 8;
		firstChild += mAllocationOffset;
		firstChild |= 0x80000000;

		mOctreeNodes[nodeId] = firstChild;
	}
}