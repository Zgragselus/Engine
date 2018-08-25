#include "Normals.hlsli"

cbuffer dataBuffer : register(b0)
{
	float4x4 projectionInverse;
	float4x4 viewInverse;
	float4 offset;
	float4 scale;
	uint2 dimensions;
	float2 resolution;
};

Texture2DMS<float> sourceDepth : register(t0);
Texture2DMS<float4> sourceNormals : register(t1);
Texture3D voxelData : register(t2);

SamplerState shadowSampler : register(s0);

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

float3 PositionFromDepth(float z, float2 texCoord)
{
	// Get x/w and y/w from the viewport position
	float x = texCoord.x * 2 - 1;
	float y = (1 - texCoord.y) * 2 - 1;
	float4 projPos = float4(x, y, z, 1.0f);

	// Transform by the inverse projection matrix
	float4 positionViewSpace = mul(projectionInverse, projPos);
	
	// Divide by w to get the view-space position
	return positionViewSpace.xyz / positionViewSpace.w;
}

float ConeTraceAO(in float3 P, in float3 V, in float cone_ratio, in float max_dist, in float step_mult)
{
	float min_voxel_diameter = resolution.y;
	float min_voxel_diameter_inv = 1.0 / min_voxel_diameter;

	float accum = 0.f;
	float factor = 0.2f;

	// push out the starting point to avoid self-intersection
	float dist = min_voxel_diameter;

	while (dist <= max_dist && accum < 1.0f)
	{
		float sample_diameter = max(min_voxel_diameter, cone_ratio * dist);

		float sample_lod = log2(sample_diameter * min_voxel_diameter_inv);

		float3 sample_pos = P + V * dist;

		dist += sample_diameter * step_mult;

		float sample_value = voxelData.SampleLevel(shadowSampler, sample_pos, sample_lod).a * factor;

		float a = 1.0 - accum;
		accum += sample_value * a;
	}

	float shadowing = saturate(1.0 - accum);

	return shadowing;
}

float4 PS(VSOut input, uint sampleID : SV_SampleIndex) : SV_TARGET
{
	uint2 coord = uint2(input.texCoord.x * dimensions.x, input.texCoord.y * dimensions.y);
	
	float3 position = PositionFromDepth(sourceDepth.Load(coord, sampleID), input.texCoord.xy);
	float4 data = sourceNormals.Load(coord, sampleID);
	float3 normals = DecodeSpheremap(data.xy);

	float4 positionWS = mul(viewInverse, float4(position, 1.0f));
	float4 normalsWS = mul(viewInverse, float4(normals, 0.0f));

	float3 voxelCoord = (positionWS.xyz + offset.xyz) * scale.xyz;
	float ao = ConeTraceAO(voxelCoord + normalsWS.xyz * resolution.y, normalsWS.xyz, 0.8, 0.1, 1.0);

	float4 result = float4(ao, ao, ao, 1.0f);

	return result;
}