#include "Normals.hlsli"

cbuffer dataBuffer : register(b0)
{
	float4x4 projectionInverse;
	float4x4 viewInverse;
	float4 offset;
	float4 scale;
	uint2 dimensions;
	float2 resolution;
	float4 cameraPos;
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

float ConeAngleFromRoughness(float roughness)
{
	return max(0.05f, roughness * roughness);
}

float4 ConeTraceGI(in float3 P, in float3 V, in float cone_ratio, in float max_dist, in float step_mult)
{
	float min_voxel_diameter = resolution.y;
	float min_voxel_diameter_inv = 1.0 / min_voxel_diameter;

	float4 accum = 0.f;
	uint steps = 0;
	float factor = 0.2f;

	// push out the starting point to avoid self-intersection
	float dist = min_voxel_diameter;

	while (dist <= max_dist && accum.a < 1.0f)
	{
		float sample_diameter = max(min_voxel_diameter, cone_ratio * dist);

		float sample_lod = log2(sample_diameter * min_voxel_diameter_inv);

		float3 sample_pos = P + V * dist;

		dist += sample_diameter * step_mult;

		float4 sample_value = voxelData.SampleLevel(shadowSampler, sample_pos, sample_lod);

		float a = 1.0 - accum.a;
		accum += sample_value * a;
	}

	return accum;
}

float sqr(in float x)
{
	return x * x;
}

// Schlick's approximation of the fresnel term
float F_schlick(in float f0, in float LoH)
{
	// only have specular if f0 isn't 0
	//float enable = float(dot(f0, 1.0f) > 0.0f);
	return (f0 + (1.0f - f0) * pow(1.0f - LoH, 5.0f));
}

// Optimizied Schlick
// http://seblagarde.wordpress.com/2011/08/17/hello-world/
float SphericalGaussianApprox(in float CosX, in float ModifiedSpecularPower)
{
	return exp2(ModifiedSpecularPower * CosX - ModifiedSpecularPower);
}

#define OneOnLN2_x6 8.656170 // == 1/ln(2) * 6   (6 is SpecularPower of 5 + 1)

float3 F_schlick_opt(in float3 SpecularColor, in float3 E, in float3 H)
{
	// In this case SphericalGaussianApprox(1.0f - saturate(dot(E, H)), OneOnLN2_x6) is equal to exp2(-OneOnLN2_x6 * x)
	return SpecularColor + (1.0f - SpecularColor) * exp2(-OneOnLN2_x6 * saturate(dot(E, H)));
}

// Microfacet Models for Refraction through Rough Surfaces
// Walter et al.
// http://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.html
// aka Towbridge-Reitz
float D_ggx(in float alpha, in float NoH)
{
	float a2 = alpha*alpha;
	float cos2 = NoH*NoH;

	return (1.0f / 3.141592f) * sqr(alpha / (cos2 * (a2 - 1) + 1));

	/*
	// version from the paper, eq 33
	float CosSquared = NoH*NoH;
	float TanSquared = (1.0f - CosSquared)/CosSquared;
	return (1.0f/M_PI) * sqr(alpha/(CosSquared * (alpha*alpha + TanSquared)));
	*/
}

// Smith GGX with denominator
// http://graphicrants.blogspot.co.uk/2013/08/specular-brdf-reference.html
float G_smith_ggx(in float a, in float NoV, in float NoL)
{
	float a2 = a*a;
	float G_V = NoV + sqrt((NoV - NoV * a2) * NoV + a2);
	float G_L = NoL + sqrt((NoL - NoL * a2) * NoL + a2);
	return rcp(G_V * G_L);
}

// Schlick GGX
// http://graphicrants.blogspot.co.uk/2013/08/specular-brdf-reference.html
float G_UE4(in float alpha, in float NoV)
{
	float k = alpha / 2;
	return NoV / (NoV * (1 - k) + k);
}

float G_implicit(in float NoV, in float NoL)
{
	return NoL * NoV;
}

// Beckmann distribution
float D_beckmann(in float m, in float t)
{
	float M = m*m;
	float T = t*t;
	return exp((T - 1) / (M*T)) / (M*T*T);
}

// Helper to convert roughness to Phong specular power
float alpha_to_spec_pow(in float a)
{
	return 2.0f / (a * a) - 2.0f;
}

// Helper to convert Phong specular power to alpha
float spec_pow_to_alpha(in float s)
{
	return sqrt(2.0f / (s + 2.0f));
}

// Blinn Phong with conversion functions for roughness
float D_blinn_phong(in float n, in float NoH)
{
	float alpha = spec_pow_to_alpha(n);

	return (1.0f / (3.141592f*alpha*alpha)) * pow(NoH, n);
}

// Cook-Torrance specular BRDF + diffuse
float3 brdf(in float3 L, in float3 V, in float3 N, in float3 cdiff, in float3 cspec, in float roughness)
{
	float alpha = roughness*roughness;

	float3 H = normalize(L + V);

	float NoL = dot(N, L);
	float NoV = dot(N, V);
	float NoH = dot(N, H);
	float LoH = dot(L, H);

	// refractive index
	float n = 1.5;
	float f0 = pow((1 - n) / (1 + n), 2);

	// the fresnel term
	float F = F_schlick(f0, LoH);

	// the geometry term
	float G = G_UE4(alpha, NoV);

	// the NDF term
	float D = D_ggx(alpha, NoH);

	// specular term
	float3 Rs = cspec / 3.141592f *
		(F * G * D) /
		(4 * NoL * NoV);

	// diffuse fresnel, can be cheaper as 1-f0
	float Fd = F_schlick(f0, NoL);

	float3 Rd = cdiff / 3.141592f * (1.0f - Fd);

	return (Rd + Rs);
}

float4 GlobalIllumination(in float2 tc, in float3 P, in float3 N, in float3 V)
{
	float3 vP = P;

	float3 diffdir = normalize(N.zxy);
	float3 crossdir = cross(N.xyz, diffdir);
	float3 crossdir2 = cross(N.xyz, crossdir);

	float j = 1.0 + (frac(sin(dot(tc, float2(12.9898, 78.233))) * 43758.5453)) * 0.2;

	/*float3 directions[5] =
	{
		N,
		normalize(crossdir   * j + N),
		normalize(-crossdir * j + N),
		normalize(crossdir2  * j + N),
		normalize(-crossdir2 * j + N)
	};

	float diff_angle = 0.75f;

	float4 diffuse = float4(0, 0, 0, 0);

	for (uint d = 0; d < 5; ++d)
	{
		float3 D = directions[d];
		float3 vD = D;
		float3 vN = N;

		vP += normalize(vN) * resolution.y;

		float NdotL = saturate(dot(normalize(N), normalize(D)));

		// TODO: add actual material properties here
		float4 f = float4(brdf(normalize(D), V, N, 1, 0, 1), 1);

		diffuse += ConeTraceGI(vP, normalize(vD), diff_angle, 1.0f, 1.0f) * NdotL * f;
		//diffuse += ConeTraceGI(vP, normalize(vD), diff_angle, 1.0f, 1.0f) * NdotL;
	}

	diffuse *= 4.0f * 3.141592f / 5.0f;*/

	float3 directions[9] =
	{
		N,
		normalize(crossdir   * j + N),
		normalize(-crossdir  * j + N),
		normalize(crossdir2  * j + N),
		normalize(-crossdir2 * j + N),
		normalize((crossdir + crossdir2)  * j + N),
		normalize((crossdir - crossdir2)  * j + N),
		normalize((-crossdir + crossdir2) * j + N),
		normalize((-crossdir - crossdir2) * j + N),
	};

	float diff_angle = 0.6f;

	float4 diffuse = float4(0, 0, 0, 0);

	for (uint d = 0; d < 9; ++d)
	{
		float3 D = directions[d];
		float3 vD = D;
		float3 vN = N;

		vP += normalize(vN) * resolution.y;

		float NdotL = saturate(dot(normalize(N), normalize(D)));

		// TODO: add actual material properties here
		float4 f = float4(brdf(normalize(D), V, N, 1, 0, 1), 1);

		diffuse += ConeTraceGI(vP, normalize(vD), diff_angle, 1.0f, 1.0f) * NdotL * f;
		//diffuse += ConeTraceGI(vP, normalize(vD), diff_angle, 1.0f, 1.0f) * NdotL;
	}

	diffuse *= 4.0f * 3.141592f / 9.0f;

	return diffuse / 3.141592f;
}


/*float4 GlobalIlluminationRandom(in float2 tc, in float3 P, in float3 N, in float3 V)
{
	float3 vP = P;

	float3 diffdir = normalize(N.zxy);
	float3 crossdir = cross(N.xyz, diffdir);
	float3 crossdir2 = cross(N.xyz, crossdir);

	float j = 1.0 + (frac(sin(dot(tc, float2(12.9898, 78.233))) * 43758.5453)) * 0.2;
	
	float diff_angle = 0.05f;

	float4 diffuse = float4(0, 0, 0, 0);

	for (uint d = 0; d < 8; ++d)
	{
		float3 D = float3(frac(sin(dot(tc, float2(12.9898 + 7.424 * d, 78.233 - 3.149 * d))) * 43758.5453), 
			frac(sin(dot(tc, float2(12.9898 + 8.312 * d, 78.233 + 6.24 * d))) * 43758.5453), frac(sin(dot(tc, float2(12.9898 - 3.81 * d, 78.233 + 14.44 * d))) * 43758.5453));
		D = normalize(D * 2.0 - 1.0);
		if (dot(D, N) < 0.0)
		{
			D = -D;
		}
		float3 vD = D;
		float3 vN = N;

		vP += normalize(vN) * resolution.y;

		float NdotL = saturate(dot(normalize(N), normalize(D)));

		// TODO: add actual material properties here
		float4 f = float4(brdf(normalize(D), V, N, 1, 0, 1), 1);

		diffuse += ConeTraceGI(vP, normalize(vD), diff_angle, 1.0f, 1.0f) * NdotL * f;
		//diffuse += ConeTraceGI(vP, normalize(vD), diff_angle, 1.0f, 1.0f) * NdotL;
	}

	diffuse *= 4.0f * 3.141592f / 8.0f;

	return diffuse / 3.141592f;
}*/

/*
#ifdef DVCT
float4 diffuse_from_vct(in float2 tc, in float3 P, in float3 N, in float3 V, bool is_real)
#else
float4 diffuse_from_vct(in float2 tc, in float3 P, in float3 N, in float3 V)
#endif
{
    float3 vP = mul(world_to_svo, float4(P, 1.0)).xyz;

    // TODO: generate better cones! This hack will likely fail at some point
    float3 diffdir = normalize(N.zxy);
    float3 crossdir = cross(N.xyz, diffdir);
    float3 crossdir2 = cross(N.xyz, crossdir);

    // jitter cones
    float j = 1.0 + (frac(sin(dot(tc, float2(12.9898, 78.233))) * 43758.5453)) * 0.2;

    float3 directions[9] =
    {
        N,
        normalize(crossdir   * j + N),
        normalize(-crossdir  * j + N),
        normalize(crossdir2  * j + N),
        normalize(-crossdir2 * j + N),
        normalize((crossdir + crossdir2)  * j + N),
        normalize((crossdir - crossdir2)  * j + N),
        normalize((-crossdir + crossdir2) * j + N),
        normalize((-crossdir - crossdir2) * j + N),
    };

    float diff_angle = 0.6f;

    float4 diffuse = float4(0, 0, 0, 0);

#define num_d 9

    for (uint d = 0; d < num_d; ++d)
    {
        float3 D = directions[d];
        float3 vD = mul(world_to_svo, float4(D, 0.0)).xyz;
        float3 vN = mul(world_to_svo, float4(N, 0.0)).xyz;

        vP += normalize(vN) / SVO_SIZE;

        float NdotL = saturate(dot(normalize(N), normalize(D)));

        // TODO: add actual material properties here
        float4 f = float4(brdf(normalize(D), V, N, 1, 0, 1), 1);

#ifdef DVCT
        diffuse += trace_cone(vP, normalize(vD), diff_angle, 5, 20, is_real) * NdotL * f;
#else
        diffuse += trace_cone(vP, normalize(vD), diff_angle, 2, 5) * NdotL * f;
#endif
    }

    diffuse *= 4.0 * M_PI / num_d;

    return diffuse / M_PI;
}*/
float4 PS(VSOut input, uint sampleID : SV_SampleIndex) : SV_TARGET
{
	uint2 coord = uint2(input.texCoord.x * dimensions.x, input.texCoord.y * dimensions.y);
	
	float3 position = PositionFromDepth(sourceDepth.Load(coord, sampleID), input.texCoord.xy);
	float4 data = sourceNormals.Load(coord, sampleID);
	float3 normals = DecodeSpheremap(data.xy);

	float4 positionWS = mul(viewInverse, float4(position, 1.0f));
	float4 normalsWS = mul(viewInverse, float4(normals, 0.0f));
	float4 viewWS = normalize(positionWS - cameraPos);

	float3 voxelCoord = (positionWS.xyz + offset.xyz) * scale.xyz;
	//float4 globalIllum = ConeTraceGI(voxelCoord + normalsWS.xyz * resolution.y * 2.5f, normalsWS.xyz, 1.0f, 1.0f, 1.0f);
	float4 globalIllum = GlobalIllumination(input.texCoord.xy, voxelCoord + normalsWS.xyz * resolution.y, normalsWS.xyz, viewWS.xyz);
	//float4 globalIllum = GlobalIlluminationRandom(input.texCoord.xy, voxelCoord + normalsWS.xyz * resolution.y, normalsWS.xyz, viewWS.xyz);
	//float4 globalIllum = voxelData.SampleLevel(shadowSampler, voxelCoord.xyz, 0.0f);
	
	return float4(globalIllum.xyz, 1.0f);
}