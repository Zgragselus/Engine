Texture2D diffuse : register(t0);
SamplerState diffuseSampler : register(s0);

cbuffer DisplaySize : register(b0)
{
	float2 size;
};

struct Input
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float4 color : COLOR;
};

Input VS(uint id : SV_VertexID,
	float4 position : POSITION,
	float2 texCoord : TEXCOORD0,
	float4 color : COLOR)
{
	Input result;

	float x = position.x / size.x * 2.0f - 1.0f;
	float y = (1.0f - position.y / size.y) * 2.0f - 1.0f;

	result.position = float4(x, y, 0.0f, 1.0f);
	result.texCoord = float2(texCoord.x, texCoord.y);
	result.color = color;

	return result;
}

float4 PS(Input input) : SV_TARGET
{
	//float4 tex = input.color * diffuse.SampleLevel(diffuseSampler, input.texCoord, 0.0f);
	float4 tex = input.color * diffuse.Sample(diffuseSampler, input.texCoord);
	return tex;
}