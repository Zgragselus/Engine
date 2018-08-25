cbuffer cameraBuffer : register(b0)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4 cameraPos;
	float cameraFov;
	float gizmoSize;
	float pad[26];
};

Texture2D<float4> guiImage : register(t0);

SamplerState imageSampler : register(s0);

struct GSInput
{
	float4 position : SV_POSITION;
	float4 texcoord : TEXCOORD0;
	uint entityId : TEXCOORD1;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	uint entityId : TEXCOORD1;
};

GSInput VS(uint id : SV_VertexID,
	float4 position : POSITION,
	float4 texcoord : TEXCOORD0,
	uint entityId : TEXCOORD1)
{
	GSInput result;
	
	result.position = position;
	result.texcoord = texcoord;
	result.entityId = entityId;

	return result;
}

[maxvertexcount(6)]
void GS(point GSInput input[1], inout TriangleStream<PSInput> output)
{
	PSInput v0, v1, v2, v3;

	float dist = length(input[0].position - cameraPos);
	float scale = dist / (2.0 * tan(cameraFov * 0.5)) * gizmoSize;
	scale *= clamp(1.0 / (dist * 0.002), 0.5, 2.5);

	float4 pos = mul(viewMatrix, input[0].position);

	v0.position = mul(projectionMatrix, pos + float4(-scale, -scale, 0.0f, 0.0f));
	v0.texcoord = float2(input[0].texcoord.x, input[0].texcoord.w);
	v0.entityId = input[0].entityId;

	v1.position = mul(projectionMatrix, pos + float4(-scale, scale, 0.0f, 0.0f));
	v1.texcoord = float2(input[0].texcoord.x, input[0].texcoord.y);
	v1.entityId = input[0].entityId;

	v2.position = mul(projectionMatrix, pos + float4(scale, -scale, 0.0f, 0.0f));
	v2.texcoord = float2(input[0].texcoord.z, input[0].texcoord.w);
	v2.entityId = input[0].entityId;

	v3.position = mul(projectionMatrix, pos + float4(scale, scale, 0.0f, 0.0f));
	v3.texcoord = float2(input[0].texcoord.z, input[0].texcoord.y);
	v3.entityId = input[0].entityId;

	output.Append(v0);
	output.Append(v1);
	output.Append(v2);
	output.Append(v3);
	output.RestartStrip();
}

float4 PS_RenderImage(PSInput input) : SV_TARGET
{
	float4 img = guiImage.Sample(imageSampler, float2(input.texcoord.x, input.texcoord.y));

	return img;
}

inline float4 UnpackRGBA(uint v)
{
	float4 rgba = float4(v & 0x000000ff, (v & 0x0000ff00) >> 8, (v & 0x00ff0000) >> 16, v >> 24) / 255.0;
	return rgba;
}

float4 PS_RenderID(PSInput input) : SV_TARGET
{
	float4 img = guiImage.Sample(imageSampler, float2(input.texcoord.x, input.texcoord.y));
	float4 id = UnpackRGBA(input.entityId);

	if (img.w < 0.5f)
	{
		discard;
	}

	return float4(id);
}