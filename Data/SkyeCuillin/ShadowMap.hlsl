cbuffer cameraBuffer : register(b0)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4 pad[8];
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
};

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

	result.position = mul(projectionMatrix, mul(viewMatrix, positionWS));
	result.positionVS = result.position;
	result.texCoord = texCoord;

	return result;
}

float4 PS(VSOut input) : SV_TARGET
{
	float depth = input.positionVS.z / input.positionVS.w;

	//return float4(depth, depth, 0.0f, 1.0f);

	//float4 diffuseMap = diffuse.Sample(anisoSampler, input.texCoord);

	//return float4(depth, depth, 0.0f, diffuseMap.w);

	float2 moments = float2(depth, depth * depth);

	float dx = ddx(depth);
	float dy = ddy(depth);
	moments.y += 0.25f * (dx * dx + dy * dy);

	float4 diffuseMap = diffuse.Sample(anisoSampler, input.texCoord);
		
	return float4(moments.xy, 0.0f, diffuseMap.w);
}