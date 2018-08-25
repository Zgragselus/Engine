#include "Lighting.hlsli"

cbuffer matrixConstBuffer : register(b0)
{
	float4x4 cubeProjection;
	float4x4 cubeView[6];
};

Texture2D diffuse : register(t0);

SamplerState anisoSampler : register(s0);

cbuffer modelBuffer : register(b1)
{
	float4x4 worldMatrix[32];
};

cbuffer rootConst : register(b2)
{
	uint rootID;
	uint lightID;
	uint cubeID;
};

StructuredBuffer<Light> lightsData : register(t1);
StructuredBuffer<ShadowVirtualTextureRecord> shadowAtlasData : register(t2);

struct VSOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float4 positionVS : TEXCOORD1;
};

VSOut VS(uint id : SV_VertexID,
	float3 position : POSITION,
	float2 texCoord : TEXCOORD0)
{
	VSOut result;

	float4 positionWS = mul(float4(position, 1.0f), worldMatrix[rootID * 2]);

	result.position = mul(shadowAtlasData[lightsData[lightID].shadowID].shadowMatrix, positionWS);
	result.positionVS = result.position;
	result.texCoord = texCoord;

	return result;
}

VSOut VSCube(uint id : SV_VertexID,
	float3 position : POSITION,
	float2 texCoord : TEXCOORD0)
{
	VSOut result;

	float4 positionWS = mul(float4(position, 1.0f), worldMatrix[rootID * 2]);

	if (cubeID == 0)
		result.position = mul(shadowAtlasData[asuint(lightsData[lightID].data0.z)].shadowMatrix, positionWS);
	else if (cubeID == 1)
		result.position = mul(shadowAtlasData[asuint(lightsData[lightID].data0.w)].shadowMatrix, positionWS);
	else if (cubeID == 2)
		result.position = mul(shadowAtlasData[asuint(lightsData[lightID].data1.x)].shadowMatrix, positionWS);
	else if (cubeID == 3)
		result.position = mul(shadowAtlasData[asuint(lightsData[lightID].data1.y)].shadowMatrix, positionWS);
	else if (cubeID == 4)
		result.position = mul(shadowAtlasData[asuint(lightsData[lightID].data1.z)].shadowMatrix, positionWS);
	else
		result.position = mul(shadowAtlasData[asuint(lightsData[lightID].data1.w)].shadowMatrix, positionWS);
	result.positionVS = result.position;
	result.texCoord = texCoord;

	return result;
}

float4 PS(VSOut input) : SV_TARGET
{
	float depth = input.positionVS.z / (lightsData[lightID].shadowFar - lightsData[lightID].shadowNear);

	float2 moments = float2(depth, depth * depth);

	float dx = ddx(depth);
	float dy = ddy(depth);
	moments.y += 0.25f * (dx * dx + dy * dy);

	float4 diffuseMap = diffuse.Sample(anisoSampler, input.texCoord);
		
	return float4(moments.xy, 0.0f, diffuseMap.w);
}