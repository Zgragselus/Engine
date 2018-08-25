#include "Normals.hlsli"
#include "Lighting.hlsli"
#include "BRDF.hlsli"

cbuffer dataBuffer : register(b0)
{
	float4x4 projectionInverse;
	float4x4 viewInverse;
	uint2 dimensions;
	float2 pad2;
	float4 pad[11];
};

struct InputData
{
	unsigned int id;
};

cbuffer lightType : register(b1)
{
	InputData lightInput;
};

Texture2DMS<float4> sourceAlbedo : register(t0);
Texture2DMS<float4> sourceNormals : register(t1);
Texture2DMS<float> sourceDepth : register(t2);
Texture2D<float2> shadowMap : register(t3);
Texture2DMS<float4> sourceReflection : register(t4);
Texture2DMS<float4> sourceAO : register(t5);
Texture2DMS<float4> sourceGI : register(t6);

StructuredBuffer<Light> lightsData : register(t7);
StructuredBuffer<ShadowVirtualTextureRecord> shadowAtlasData : register(t8);

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

/*static const uint sampleCount = 16;
static const float poissonRadius = 1.0f;
static float2 poissonDiskValues[sampleCount] =
{
	float2(-0.1307115f, 0.1291686f),
	float2(0.6154836f, 0.3147851f),
	float2(-0.4295029f, -0.155486f),
	float2(-0.591889f, 0.6969846f),
	float2(0.2217633f, -0.3848645f),
	float2(-0.2653467f, -0.6504169f),
	float2(-0.1259185f, 0.8731781f),
	float2(0.3423603f, 0.01230872f),
	float2(0.1190722f, 0.4535096f),
	float2(-0.7164063f, 0.1303319f),
	float2(0.6048174f, -0.5608257f),
	float2(0.1267647f, -0.8851751f),
	float2(0.8812442f, -0.224937f),
	float2(0.4235326f, 0.8168082f),
	float2(-0.9404536f, -0.2363012f),
	float2(-0.7055491f, -0.7052079f)
};

static float levelTexel[12] = { 1.0f / 2048.0f, 1.0f / 1024.0f, 1.0f / 512.0f, 1.0f / 256.0f, 1.0f / 128.0f, 1.0f / 64.0f, 1.0f / 32.0f, 1.0f / 16.0f, 1.0f / 8.0f, 1.0f / 4.0f, 1.0f / 2.0f, 1.0f };
static float levelSize[12] = { 2048.0f, 1024.0f, 512.0f, 256.0f, 128.0f, 64.0f, 32.0f, 16.0f, 8.0f, 4.0f, 2.0f, 1.0f };

float ChebyshevsUpperBound(float depth, float2 projCoord)
{
	float2 moments = shadowMap.SampleLevel(shadowSampler, projCoord.xy, 0.0f).xy;
	
	float variance = moments.y - (moments.x * moments.x);
	variance = max(variance, 0.00002f);

	float d = depth - moments.x;
	float p_max = variance / (variance + d * d);

	if (depth <= moments.x)
	{
		p_max = 1.0f;
	}
	
	return p_max;
}

float ShadowMapPCF(float3 projCoord, int size)
{
	float shadow = 0.0f;

	for (int i = -size; i < size; i++)
	{
		for (int j = -size; j < size; j++)
		{
			shadow += shadowMap.SampleLevel(shadowSampler, projCoord.xy + float2(i, j) * float2(1.0f / 2048.0, 1.0f / 2048.0), 0.0f).x < projCoord.z ? 0.0f : 1.0f;
		}
	}

	return shadow / (float)((2 * size + 1) * (2 * size + 1));
}

float CubicHermite(float A, float B, float C, float D, float t)
{
	float t2 = t * t;
	float t3 = t * t*t;
	float a = -A / 2.0 + (3.0*B) / 2.0 - (3.0*C) / 2.0 + D / 2.0;
	float b = A - (5.0*B) / 2.0 + 2.0*C - D / 2.0;
	float c = -A / 2.0 + C / 2.0;
	float d = B;

	return a * t3 + b * t2 + c * t + d;
}

float ShadowMapBicubic(float3 projCoord)
{
	float c_onePixel = 1.0 / 2048.0;
	float c_twoPixels = 2.0 / 2048.0;

	float2 grad = frac(projCoord.xy * 2048.0 + 0.5f);

	float2 pixel = projCoord.xy * 2048.0 + 0.5f;

	float2 fracTmp = frac(pixel);
	pixel = floor(pixel) / 2048.0 - float2(c_onePixel / 2.0, c_onePixel / 2.0);

	float C00 = shadowMap.SampleLevel(shadowSampler, pixel + float2(-c_onePixel, -c_onePixel), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C10 = shadowMap.SampleLevel(shadowSampler, pixel + float2(0.0, -c_onePixel), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C20 = shadowMap.SampleLevel(shadowSampler, pixel + float2(c_onePixel, -c_onePixel), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C30 = shadowMap.SampleLevel(shadowSampler, pixel + float2(c_twoPixels, -c_onePixel), 0.0).x  < projCoord.z ? 0.0f : 1.0f;

	float C01 = shadowMap.SampleLevel(shadowSampler, pixel + float2(-c_onePixel, 0.0), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C11 = shadowMap.SampleLevel(shadowSampler, pixel + float2(0.0, 0.0), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C21 = shadowMap.SampleLevel(shadowSampler, pixel + float2(c_onePixel, 0.0), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C31 = shadowMap.SampleLevel(shadowSampler, pixel + float2(c_twoPixels, 0.0), 0.0).x  < projCoord.z ? 0.0f : 1.0f;

	float C02 = shadowMap.SampleLevel(shadowSampler, pixel + float2(-c_onePixel, c_onePixel), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C12 = shadowMap.SampleLevel(shadowSampler, pixel + float2(0.0, c_onePixel), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C22 = shadowMap.SampleLevel(shadowSampler, pixel + float2(c_onePixel, c_onePixel), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C32 = shadowMap.SampleLevel(shadowSampler, pixel + float2(c_twoPixels, c_onePixel), 0.0).x  < projCoord.z ? 0.0f : 1.0f;

	float C03 = shadowMap.SampleLevel(shadowSampler, pixel + float2(-c_onePixel, c_twoPixels), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C13 = shadowMap.SampleLevel(shadowSampler, pixel + float2(0.0, c_twoPixels), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C23 = shadowMap.SampleLevel(shadowSampler, pixel + float2(c_onePixel, c_twoPixels), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C33 = shadowMap.SampleLevel(shadowSampler, pixel + float2(c_twoPixels, c_twoPixels), 0.0).x  < projCoord.z ? 0.0f : 1.0f;

	float CP0X = CubicHermite(C00, C10, C20, C30, fracTmp.x);
	float CP1X = CubicHermite(C01, C11, C21, C31, fracTmp.x);
	float CP2X = CubicHermite(C02, C12, C22, C32, fracTmp.x);
	float CP3X = CubicHermite(C03, C13, C23, C33, fracTmp.x);

	return CubicHermite(CP0X, CP1X, CP2X, CP3X, fracTmp.y);
}

float ShadowMapBilinear(float3 projCoord)
{
	float2 grad = frac(projCoord.xy * 2048.0 + 0.5f);
	float4 tmp = shadowMap.Gather(shadowSampler, projCoord.xy);
	tmp.x = tmp.x < projCoord.z ? 0.0f : 1.0f;
	tmp.y = tmp.y < projCoord.z ? 0.0f : 1.0f;
	tmp.z = tmp.z < projCoord.z ? 0.0f : 1.0f;
	tmp.w = tmp.w < projCoord.z ? 0.0f : 1.0f;
	float shadow = lerp(lerp(tmp.w, tmp.z, grad.x), lerp(tmp.x, tmp.y, grad.x), grad.y);
	return shadow;
}

float ShadowMapBilinearPCF(float3 projCoord, int size)
{
	float shadow = 0.0f;
	float2 grad = frac(projCoord.xy * 2048.0f + 0.5f);

	for (int i = -size; i <= size; i++)
	{
		for (int j = -size; j <= size; j++)
		{
			float4 tmp = shadowMap.Gather(shadowSampler, projCoord.xy + float2(i, j) * float2(1.0f / 2048.0f, 1.0f / 2048.0f));
			tmp.x = tmp.x < projCoord.z ? 0.0f : 1.0f;
			tmp.y = tmp.y < projCoord.z ? 0.0f : 1.0f;
			tmp.z = tmp.z < projCoord.z ? 0.0f : 1.0f;
			tmp.w = tmp.w < projCoord.z ? 0.0f : 1.0f;
			shadow += lerp(lerp(tmp.w, tmp.z, grad.x), lerp(tmp.x, tmp.y, grad.x), grad.y);
		}
	}

	return shadow / (float)((2 * size + 1) * (2 * size + 1));
}

float ShadowMapBilinearMIP(float3 projCoord, int level)
{
	float shadow = 0.0f;
	float2 grad = frac(projCoord.xy * levelSize[level]);

	for (int i = -3; i < 4; i++)
	{
		for (int j = -3; j < 4; j++)
		{
			float4 tmp;
			tmp.x = shadowMap.SampleLevel(shadowSampler, projCoord.xy + float2(i, j) * float2(levelTexel[level], levelTexel[level]) + float2(0.0f, 0.0f), level).x < projCoord.z ? 0.0f : 1.0f;
			tmp.y = shadowMap.SampleLevel(shadowSampler, projCoord.xy + float2(i, j) * float2(levelTexel[level], levelTexel[level]) + float2(levelTexel[level], 0.0f), level).x < projCoord.z ? 0.0f : 1.0f;
			tmp.z = shadowMap.SampleLevel(shadowSampler, projCoord.xy + float2(i, j) * float2(levelTexel[level], levelTexel[level]) + float2(levelTexel[level], levelTexel[level]), level).x < projCoord.z ? 0.0f : 1.0f;
			tmp.w = shadowMap.SampleLevel(shadowSampler, projCoord.xy + float2(i, j) * float2(levelTexel[level], levelTexel[level]) + float2(0.0f, levelTexel[level]), level).x < projCoord.z ? 0.0f : 1.0f;
			shadow += lerp(lerp(tmp.x, tmp.y, grad.x), lerp(tmp.w, tmp.z, grad.x), grad.y);
		}
	}

	return shadow / 49.0f;
}

float ShadowMapBilinearMIPF(float3 projCoord, float level)
{
	int l = (int)level;
	float t0 = ShadowMapBilinearMIP(projCoord, l);
	float t1 = ShadowMapBilinearMIP(projCoord, l + 1);
	return lerp(t0, t1, level - (float)l);
}

float Random(float2 co)
{
	return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
}

float PCSS_BlockerDistance(float uvLightSize, float3 projCoord)
{
	int size = 2;
	int blockers = 0;
	float avgBlockerDistance = 0.0f;
	for (int i = 0; i < (int)sampleCount; i++)
	{
		float2 rand = Random(projCoord.xy + projCoord.xy * poissonDiskValues[i] + poissonDiskValues[i]);
		float z = shadowMap.SampleLevel(shadowSampler, projCoord.xy + rand * uvLightSize * float2(1.0f / 1024.0f, 1.0f / 1024.0f) + poissonDiskValues[i] * uvLightSize * float2(1.0f / 1024.0f, 1.0f / 1024.0f), 0.0f).x;
		if (z < projCoord.z)
		{
			blockers++;
			avgBlockerDistance += z;
		}
	}

	if (blockers > 0)
	{
		return avgBlockerDistance / blockers;
	}
	else
	{
		return -1.0f;
	}
}

float AdvancedBlocker(float uvLightSize, float3 projCoord)
{
	int size = 2;
	int blockers = 0;
	float avgBlockerDistance = 0.0f;
	for (int i = 0; i < (int)sampleCount; i++)
	{
		float z = shadowMap.SampleLevel(shadowSampler, projCoord.xy + poissonDiskValues[i] * uvLightSize * float2(1.0f / 128.0f, 1.0f / 128.0f), 0.0f).x;
		if (z < projCoord.z)
		{
			blockers++;
			avgBlockerDistance += z;
		}

		z = shadowMap.SampleLevel(shadowSampler, projCoord.xy + poissonDiskValues[i] * uvLightSize * float2(1.0f / 128.0f, 1.0f / 128.0f), 1.0f).x;
		if (z < projCoord.z)
		{
			blockers++;
			avgBlockerDistance += z;
		}

		z = shadowMap.SampleLevel(shadowSampler, projCoord.xy + poissonDiskValues[i] * uvLightSize * float2(1.0f / 128.0f, 1.0f / 128.0f), 2.0f).x;
		if (z < projCoord.z)
		{
			blockers++;
			avgBlockerDistance += z;
		}
	}

	if (blockers > 0)
	{
		return avgBlockerDistance / blockers;
	}
	else
	{
		return -1.0f;
	}
}

float PenumbraSize(float uvLightSize, float3 projCoord)
{
	float blockerDistance = AdvancedBlocker(uvLightSize, projCoord);
	float penumbraSize = max((projCoord.z - blockerDistance) * uvLightSize / blockerDistance, 0.0f);
	return penumbraSize;
}

float ShadowMapPCSS(float uvLightSize, float3 projCoord)
{
	int size = 2;

	float blockerDistance = PCSS_BlockerDistance(uvLightSize, projCoord);
	float penumbraSize = max((projCoord.z - blockerDistance) * uvLightSize / blockerDistance, 0.0f);
	
	float shadow = 0.0f;

	for (int i = 0; i < (int)sampleCount; i++)
	{
		float2 rand = Random(projCoord.xy + projCoord.xy * poissonDiskValues[i] + poissonDiskValues[i]);
		shadow += (shadowMap.SampleLevel(shadowSampler, projCoord.xy +
			(poissonDiskValues[i] + clamp((rand * 2.0f - 1.0f) * penumbraSize, -1.0f, 1.0f)) * float2(1.0f / 1024.0f, 1.0f / 1024.0f) * 1.0f * penumbraSize +
			clamp(rand * 2.0f - 1.0f, 0.0f, 1.0f) * float2(1.0f / 1024.0f, 1.0f / 1024.0f), 0.0f).x < projCoord.z) ? 0.0f : 1.0f;
	}

	return shadow / (float)sampleCount;
}*/

