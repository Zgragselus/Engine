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

Texture2D<float4> brdfLookup : register(t9);

SamplerState shadowSampler : register(s0);
SamplerState brdfSampler : register(s1);

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
		float3 viewDir = normalize(-position.xyz);

		float3 F0 = float3(0.04f, 0.04f, 0.04f);
		F0 = lerp(F0, diffuse.xyz, data.z);

		float3 kS = fresnelSchlickRoughness(max(dot(normals, viewDir), 0.0f), F0, data.w);
		float3 kD = 1.0f - kS;
		kD *= 1.0f - data.z;

#if SamplesMSAAGI == SamplesMSAA
		float3 irradiance = sourceGI.Load(coord, sampleID).xyz;
#else
		float3 irradiance = sourceGI.Load(coord, 0).xyz;
#endif

		float2 envBRDF = brdfLookup.SampleLevel(brdfSampler, float2(max(dot(normals, viewDir), 0.0f), data.w), 0.0f).xy;

		float3 gi = irradiance * diffuse.xyz + env * (kS * envBRDF.x + envBRDF.y);
		//float3 giAmbient = (kD * diffuse.xyz) * ao;

#if SamplesMSAAGI == SamplesMSAA
		//float3 gi = env * data.z;
		//float3 gi = diffuse.xyz * sourceGI.Load(coord, sampleID).xyz + env * data.z;
#else
		//float3 gi = diffuse.xyz * sourceGI.Load(coord, 0).xyz + env * data.z;
