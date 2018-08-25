cbuffer dataBuffer : register(b0)
{
	uint2 dimensions;
	float2 offsets;
};

cbuffer inputData : register(b1)
{
	uint direction;
};

Texture2D inputMap : register(t0);

SamplerState inputSampler : register(s0);

struct VSOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

VSOut VS(uint id : SV_VertexID,
	float3 position : POSITION,
	float2 texCoord : TEXCOORD0)
{
	VSOut result;

	result.position = float4(position, 1.0f);
	result.texCoord = texCoord;

	return result;
}

float4 PS(VSOut input) : SV_TARGET
{
	float4 data = float4(0.0f, 0.0f, 0.0f, 0.0f);

	if (direction == 0)
	{
		for (int i = -2; i <= 2; i++)
		{
			data += inputMap.Sample(inputSampler, input.texCoord.xy + float2(offsets.x * (float)i, 0.0f));
		}
		data /= 5.0f;
	}
	else
	{
		for (int i = -2; i <= 2; i++)
		{
			data += inputMap.Sample(inputSampler, input.texCoord.xy + float2(0.0f, offsets.y * (float)i));
		}
		data /= 5.0f;
	}

	return data;
}