/*float3 EnvBRDFApprox(float3 specularColor, float roughness, float ndotv)
{
	const float4 c0 = float4(-1, -0.0275, -0.572, 0.022);
	const float4 c1 = float4(1, 0.0425, 1.04, -0.04);
	float4 r = roughness * c0 + c1;
	float a004 = min(r.x * r.x, exp2(-9.28 * ndotv)) * r.x + r.y;
	float AB = float2(-1.04, 1.04) * a004 + r.zw;
	return specularColor * AB.x + AB.y;
}*/

bool RayIntersect(float raya, float rayb, float z, float thickness) 
{
	if (raya > rayb) {
		float t = raya;
		raya = rayb;
		rayb = t;
	}

	return raya < z && rayb > z - thickness;
}

float4 PS(VSOut input, uint sampleID : SV_SampleIndex) : SV_TARGET
{
	uint2 coord = uint2(input.texCoord.x * dimensions.x, input.texCoord.y * dimensions.y);

	float4 diffuse = sourceAlbedo.Load(coord, sampleID);

	float4 data = sourceNormals.Load(coord, sampleID);
	float3 normals = DecodeSpheremap(data.xy);

#if SamplesMSAAGI == SamplesMSAA
	float ao = sourceAO.Load(coord, sampleID).x;
	float3 env = sourceReflection.Load(coord, sampleID).xyz;
#else
	float ao = sourceAO.Load(coord, 0).x;
	float3 env = sourceReflection.Load(coord, 0).xyz;
#endif

	float3 position = PositionFromDepth(sourceDepth.Load(coord, sampleID), input.texCoord.xy);

	float4 positionWS = mul(viewInverse, float4(position, 1.0f));

	float diffuseLight = 0.0f;
	float3 specularLight = 0.0f;
	float shadowMask = 1.0f;
	float roughnessE = data.w * data.w;
	float roughnessL = max(.01, roughnessE);
	
	float4 result = float4(0.0f, 0.0f, 0.0f, 1.0f);
	if (lightInput.id == (uint)(-1))
	{
#if SamplesMSAAGI == SamplesMSAA
		float3 gi = diffuse.xyz * sourceGI.Load(coord, sampleID).xyz + env * data.z;
#else
		float3 gi = diffuse.xyz * sourceGI.Load(coord, 0).xyz + env * data.z;
#endif

		return float4(gi, 1.0f);
		//return float4(0.0f, 0.0f, 0.0f, 1.0f);
	}
	else
	{
		if (lightsData[lightInput.id].type == 0)
		{
			float3 lightVec = lightsData[lightInput.id].positionVS.xyz - position.xyz;

			float lightDist = length(lightVec);

			float3 lightDir = normalize(lightVec);
			float3 viewDir = normalize(-position.xyz);
			float3 halfDir = normalize(viewDir + lightDir);

			float vdoth = saturate(dot(viewDir, halfDir));
			float ndoth = saturate(dot(normals, halfDir));
			float ndotv = saturate(dot(normals, viewDir));
			float ndotl = saturate(dot(normals, lightDir));

			float attenuation = 1.0f / (1.0f + lightDist * lightsData[lightInput.id].data0.x * 4.0f * 3.141592f + lightDist * lightDist * lightsData[lightInput.id].data0.y * 4.0f * 3.141592f);

			diffuseLight = DiffuseLambert(lightDir, normals) * attenuation;
			//diffuseLight = DiffuseOrenNayar(lightDir, viewDir, normals, roughnessL) * attenuation;
			//diffuseLight = max(dot(lightDir, normals), 0.0f) * attenuation;

			/*float3 lightF = FresnelTerm(vdoth);
			float lightD = DistributionTerm(roughnessL, ndoth);
			float lightV = VisibilityTerm(roughnessL, ndotv, ndotl);
			specularLight = lightF * (lightD * lightV * 3.141592f * ndotl) * attenuation;
			specularLight *= clamp(pow(abs(ndotv + ao), roughnessE) - 1.0f, 0.0f, 1.0f);*/

			specularLight = SpecularGGX(lightDir, viewDir, halfDir, normals, diffuse.xyz, data.w, ao, roughnessE, roughnessL) * attenuation;

			if (asint(lightsData[lightInput.id].data0.z) >= 0)
			{
				float3 shadowDir = normalize(lightsData[lightInput.id].position.xyz - positionWS.xyz);
				float3 shadowDirAbs = abs(shadowDir);
				float3 shadowDirSign = sign(shadowDir);
				uint shadowMax = 0;
				if (shadowDirAbs.x > shadowDirAbs.y)
				{
					if (shadowDirAbs.x > shadowDirAbs.z)
					{
						shadowMax = 0;
					}
					else
					{
						shadowMax = 2;
					}
				}
				else
				{
					if (shadowDirAbs.y > shadowDirAbs.z)
					{
						shadowMax = 1;
					}
					else
					{
						shadowMax = 2;
					}
				}

				if (shadowDirSign[shadowMax] < 0.0)
				{
					shadowMax = shadowMax * 2 + 1;
				}
				else
				{
					shadowMax = shadowMax * 2;
				}

				float4 shadowProj = float4(0.0f, 0.0f, 0.0f, 0.0f);
				if (shadowMax == 0)
				{
					shadowProj = mul(shadowAtlasData[asuint(lightsData[lightInput.id].data0.z)].shadowMatrix, positionWS);
					shadowProj.y = -shadowProj.y;
					shadowProj.xy /= shadowProj.w;
					shadowProj.xy *= 0.5;
					shadowProj.xy += 0.5;
					shadowProj.z /= (lightsData[lightInput.id].shadowFar - lightsData[lightInput.id].shadowNear);
					shadowProj.z -= lightsData[lightInput.id].offset;
					shadowProj.xy *= shadowAtlasData[asuint(lightsData[lightInput.id].data0.z)].size;
					shadowProj.xy += shadowAtlasData[asuint(lightsData[lightInput.id].data0.z)].offset;
				}
				else if (shadowMax == 1)
				{
					shadowProj = mul(shadowAtlasData[asuint(lightsData[lightInput.id].data0.w)].shadowMatrix, positionWS);
					shadowProj.y = -shadowProj.y;
					shadowProj.xy /= shadowProj.w;
					shadowProj.xy *= 0.5;
					shadowProj.xy += 0.5;
					shadowProj.z /= (lightsData[lightInput.id].shadowFar - lightsData[lightInput.id].shadowNear);
					shadowProj.z -= lightsData[lightInput.id].offset;
					shadowProj.xy *= shadowAtlasData[asuint(lightsData[lightInput.id].data0.w)].size;
					shadowProj.xy += shadowAtlasData[asuint(lightsData[lightInput.id].data0.w)].offset;
				}
				else if (shadowMax == 2)
				{
					shadowProj = mul(shadowAtlasData[asuint(lightsData[lightInput.id].data1.y)].shadowMatrix, positionWS);
					shadowProj.y = -shadowProj.y;
					shadowProj.xy /= shadowProj.w;
					shadowProj.xy *= 0.5;
					shadowProj.xy += 0.5;
					shadowProj.z /= (lightsData[lightInput.id].shadowFar - lightsData[lightInput.id].shadowNear);
					shadowProj.z -= lightsData[lightInput.id].offset;
					shadowProj.xy *= shadowAtlasData[asuint(lightsData[lightInput.id].data1.y)].size;
					shadowProj.xy += shadowAtlasData[asuint(lightsData[lightInput.id].data1.y)].offset;
				}
				else if (shadowMax == 3)
				{
					shadowProj = mul(shadowAtlasData[asuint(lightsData[lightInput.id].data1.x)].shadowMatrix, positionWS);
					shadowProj.y = -shadowProj.y;
					shadowProj.xy /= shadowProj.w;
					shadowProj.xy *= 0.5;
					shadowProj.xy += 0.5;
					shadowProj.z /= (lightsData[lightInput.id].shadowFar - lightsData[lightInput.id].shadowNear);
					shadowProj.z -= lightsData[lightInput.id].offset;
					shadowProj.xy *= shadowAtlasData[asuint(lightsData[lightInput.id].data1.x)].size;
					shadowProj.xy += shadowAtlasData[asuint(lightsData[lightInput.id].data1.x)].offset;
				}
				else if (shadowMax == 4)
				{
					shadowProj = mul(shadowAtlasData[asuint(lightsData[lightInput.id].data1.w)].shadowMatrix, positionWS);
					shadowProj.y = -shadowProj.y;
					shadowProj.xy /= shadowProj.w;
					shadowProj.xy *= 0.5;
					shadowProj.xy += 0.5;
					shadowProj.z /= (lightsData[lightInput.id].shadowFar - lightsData[lightInput.id].shadowNear);
					shadowProj.z -= lightsData[lightInput.id].offset;
					shadowProj.xy *= shadowAtlasData[asuint(lightsData[lightInput.id].data1.w)].size;
					shadowProj.xy += shadowAtlasData[asuint(lightsData[lightInput.id].data1.w)].offset;
				}
				else
				{
					shadowProj = mul(shadowAtlasData[asuint(lightsData[lightInput.id].data1.z)].shadowMatrix, positionWS);
					shadowProj.y = -shadowProj.y;
					shadowProj.xy /= shadowProj.w;
					shadowProj.xy *= 0.5;
					shadowProj.xy += 0.5;
					shadowProj.z /= (lightsData[lightInput.id].shadowFar - lightsData[lightInput.id].shadowNear);
					shadowProj.z -= lightsData[lightInput.id].offset;
					shadowProj.xy *= shadowAtlasData[asuint(lightsData[lightInput.id].data1.z)].size;
					shadowProj.xy += shadowAtlasData[asuint(lightsData[lightInput.id].data1.z)].offset;
				}

				if (lightsData[lightInput.id].shadowFilter == 1)
				{
					shadowMask *= ShadowMapBilinear(shadowMap, shadowSampler, shadowProj.xyz, lightsData[lightInput.id].shadowResolution, lightsData[lightInput.id].shadowScale);
				}
				else if (lightsData[lightInput.id].shadowFilter == 2)
				{
					shadowMask *= ShadowMapBicubic(shadowMap, shadowSampler, shadowProj.xyz, lightsData[lightInput.id].shadowResolution, lightsData[lightInput.id].shadowScale);
				}
				else if (lightsData[lightInput.id].shadowFilter == 3)
				{
					shadowMask *= ShadowMapPCF(shadowMap, shadowSampler, shadowProj.xyz, lightsData[lightInput.id].shadowResolution, lightsData[lightInput.id].shadowScale, uint(lightsData[lightInput.id].data3.x));
				}
				else if (lightsData[lightInput.id].shadowFilter == 4)
				{
					shadowMask *= ShadowMapPCSS(shadowMap, shadowSampler, shadowProj.xyz, lightsData[lightInput.id].shadowResolution, lightsData[lightInput.id].shadowScale, lightsData[lightInput.id].data3.x);
				}
				else if (lightsData[lightInput.id].shadowFilter == 5)
				{
					shadowMask *= ShadowMapPCMLSM(shadowMap, shadowSampler, shadowProj.xyz, lightsData[lightInput.id].shadowResolution, lightsData[lightInput.id].shadowScale, lightsData[lightInput.id].data3.x);
				}
				else
				{
					shadowMask *= ShadowMap(shadowMap, shadowSampler, shadowProj.xyz);
				}
			}

			shadowMask = clamp(shadowMask, 0.0f, 1.0f);

			float3 tmp = (specularLight + diffuse.xyz * diffuseLight) * lightsData[lightInput.id].color.xyz;
			//float3 tmp = float3(diffuseLight, diffuseLight, diffuseLight) * lightsData[lightInput.id].color.xyz;

			return float4(tmp.xyz * shadowMask, 1.0f);
		}
		else if (lightsData[lightInput.id].type == 1)
		{
			SpotLight spotLight = (SpotLight)lightsData[lightInput.id];

			float3 lightVec = lightsData[lightInput.id].positionVS.xyz - position.xyz;

			float lightDist = length(lightVec);

			float3 lightDir = normalize(lightVec);
			float3 viewDir = normalize(-position.xyz);
			float3 halfDir = normalize(viewDir + lightDir);

			float vdoth = saturate(dot(viewDir, halfDir));
			float ndoth = saturate(dot(normals, halfDir));
			float ndotv = saturate(dot(normals, viewDir));
			float ndotl = saturate(dot(normals, lightDir));

			float attenuation = 1.0f / (1.0f + lightDist * spotLight.spotLinearAtt * 3.141592f + lightDist * lightDist * spotLight.spotQuadraticAtt * 3.141592f);
			//float spot = dot(-lightDir, spotLight.spotDirectionVS.xyz) > spotLight.spotFalloff ? 1.0f : 0.0f;
			float spot = clamp(16.0 * (dot(-lightDir, spotLight.spotDirectionVS.xyz) - spotLight.spotFalloff), 0.0f, 1.0f);

			diffuseLight = DiffuseLambert(lightDir, normals) * attenuation * spot;
			//diffuseLight = DiffuseOrenNayar(lightDir, viewDir, normals, roughnessL) * attenuation * spot;
			//diffuseLight = max(dot(lightDir, normals), 0.0f) * attenuation * spot;

			/*float3 lightF = FresnelTerm(vdoth);
			float lightD = DistributionTerm(roughnessL, ndoth);
			float lightV = VisibilityTerm(roughnessL, ndotv, ndotl);
			specularLight = lightF * (lightD * lightV * 3.141592f * ndotl) * attenuation;
			specularLight *= clamp(pow(abs(ndotv + ao), roughnessE) - 1.0f, 0.0f, 1.0f) * spot;*/

			specularLight = SpecularGGX(lightDir, viewDir, halfDir, normals, diffuse.xyz, data.w, ao, roughnessE, roughnessL) * attenuation * spot;

			if (spotLight.shadowID >= 0)
			{
				float4 positionProj = mul(shadowAtlasData[spotLight.shadowID].shadowMatrix, positionWS);
				positionProj.xy /= positionProj.w;
				positionProj.y = -positionProj.y;
				positionProj.x *= 0.5f;
				positionProj.x += 0.5f;
				positionProj.y *= 0.5f;
				positionProj.y += 0.5f;
				positionProj.z /= (spotLight.shadowFar - spotLight.shadowNear);
				positionProj.z -= lightsData[lightInput.id].offset;

				positionProj.xy *= shadowAtlasData[spotLight.shadowID].size;
				positionProj.xy += shadowAtlasData[spotLight.shadowID].offset;

				//float shadow = ShadowMap(shadowMap, shadowSampler, positionProj.xyz);
				//float shadow = ShadowMapPCF(positionProj.xyz, 1);
				//float shadow = ShadowMapBilinear(positionProj.xyz);
				//float shadow = ShadowMapBilinearPCF(positionProj.xyz, 1);
				//float shadow = ShadowMapPCSS(1.0f, positionProj.xyz);

				if (lightsData[lightInput.id].shadowFilter == 1)
				{
					shadowMask *= ShadowMapBilinear(shadowMap, shadowSampler, positionProj.xyz, lightsData[lightInput.id].shadowResolution, lightsData[lightInput.id].shadowScale);
				}
				else if (lightsData[lightInput.id].shadowFilter == 2)
				{
					shadowMask *= ShadowMapBicubic(shadowMap, shadowSampler, positionProj.xyz, lightsData[lightInput.id].shadowResolution, lightsData[lightInput.id].shadowScale);
				}
				else if (lightsData[lightInput.id].shadowFilter == 3)
				{
					shadowMask *= ShadowMapPCF(shadowMap, shadowSampler, positionProj.xyz, lightsData[lightInput.id].shadowResolution, lightsData[lightInput.id].shadowScale, uint(lightsData[lightInput.id].data3.x));
				}
				else if (lightsData[lightInput.id].shadowFilter == 4)
				{
					shadowMask *= ShadowMapPCSS(shadowMap, shadowSampler, positionProj.xyz, lightsData[lightInput.id].shadowResolution, lightsData[lightInput.id].shadowScale, lightsData[lightInput.id].data3.x);
				}
				else if (lightsData[lightInput.id].shadowFilter == 5)
				{
					shadowMask *= ShadowMapPCMLSM(shadowMap, shadowSampler, positionProj.xyz, lightsData[lightInput.id].shadowResolution, lightsData[lightInput.id].shadowScale, lightsData[lightInput.id].data3.x);
				}
				else if (lightsData[lightInput.id].shadowFilter == 6)
				{
					float4 origin = mul(shadowAtlasData[spotLight.shadowID].shadowMatrix, positionWS);
					origin.z /= (spotLight.shadowFar - spotLight.shadowNear);
					origin.z -= lightsData[lightInput.id].offset;

					float areaShadows = 0.0f;
					
					for (uint i = 0; i < PCSS_SampleCount; i++)
					{
						//float2 rand = normalize(PCSS_Samples[i] + (float2(Random(origin.xy / origin.w + diffuse.xy + PCSS_Samples[i]), Random(origin.xy / origin.w + diffuse.xz + PCSS_Samples[i])) * 2.0f - 1.0f));
						float2 rand = normalize(PCSS_Samples[i] + (float2(Random(PCSS_Samples[i] + input.texCoord.xy * 2.0f - 1.0f), Random(PCSS_Samples[i] + diffuse.xy * 2.0f - 1.0f)) * 2.0f - 1.0f));
						//float2 rand = PCSS_Samples[i];

						//float3 direction = normalize(float3(0.5f + PCSS_Samples[i].x * lightsData[lightInput.id].data3.x, 0.5f + PCSS_Samples[i].y * lightsData[lightInput.id].data3.x, 0.0f) - origin.xyz);
						float4 direction = (float4(rand.x * lightsData[lightInput.id].data3.x, rand.y * lightsData[lightInput.id].data3.x, 0.0f, 0.0f) * 10.0f - origin) / 64.0f;
						float stepSize = 1.0f;
						float shadow = 0.0f;

						float4 tmp = origin;
						
						float4 shadowCoord = tmp;
						shadowCoord.xy /= shadowCoord.w;
						shadowCoord.y = -shadowCoord.y;
						shadowCoord.x *= 0.5f;
						shadowCoord.x += 0.5f;
						shadowCoord.y *= 0.5f;
						shadowCoord.y += 0.5f;
						shadowCoord.xy *= shadowAtlasData[spotLight.shadowID].size;
						shadowCoord.xy += shadowAtlasData[spotLight.shadowID].offset;
						float2 prev = float2(shadowMap.SampleLevel(shadowSampler, shadowCoord.xy, 0.0f).x, shadowCoord.z);
						float2 curr = prev;

						[loop] for (int j = 0; j < 64; j++)
						{
							[branch] if (RayIntersect(curr.y, prev.y, prev.x, 0.05f))
							{
								shadow = 1.0f;
								break;
							}
							
							prev = curr;

							tmp += direction * stepSize;

							shadowCoord = tmp;
							shadowCoord.xy /= shadowCoord.w;
							shadowCoord.y = -shadowCoord.y;
							shadowCoord.x *= 0.5f;
							shadowCoord.x += 0.5f;
							shadowCoord.y *= 0.5f;
							shadowCoord.y += 0.5f;

							if (shadowCoord.x <= 0.01f || shadowCoord.x >= 0.99f || shadowCoord.y <= 0.0f || shadowCoord.y >= 0.99f)
							{
								shadow = 0.0f;
								break;
							}

							shadowCoord.xy *= shadowAtlasData[spotLight.shadowID].size;
							shadowCoord.xy += shadowAtlasData[spotLight.shadowID].offset;

							if (tmp.z <= 0.0f)
							{
								shadow = 0.0f;
								break;
							}

							curr = float2(shadowMap.SampleLevel(shadowSampler, shadowCoord.xy, 0.0f).x, shadowCoord.z);

							//rand = normalize(PCSS_Samples[i] + float2(Random(origin.xy / origin.w + diffuse.xy + PCSS_Samples[i]), Random(origin.xy / origin.w + diffuse.xz + PCSS_Samples[i])) * 2.0f - 1.0f);
							//direction = (float4(rand.x * lightsData[lightInput.id].data3.x, rand.y * lightsData[lightInput.id].data3.x, 0.0f, 0.0f) * 100.0f - origin) / 64.0f;
						}
						
						areaShadows += shadow;
					}

					areaShadows /= (float)PCSS_SampleCount;
					areaShadows = min(areaShadows, 1.0f);
					shadowMask *= (1.0f - areaShadows);
				}
				else
				{
					shadowMask *= ShadowMap(shadowMap, shadowSampler, positionProj.xyz);
				}

				//shadowMask *= shadow;
			}

			shadowMask = clamp(shadowMask, 0.0f, 1.0f);

			float3 tmp = (specularLight + diffuse.xyz * diffuseLight) * spotLight.color.xyz;

			return float4(tmp.xyz * shadowMask, 1.0f);
		}
		else
		{
			return float4(0.0f, 0.0f, 0.0f, 1.0f);
		}
	}
}