/*cbuffer InputBuffer : register(b0)
{
	float4 offset[16];
};*/

//float4x4 view : register(b0);
//float4x4 projection : register(b16);
cbuffer inputBuffer : register(b0)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4 pad[8];
};

Texture2D diffuse : register(t0);
SamplerState diffuseSampler : register(s0);

struct Input
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal : TEXCOORD1;
};

Input VS(uint id : SV_VertexID, 
	float3 position : POSITION, 
	float3 normal : NORMAL,
	float2 texCoord : TEXCOORD0)
{
	Input result;

	result.position = mul(projectionMatrix, mul(viewMatrix, float4(position, 1.0f)));
	result.texCoord = texCoord;
	result.normal = normal;

	return result;
}

float4 PS(Input input) : SV_TARGET
{
	float3 n = normalize(input.normal);
	float light = max(dot(n, normalize(float3(-0.2f, 1.0f, 0.3f))), 0.0f);

	float4 tex = diffuse.Sample(diffuseSampler, input.texCoord);
	
	return float4(tex.xyz * min(light + 0.2f, 1.0f), tex.w);
}