#include "Voxel.hlsli"

cbuffer inputBuffer : register(b0)
{
	float4x4 mvpX;
	float4x4 mvpY;
	float4x4 mvpZ;
	float4 offset;
	float4 scale;
	uint4 dimension;
	float4 pad;
};

Texture2D diffuse : register(t0);
SamplerState diffuseSampler : register(s0);
//globallycoherent RWStructuredBuffer<uint4> voxel : register(u0);

RWStructuredBuffer<uint> voxel : register(u0);
RWStructuredBuffer<uint> voxelColor : register(u1);

struct Vert2Geom
{
	float4 mPosition : SV_POSITION;
	float2 mTexCoord : TEXCOORD0;
	float3 mNormal : TEXCOORD1;
};

struct Geom2Frag
{
	float4 mPosition : SV_POSITION;
	float2 mTexCoord : TEXCOORD0;
	float3 mNormal : TEXCOORD1;
	nointerpolation uint mAxis : AXIS;
	nointerpolation float4 mAABB : AABB;
};

Vert2Geom VS(float3 mPosition : POSITION,
	float3 mNormal : NORMAL,
	float2 mTexCoord : TEXCOORD0)
{
	Vert2Geom result;

	result.mPosition = float4((mPosition + offset.xyz) * scale.xyz * 2.0f - 1.0f, 1.0f);
	result.mNormal = mNormal;
	result.mTexCoord = mTexCoord;

	return result;
}

[maxvertexcount(3)]
void GS(triangle Vert2Geom input[3], inout TriangleStream<Geom2Frag> output)
{
	float3 faceNormal = normalize(cross(input[1].mPosition.xyz - input[0].mPosition.xyz, input[2].mPosition.xyz - input[0].mPosition.xyz));
	float xAxis = abs(faceNormal.x);
	float yAxis = abs(faceNormal.y);
	float zAxis = abs(faceNormal.z);

	float4x4 projectionMatrix;
	uint axis;
	if (xAxis > yAxis && xAxis > zAxis)
	{
		projectionMatrix = mvpX;
		axis = 0;
	}
	else if (yAxis > xAxis && yAxis > zAxis)
	{
		projectionMatrix = mvpY;
		axis = 1;
	}
	else
	{
		projectionMatrix = mvpZ;
		axis = 2;
	}

	Geom2Frag v[3];

	v[0].mPosition = mul(projectionMatrix, input[0].mPosition);
	v[0].mTexCoord = input[0].mTexCoord;
	v[0].mNormal = input[0].mPosition.xyz;
	v[0].mAxis = axis;

	v[1].mPosition = mul(projectionMatrix, input[1].mPosition);
	v[1].mTexCoord = input[1].mTexCoord;
	v[1].mNormal = input[1].mPosition.xyz;
	v[1].mAxis = axis;

	v[2].mPosition = mul(projectionMatrix, input[2].mPosition);
	v[2].mTexCoord = input[2].mTexCoord;
	v[2].mNormal = input[2].mPosition.xyz;
	v[2].mAxis = axis;

	float2 halfPixel = float2(1.0f / (float)dimension.x, 1.0f / (float)dimension.y) * 0.5f;
	float dilationSize = 1.4142135637309f / (float)dimension.x;

	float4 AABB = float4(0.0f, 0.0f, 0.0f, 0.0f);
	AABB.xy = v[0].mPosition.xy;
	AABB.zw = v[0].mPosition.xy;

	AABB.xy = min(v[1].mPosition.xy, AABB.xy);
	AABB.zw = max(v[1].mPosition.xy, AABB.zw);

	AABB.xy = min(v[2].mPosition.xy, AABB.xy);
	AABB.zw = max(v[2].mPosition.xy, AABB.zw);

	AABB.xy -= halfPixel;
	AABB.zw += halfPixel;

	v[0].mAABB = AABB;
	v[1].mAABB = AABB;
	v[2].mAABB = AABB;

	float3 edge[3];
	edge[0] = normalize(float3(v[1].mPosition.xy - v[0].mPosition.xy, 0.0f));
	edge[1] = normalize(float3(v[2].mPosition.xy - v[1].mPosition.xy, 0.0f));
	edge[2] = normalize(float3(v[0].mPosition.xy - v[2].mPosition.xy, 0.0f));

	v[0].mPosition.xy = v[0].mPosition.xy + dilationSize * normalize(-edge[0].xy + edge[2].xy);
	v[1].mPosition.xy = v[1].mPosition.xy + dilationSize * normalize(-edge[1].xy + edge[0].xy);
	v[2].mPosition.xy = v[2].mPosition.xy + dilationSize * normalize(-edge[2].xy + edge[1].xy);

	output.Append(v[0]);
	output.Append(v[1]);
	output.Append(v[2]);

	output.RestartStrip();
}

float4 PS(Geom2Frag input, float4 fragCoord : SV_Position) : SV_TARGET
{
	uint4 temp = uint4((uint)fragCoord.x, (uint)fragCoord.y, (uint)((float)dimension.z * fragCoord.z), 0);
	uint4 texCoord = uint4(0, 0, 0, 0);
	if (input.mAxis == 1)
	{
		texCoord.x = dimension.x - temp.x - 1;
		texCoord.y = temp.z;
		texCoord.z = dimension.y - temp.y - 1;
	}
	else if (input.mAxis == 2)
	{
		texCoord.x = temp.x;
		texCoord.y = dimension.y - temp.y - 1;
		texCoord.z = temp.z;
	}
	else
	{
		texCoord.x = temp.z;
		texCoord.y = dimension.y - temp.y - 1;
		texCoord.z = dimension.x - temp.x - 1;
	}

	float4 tex = diffuse.Sample(diffuseSampler, input.mTexCoord);
	
	uint idx = voxel.IncrementCounter();
	if (idx < dimension.w)
	{
		voxel[idx] = PackRGB10A2(texCoord);
		voxelColor[idx] = PackRGBA(tex);
	}

	return float4(tex.xyz, 1.0f);
}