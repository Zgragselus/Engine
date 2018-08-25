cbuffer cameraBuffer : register(b0)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4 pad[8];
};

cbuffer rootConst : register(b1)
{
	uint rootID;
	uint entityID;
}

cbuffer modelBuffer : register(b2)
{
	float4x4 worldMatrix[32];
}

struct VSOut
{
	float4 position : SV_POSITION;
};

VSOut VS(uint id : SV_VertexID,
	float3 position : POSITION,
	float3 normal : NORMAL,
	float2 texCoord : TEXCOORD0,
	float3 tangent : TEXCOORD2,
	float3 bitangent : TEXCOORD3)
{
	VSOut result;

	float4 positionWS = mul(float4(position, 1.0f), worldMatrix[rootID * 2]);
	result.position = mul(projectionMatrix, mul(viewMatrix, positionWS));
	return result;
}

inline float4 UnpackRGBA(uint v)
{
	float4 rgba = float4(v & 0x000000ff, (v & 0x0000ff00) >> 8, (v & 0x00ff0000) >> 16, v >> 24) / 255.0;
	return rgba;
}

float4 PS(VSOut input) : SV_TARGET
{
	return UnpackRGBA(entityID);
}