#endif

		return float4(gi * ao, 1.0f);
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

			//float vdoth = saturate(dot(viewDir, halfDir));
			//float ndoth = saturate(dot(normals, halfDir));
			//float ndotv = saturate(dot(normals, viewDir));
			//float ndotl = saturate(dot(normals, lightDir));

			//float attenuation = 1.0f / (1.0f + lightDist * lightsData[lightInput.id].data0.x * 4.0f * 3.141592f + lightDist * lightDist * lightsData[lightInput.id].data0.y * 4.0f * 3.141592f);

			//diffuseLight = DiffuseLambert(lightDir, normals) * attenuation;
			//diffuseLight = DiffuseOrenNayar(lightDir, viewDir, normals, roughnessL) * attenuation;
			//diffuseLight = max(dot(lightDir, normals), 0.0f) * attenuation;

			/*float3 lightF = FresnelTerm(vdoth);
			float lightD = DistributionTerm(roughnessL, ndoth);
			float lightV = VisibilityTerm(roughnessL, ndotv, ndotl);
			specularLight = lightF * (lightD * lightV * 3.141592f * ndotl) * attenuation;
			specularLight *= clamp(pow(abs(ndotv + ao), roughnessE) - 1.0f, 0.0f, 1.0f);*/

			//specularLight = SpecularGGX(lightDir, viewDir, halfDir, normals, diffuse.xyz, data.w, ao, roughnessE, roughnessL) * attenuation;

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

			float3 tmp = BRDF(normals, viewDir, lightDir, halfDir, diffuse.xyz, data.z, data.w) * lightsData[lightInput.id].color.xyz;
			//float3 tmp = (specularLight + diffuse.xyz * diffuseLight) * lightsData[lightInput.id].color.xyz;
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

			//diffuseLight = DiffuseLambert(lightDir, normals) * attenuation * spot;
			//diffuseLight = DiffuseOrenNayar(lightDir, viewDir, normals, roughnessL) * attenuation * spot;
			//diffuseLight = max(dot(lightDir, normals), 0.0f) * attenuation * spot;

			/*float3 lightF = FresnelTerm(vdoth);
			float lightD = DistributionTerm(roughnessL, ndoth);
			float lightV = VisibilityTerm(roughnessL, ndotv, ndotl);
			specularLight = lightF * (lightD * lightV * 3.141592f * ndotl) * attenuation;
			specularLight *= clamp(pow(abs(ndotv + ao), roughnessE) - 1.0f, 0.0f, 1.0f) * spot;*/

			//specularLight = SpecularGGX(lightDir, viewDir, halfDir, normals, diffuse.xyz, data.w, ao, roughnessE, roughnessL) * attenuation * spot;

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
					float3 ndc = positionProj.xyz;

					float visibility = 1.0f;
					float oneTexelSize = 1.0f / lightsData[lightInput.id].shadowScale;
					float halfTexelSize = oneTexelSize * 0.5f;
					float2 ndcCurrentFragmentLightmapTexel = ndc.xy;
					float lightSizeScale = 1.0f / lightsData[lightInput.id].data3.x;
					float near_plane = spotLight.shadowNear;
					float far_plane = spotLight.shadowFar;
					float lightmapFarPlaneSize = tan(spotLight.spotAngle * 0.5f);

					float z = ndc.z * (far_plane - near_plane);
					float zlinearized = ((2.0f * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane))) / far_plane;

					for (int i = -10; i < 10; i++)
					{
						for (int j = -10; j < 10; j++)
						{
							float sampleTexX = ndc.x + oneTexelSize * 2.5f * float(i);
							float sampleTexY = ndc.y + oneTexelSize * 2.5f * float(j);
							float2 sampleCoords = float2(sampleTexX, sampleTexY);

							float zs = shadowMap.SampleLevel(shadowSampler, sampleCoords.xy, 0.0f).x * (far_plane - near_plane);
							float zslinearized = ((2.0f * near_plane * far_plane) / (far_plane + near_plane - zs * (far_plane - near_plane))) / far_plane;

							if (zslinearized > zlinearized)
							{
								continue;
							}

							float thisSampleZPlaneSize = lightmapFarPlaneSize * zslinearized;
							float2 ndcNextFragmentLightmapTexel = sampleCoords;
							float scaleFactor = zlinearized / (zlinearized - zslinearized);

							float left = (ndcNextFragmentLightmapTexel.x - ndcCurrentFragmentLightmapTexel.x - halfTexelSize) * thisSampleZPlaneSize * scaleFactor * lightSizeScale;
							float right = (ndcNextFragmentLightmapTexel.x - ndcCurrentFragmentLightmapTexel.x + halfTexelSize) * thisSampleZPlaneSize * scaleFactor * lightSizeScale;
							float top = (ndcNextFragmentLightmapTexel.y - ndcCurrentFragmentLightmapTexel.y + halfTexelSize) * thisSampleZPlaneSize * scaleFactor * lightSizeScale;
							float bottom = (ndcNextFragmentLightmapTexel.y - ndcCurrentFragmentLightmapTexel.y - halfTexelSize) * thisSampleZPlaneSize * scaleFactor * lightSizeScale;

							left = clamp(left, -0.5f, 0.5f);
							right = clamp(right, -0.5f, 0.5f);
							top = clamp(top, -0.5f, 0.5f);
							bottom = clamp(bottom, -0.5f, 0.5f);

							float2 sampleCoords1 = float2(sampleTexX - (oneTexelSize * 2.5f), sampleTexY);
							float zs1 = shadowMap.SampleLevel(shadowSampler, sampleCoords1.xy, 0.0f).x * (far_plane - near_plane);
							float zslinearized1 = ((2.0f * near_plane * far_plane) / (far_plane + near_plane - zs1 * (far_plane - near_plane))) / far_plane;

							if (zslinearized1 < zlinearized)
							{
								float thisSampleZPlaneTexelSize = lightmapFarPlaneSize * zslinearized1;
								ndcNextFragmentLightmapTexel = sampleCoords1;
								scaleFactor = zlinearized / (zlinearized - zslinearized1);
								float left1 = (ndcNextFragmentLightmapTexel.x - ndcCurrentFragmentLightmapTexel.x - halfTexelSize) * thisSampleZPlaneTexelSize * scaleFactor * lightSizeScale;
								left1 = clamp(left1, -0.5f, 0.5f);
								left = min(left1, left);
							}

							float2 sampleCoords2 = float2(sampleTexX, sampleTexY - (oneTexelSize * 2.5f));
							float zs2 = shadowMap.SampleLevel(shadowSampler, sampleCoords2.xy, 0.0f).x * (far_plane - near_plane);
							float zslinearized2 = ((2.0 * near_plane * far_plane) / (far_plane + near_plane - zs2 * (far_plane - near_plane))) / far_plane;

							if (zslinearized2 < zlinearized)
							{
								float thisSampleZPlaneTexelSize = lightmapFarPlaneSize * zslinearized1;
								ndcNextFragmentLightmapTexel = sampleCoords2;
								scaleFactor = zlinearized / (zlinearized - zslinearized2);
								float bottom1 = (ndcNextFragmentLightmapTexel.y - ndcCurrentFragmentLightmapTexel.y - halfTexelSize) * thisSampleZPlaneTexelSize * scaleFactor * lightSizeScale;
								bottom1 = clamp(bottom1, -0.5f, 0.5f);
								bottom = min(bottom1, bottom);
							}

							float width = right - left;
							float height = top - bottom;

							float area = width * height;
							visibility -= area * 1.0f;
						}
					}

					if (visibility < 0.0f) visibility = 0.0f;

					return float4(visibility, visibility, visibility, 1.0f);

					/*float4 origin = mul(shadowAtlasData[spotLight.shadowID].shadowMatrix, positionWS);
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
					shadowMask *= (1.0f - areaShadows);*/
				}
				else
				{
					shadowMask *= ShadowMap(shadowMap, shadowSampler, positionProj.xyz);
				}

				//shadowMask *= shadow;
			}

			shadowMask = clamp(shadowMask, 0.0f, 1.0f);

			float3 tmp = BRDF(normals, viewDir, lightDir, halfDir, diffuse.xyz, data.z, data.w) * spot * lightsData[lightInput.id].color.xyz;
			//float3 tmp = (specularLight + diffuse.xyz * diffuseLight) * spotLight.color.xyz;

			return float4(tmp.xyz * shadowMask, 1.0f);
		}
		else
		{
			return float4(0.0f, 0.0f, 0.0f, 1.0f);
		}
	}
}