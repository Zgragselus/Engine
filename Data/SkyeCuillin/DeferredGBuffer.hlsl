#include "Normals.hlsli"

cbuffer cameraBuffer : register(b0)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4 pad[8];
};

Texture2D diffuse : register(t0);
Texture2D normals : register(t1);
Texture2D metallic : register(t2);
Texture2D roughness : register(t3);
Texture2D height : register(t4);

SamplerState anisoSampler : register(s0);

cbuffer rootConst : register(b1)
{
	uint rootID;
}

cbuffer modelBuffer : register(b2)
{
	float4x4 worldMatrix[32];
}

struct VSOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal : TEXCOORD1;
	float3 tangent : TEXCOORD2;
	float3 bitangent : TEXCOORD3;
	float tessfactor : TESS;
	float3 viewDir : TEXCOORD4;
};

VSOut VS(uint id : SV_VertexID,
	float3 position : POSITION,
	float3 normal : NORMAL,
	float2 texCoord : TEXCOORD0,
	float3 tangent : TEXCOORD2,
	float3 bitangent : TEXCOORD3)
{
	VSOut result;

	float4x4 normalMatrix = worldMatrix[rootID * 2 + 1];

	float4 positionWS = mul(float4(position, 1.0f), worldMatrix[rootID * 2]);

	float4 positionVS = mul(viewMatrix, positionWS);

	result.position = positionWS;
	result.texCoord = texCoord;
	result.normal = mul(float4(normal.xyz, 1.0f), normalMatrix).xyz;
	result.tangent = mul(float4(tangent.xyz, 1.0f), normalMatrix).xyz;
	result.bitangent = mul(float4(bitangent.xyz, 1.0f), normalMatrix).xyz;
	result.viewDir = normalize(-positionVS.xyz);

	float d = -positionVS.z;
	float tess = saturate((500.0f - d) / (500.0f - 10.0f));

	//result.tessfactor = 1.0f + tess * (8.0f - 1.0f);
	result.tessfactor = 1.0f;

	return result;
}

struct HSOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal : TEXCOORD1;
	float3 tangent : TEXCOORD2;
	float3 bitangent : TEXCOORD3;
	float3 viewDir : TEXCOORD4;
};

struct PatchTess
{
	float EdgeTess[3] : SV_TessFactor;
	float InsideTess : SV_InsideTessFactor;
};

PatchTess PatchHS(InputPatch<VSOut, 3> patch,
	uint patchID : SV_PrimitiveID)
{
	PatchTess pt;

	pt.EdgeTess[0] = 0.5f * (patch[1].tessfactor + patch[2].tessfactor);
	pt.EdgeTess[1] = 0.5f * (patch[2].tessfactor + patch[0].tessfactor);
	pt.EdgeTess[2] = 0.5f * (patch[0].tessfactor + patch[1].tessfactor);
	pt.InsideTess = pt.EdgeTess[0];

	return pt;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("PatchHS")]
HSOut HS(InputPatch<VSOut, 3> p, uint i : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
	HSOut result;

	result.position = p[i].position;
	result.texCoord = p[i].texCoord;
	result.normal = p[i].normal;
	result.tangent = p[i].tangent;
	result.bitangent = p[i].bitangent;
	result.viewDir = p[i].viewDir;

	return result;
}

struct DSOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal : TEXCOORD1;
	float3 tangent : TEXCOORD2;
	float3 bitangent : TEXCOORD3;
	float3 viewDir : TEXCOORD4;
};

[domain("tri")]
DSOut DS(PatchTess tess, float3 bary : SV_DomainLocation, const OutputPatch<HSOut, 3> tri)
{
	DSOut result;

	result.position = bary.x * tri[0].position + bary.y * tri[1].position + bary.z * tri[2].position;
	result.texCoord = bary.x * tri[0].texCoord + bary.y * tri[1].texCoord + bary.z * tri[2].texCoord;
	result.normal = bary.x * tri[0].normal + bary.y * tri[1].normal + bary.z * tri[2].normal;
	result.tangent = bary.x * tri[0].tangent + bary.y * tri[1].tangent + bary.z * tri[2].tangent;
	result.bitangent = bary.x * tri[0].bitangent + bary.y * tri[1].bitangent + bary.z * tri[2].bitangent;
	result.viewDir = bary.x * tri[0].viewDir + bary.y * tri[1].viewDir + bary.z * tri[2].viewDir;

	result.normal = normalize(result.normal);
	result.tangent = normalize(result.tangent);
	result.bitangent = normalize(result.bitangent);
	result.viewDir = normalize(result.viewDir);

	float h = height.SampleLevel(anisoSampler, result.texCoord, 0.0f).x * 2.0f - 1.0f;
	//result.position = result.position + float4(result.normal.xyz, 0.0f) * h * 2.5f;
	result.position = result.position + float4(result.normal.xyz, 0.0f) * 0.0f;

	result.position = mul(projectionMatrix, mul(viewMatrix, result.position));
	result.normal = mul(viewMatrix, float4(result.normal, 0.0f)).xyz;
	result.tangent = mul(viewMatrix, float4(result.tangent, 0.0f)).xyz;
	result.bitangent = mul(viewMatrix, float4(result.bitangent, 0.0f)).xyz;

	return result;
}

struct PSOut
{
	float4 color : SV_TARGET0;
	float4 normal : SV_TARGET1;
};

PSOut PS(DSOut input)
{
	PSOut result;

	float3x3 tangentFrame = float3x3(input.tangent, -input.bitangent, input.normal);

	float3 viewDir = normalize(input.viewDir);
	float3 tangentViewDir = normalize(mul(tangentFrame, viewDir));

	float2 texCoord = input.texCoord;

	float4 diffuseMap = diffuse.Sample(anisoSampler, texCoord);
	//result.color = diffuseMap;
	result.color = float4(pow(max(diffuseMap.x, 0.0f), 2.2f), pow(max(diffuseMap.y, 0.0f), 2.2f), pow(max(diffuseMap.z, 0.0f), 2.2f), diffuseMap.w);
	//result.color = float4(0.0f, 1.0f, 0.0f, 1.0f);
	//result.color = float4(input.tessfactor, input.tessfactor, input.tessfactor, 1.0f);

	float3 normalsMap = normalize(mul(normals.Sample(anisoSampler, texCoord).xyz * 2.0f - 1.0f, tangentFrame));
	float metallicMap = metallic.Sample(anisoSampler, texCoord).x;
	float roughnessMap = roughness.Sample(anisoSampler, texCoord).x;
	result.normal = float4(EncodeSpheremap(normalsMap), metallicMap, roughnessMap);

	return result;
}