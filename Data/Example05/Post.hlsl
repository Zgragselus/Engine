Texture2DMS<float4, 8> diffuse : register(t0);
SamplerState diffuseSampler : register(s0);

cbuffer buffer1 : register(b0)
{
	uint2 dimensions;
}

cbuffer buffer2 : register(b1)
{
	uint samples;
}

struct Input
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

Input VS(uint id : SV_VertexID, 
	float3 position : POSITION,
	float2 texCoord : TEXCOORD0)
{
	Input result;

	result.position = float4(position, 1.0f);
	result.texCoord = texCoord;

	return result;
}

float4 PS(Input input) : SV_TARGET
{
	uint2 coord = uint2(input.texCoord.x * 2 * dimensions.x, input.texCoord.y * 2 * dimensions.y);
	uint2 coord2 = uint2(input.texCoord.x * 2 * dimensions.x + 1, input.texCoord.y * 2 * dimensions.y);
	uint2 coord3 = uint2(input.texCoord.x * 2 * dimensions.x, input.texCoord.y * 2 * dimensions.y + 1);
	uint2 coord4 = uint2(input.texCoord.x * 2 * dimensions.x + 1, input.texCoord.y * 2 * dimensions.y + 1);
	
	float4 tex = float4(0.0f, 0.0f, 0.0f, 0.0f);
	for (uint i = 0; i < samples; i++)
	{
		tex += diffuse.Load(coord, i);
		tex += diffuse.Load(coord2, i);
		tex += diffuse.Load(coord3, i);
		tex += diffuse.Load(coord4, i);
	}
	tex *= 1.0f / float(samples);
	tex *= 0.25f;

	return float4(tex.xyz, 1.0f);
}