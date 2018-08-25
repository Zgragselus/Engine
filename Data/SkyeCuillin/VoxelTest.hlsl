#include "Voxel.hlsli"

cbuffer inputBuffer : register(b0)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4 offset;
	float4 scale;
	float4 cameraPos;
	float4 pad[5];
};

StructuredBuffer<uint> voxel : register(t0);
StructuredBuffer<uint> voxelColor : register(t1);
StructuredBuffer<uint> octree : register(t2);

cbuffer inputBuffer : register(b1)
{
	int level;
}

struct Vert2Frag
{
	float4 mPosition : SV_POSITION;
	float4 mColor : TEXCOORD0;
};

Vert2Frag VS(float3 mPosition : POSITION,
	uint mInstance : SV_InstanceID)
{
	float scale = 512.0 / pow(2.0, (float)level);

	uint4 position = UnpackRGB10A2(voxel[mInstance]);
	uint dimensions = 512;
	int3 nodeMin = int3(0, 0, 0);
	int3 nodeMax = int3(512, 512, 512);
	
	int currentId = 0;
	uint node = octree[currentId];
	uint child = 0;
	bool flag = true;

	int offsetX;
	int offsetY;
	int offsetZ;

	for (int i = 0; i < level; i++)
	{
		dimensions /= 2;

		if ((node & 0x80000000) == 0)
		{
			flag = false;
			break;
		}

		child = (node & 0x7FFFFFFF);

		offsetX = 0;
		offsetY = 0;
		offsetZ = 0;
		if (((int)position.x - nodeMin.x) >= (int)dimensions)
		{
			offsetX = 1;
		}
		if (((int)position.y - nodeMin.y) >= (int)dimensions)
		{
			offsetY = 1;
		}
		if (((int)position.z - nodeMin.z) >= (int)dimensions)
		{
			offsetZ = 1;
		}
		//offsetX = ((int)position.x - nodeMin.x) < (int)dimensions ? 0 : 1;
		//offsetY = ((int)position.y - nodeMin.y) < (int)dimensions ? 0 : 1;
		//offsetZ = ((int)position.z - nodeMin.z) < (int)dimensions ? 0 : 1;
		child += offsetX + 2 * offsetY + 4 * offsetZ;
		nodeMin.x += dimensions * offsetX;
		nodeMin.y += dimensions * offsetY;
		nodeMin.z += dimensions * offsetZ;
		nodeMax.x = nodeMin.x + dimensions;
		nodeMax.y = nodeMin.y + dimensions;
		nodeMax.z = nodeMin.z + dimensions;
		currentId = child;
		node = octree[currentId];
	}

	Vert2Frag result;
	
	uint4 location = UnpackRGB10A2(voxel[mInstance]);
	float3 offset = float3(float(location.x), float(location.y), float(location.z));
	offset = float3(float(nodeMin.x), float(nodeMin.y), float(nodeMin.z));
	if (flag == false)
	{
		offset.x += 10000.0;
	}
	result.mPosition = mul(projectionMatrix, mul(viewMatrix, float4(mPosition * 0.5f * float(nodeMax.x - nodeMin.x) + offset, 1.0f)));
	//result.mPosition = mul(projectionMatrix, mul(viewMatrix, float4(mPosition * 0.5f + offset, 1.0f)));
	result.mColor = UnpackRGBA(voxelColor[currentId]);

	return result;
}

float4 PS(Vert2Frag input) : SV_TARGET
{
	return float4(input.mColor.xyz, 1.0f);
}