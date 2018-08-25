#include "Lighting.hlsli"

cbuffer inputBuffer : register(b0)
{
	float4x4 mvpX;
	float4x4 mvpY;
	float4x4 mvpZ;
	float4 offset;
	float4 scale;
	uint4 dimension;
	float4 pad;
};

Texture2D diffuse : register(t0);
SamplerState diffuseSampler : register(s0);
RWTexture3D<float4> voxel : register(u1);
RWTexture3D<float4> voxelInput : register(u2);
Texture2D<float2> shadowMap : register(t1);

StructuredBuffer<Light> lightsData : register(t2);
StructuredBuffer<ShadowVirtualTextureRecord> shadowAtlasData : register(t3);

cbuffer rootConst : register(b1)
{
	uint rootID;
	uint lightsCount;
}

cbuffer modelBuffer : register(b2)
{
	float4x4 worldMatrix[32];
}

struct Vert2Geom
{
	float4 mPosition : SV_POSITION;
	float2 mTexCoord : TEXCOORD0;
	float3 mNormal : TEXCOORD1;
	float4 mPositionWS : TEXCOORD2;
};

struct Geom2Frag
{
	float4 mPosition : SV_POSITION;
	nointerpolation float4 mAABB : AABB;
	float3 mNormal : TEXCOORD1;
	float2 mTexCoord : TEXCOORD0;
	nointerpolation uint mAxis : AXIS;
	float4 mPositionWS : TEXCOORD2;
};

Vert2Geom VS(float3 mPosition : POSITION,
	float3 mNormal : NORMAL,
	float2 mTexCoord : TEXCOORD0)
{
	Vert2Geom result;

	float4 positionWS = mul(float4(mPosition, 1.0f), worldMatrix[rootID * 2]);

	result.mPosition = float4((positionWS.xyz + offset.xyz) * scale.xyz * 2.0f - 1.0f, 1.0f);
	result.mNormal = mul(float4(mNormal.xyz, 1.0f), worldMatrix[rootID * 2 + 1]).xyz;
	result.mTexCoord = mTexCoord;
	result.mPositionWS = positionWS;

	return result;
}

[maxvertexcount(3)]
void GS(triangle Vert2Geom input[3], inout TriangleStream<Geom2Frag> output)
{
	float3 faceNormal = normalize(cross(input[1].mPosition.xyz - input[0].mPosition.xyz, input[2].mPosition.xyz - input[0].mPosition.xyz));
	float xAxis = abs(faceNormal.x);
	float yAxis = abs(faceNormal.y);
	float zAxis = abs(faceNormal.z);

	float4x4 projectionMatrix;
	uint axis;
	if (xAxis > yAxis && xAxis > zAxis)
	{
		projectionMatrix = mvpX;
		axis = 0;
	}
	else if (yAxis > xAxis && yAxis > zAxis)
	{
		projectionMatrix = mvpY;
		axis = 1;
	}
	else
	{
		projectionMatrix = mvpZ;
		axis = 2;
	}

	//Geom2Frag v[3];
	Geom2Frag v0;
	Geom2Frag v1;
	Geom2Frag v2;

	v0.mPosition = mul(projectionMatrix, input[0].mPosition);
	v0.mTexCoord = input[0].mTexCoord;
	v0.mNormal = input[0].mNormal.xyz;
	v0.mAxis = axis;
	v0.mPositionWS = input[0].mPositionWS;

	v1.mPosition = mul(projectionMatrix, input[1].mPosition);
	v1.mTexCoord = input[1].mTexCoord;
	v1.mNormal = input[1].mNormal.xyz;
	v1.mAxis = axis;
	v1.mPositionWS = input[1].mPositionWS;

	v2.mPosition = mul(projectionMatrix, input[2].mPosition);
	v2.mTexCoord = input[2].mTexCoord;
	v2.mNormal = input[2].mNormal.xyz;
	v2.mAxis = axis;
	v2.mPositionWS = input[2].mPositionWS;

	float2 halfPixel = float2(1.0f / (float)dimension.x, 1.0f / (float)dimension.y) * 0.5f;
	float dilationSize = 1.4142135637309f / (float)dimension.x;

	float4 AABB = float4(0.0f, 0.0f, 0.0f, 0.0f);
	AABB.xy = v0.mPosition.xy;
	AABB.zw = v0.mPosition.xy;

	AABB.xy = min(v1.mPosition.xy, AABB.xy);
	AABB.zw = max(v1.mPosition.xy, AABB.zw);

	AABB.xy = min(v2.mPosition.xy, AABB.xy);
	AABB.zw = max(v2.mPosition.xy, AABB.zw);

	AABB.xy -= halfPixel;
	AABB.zw += halfPixel;

	v0.mAABB = AABB;
	v1.mAABB = AABB;
	v2.mAABB = AABB;

	float3 edge[3];
	edge[0] = normalize(float3(v1.mPosition.xy - v0.mPosition.xy, 0.0f));
	edge[1] = normalize(float3(v2.mPosition.xy - v1.mPosition.xy, 0.0f));
	edge[2] = normalize(float3(v0.mPosition.xy - v2.mPosition.xy, 0.0f));

	v0.mPosition.xy = v0.mPosition.xy + dilationSize * normalize(-edge[0].xy + edge[2].xy);
	v1.mPosition.xy = v1.mPosition.xy + dilationSize * normalize(-edge[1].xy + edge[0].xy);
	v2.mPosition.xy = v2.mPosition.xy + dilationSize * normalize(-edge[2].xy + edge[1].xy);

	output.Append(v0);
	output.Append(v1);
	output.Append(v2);

	output.RestartStrip();
}

float4 PS(Geom2Frag input, float4 fragCoord : SV_Position) : SV_TARGET
{
	uint4 temp = uint4((uint)fragCoord.x, (uint)fragCoord.y, (uint)((float)dimension.z * fragCoord.z), 0);
	uint4 texCoord;
	if (input.mAxis == 1)
	{
		texCoord.x = dimension.x - temp.x - 1;
		texCoord.y = temp.z;
		texCoord.z = dimension.y - temp.y - 1;
	}
	else if (input.mAxis == 2)
	{
		texCoord.x = temp.x;
		texCoord.y = dimension.y - temp.y - 1;
		texCoord.z = temp.z;
	}
	else
	{
		texCoord.x = temp.z;
		texCoord.y = dimension.y - temp.y - 1;
		texCoord.z = dimension.x - temp.x - 1;
	}

	float4 tex = diffuse.Sample(diffuseSampler, input.mTexCoord.xy);
	tex = float4(pow(max(tex.x, 0.0f), 2.2f), pow(max(tex.y, 0.0f), 2.2f), pow(max(tex.z, 0.0f), 2.2f), tex.w);
	float4 result = float4(0.0f, 0.0f, 0.0f, 1.0f);
	float3 position = input.mPositionWS.xyz;
	float3 normals = normalize(input.mNormal);
	
	[loop]
	for (uint i = 0; i < lightsCount; i++)
	{
		if (lightsData[NonUniformResourceIndex(i)].type == 0)
		{
			float3 lightVec = lightsData[NonUniformResourceIndex(i)].position.xyz - position;
			float lightDist = length(lightVec);
			float3 lightDir = normalize(lightVec);
			float attenuation = 1.0f / (1.0f + lightDist * lightsData[NonUniformResourceIndex(i)].data0.x + lightDist * lightDist * lightsData[NonUniformResourceIndex(i)].data0.y);
			float diffuseLight = max(dot(lightDir, normals), 0.0f) * attenuation;
			float shadowMask = 1.0f;

			if (asuint(lightsData[NonUniformResourceIndex(i)].data0.z) >= 0)
			{
				float3 shadowDir = normalize(lightsData[NonUniformResourceIndex(i)].position.xyz - position);
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
					shadowProj = mul(shadowAtlasData[asuint(lightsData[NonUniformResourceIndex(i)].data0.z)].shadowMatrix, input.mPositionWS);
					shadowProj.y = -shadowProj.y;
					shadowProj.xy /= shadowProj.w;
					shadowProj.xy *= 0.5;
					shadowProj.xy += 0.5;
					shadowProj.z /= (lightsData[NonUniformResourceIndex(i)].shadowFar - lightsData[NonUniformResourceIndex(i)].shadowNear);
					shadowProj.z -= 0.005;
					shadowProj.xy *= shadowAtlasData[asuint(lightsData[NonUniformResourceIndex(i)].data0.z)].size;
					shadowProj.xy += shadowAtlasData[asuint(lightsData[NonUniformResourceIndex(i)].data0.z)].offset;
				}
				else if (shadowMax == 1)
				{
					shadowProj = mul(shadowAtlasData[asuint(lightsData[NonUniformResourceIndex(i)].data0.w)].shadowMatrix, input.mPositionWS);
					shadowProj.y = -shadowProj.y;
					shadowProj.xy /= shadowProj.w;
					shadowProj.xy *= 0.5;
					shadowProj.xy += 0.5;
					shadowProj.z /= (lightsData[NonUniformResourceIndex(i)].shadowFar - lightsData[NonUniformResourceIndex(i)].shadowNear);
					shadowProj.z -= 0.005;
					shadowProj.xy *= shadowAtlasData[asuint(lightsData[NonUniformResourceIndex(i)].data0.w)].size;
					shadowProj.xy += shadowAtlasData[asuint(lightsData[NonUniformResourceIndex(i)].data0.w)].offset;
				}
				else if (shadowMax == 2)
				{
					shadowProj = mul(shadowAtlasData[asuint(lightsData[NonUniformResourceIndex(i)].data1.y)].shadowMatrix, input.mPositionWS);
					shadowProj.y = -shadowProj.y;
					shadowProj.xy /= shadowProj.w;
					shadowProj.xy *= 0.5;
					shadowProj.xy += 0.5;
					shadowProj.z /= (lightsData[NonUniformResourceIndex(i)].shadowFar - lightsData[NonUniformResourceIndex(i)].shadowNear);
					shadowProj.z -= 0.005;
					shadowProj.xy *= shadowAtlasData[asuint(lightsData[NonUniformResourceIndex(i)].data1.y)].size;
					shadowProj.xy += shadowAtlasData[asuint(lightsData[NonUniformResourceIndex(i)].data1.y)].offset;
				}
				else if (shadowMax == 3)
				{
					shadowProj = mul(shadowAtlasData[asuint(lightsData[NonUniformResourceIndex(i)].data1.x)].shadowMatrix, input.mPositionWS);
					shadowProj.y = -shadowProj.y;
					shadowProj.xy /= shadowProj.w;
					shadowProj.xy *= 0.5;
					shadowProj.xy += 0.5;
					shadowProj.z /= (lightsData[NonUniformResourceIndex(i)].shadowFar - lightsData[NonUniformResourceIndex(i)].shadowNear);
					shadowProj.z -= 0.005;
					shadowProj.xy *= shadowAtlasData[asuint(lightsData[NonUniformResourceIndex(i)].data1.x)].size;
					shadowProj.xy += shadowAtlasData[asuint(lightsData[NonUniformResourceIndex(i)].data1.x)].offset;
				}
				else if (shadowMax == 4)
				{
					shadowProj = mul(shadowAtlasData[asuint(lightsData[NonUniformResourceIndex(i)].data1.w)].shadowMatrix, input.mPositionWS);
					shadowProj.y = -shadowProj.y;
					shadowProj.xy /= shadowProj.w;
					shadowProj.xy *= 0.5;
					shadowProj.xy += 0.5;
					shadowProj.z /= (lightsData[NonUniformResourceIndex(i)].shadowFar - lightsData[NonUniformResourceIndex(i)].shadowNear);
					shadowProj.z -= 0.005;
					shadowProj.xy *= shadowAtlasData[asuint(lightsData[NonUniformResourceIndex(i)].data1.w)].size;
					shadowProj.xy += shadowAtlasData[asuint(lightsData[NonUniformResourceIndex(i)].data1.w)].offset;
				}
				else
				{
					shadowProj = mul(shadowAtlasData[asuint(lightsData[NonUniformResourceIndex(i)].data1.z)].shadowMatrix, input.mPositionWS);
					shadowProj.y = -shadowProj.y;
					shadowProj.xy /= shadowProj.w;
					shadowProj.xy *= 0.5;
					shadowProj.xy += 0.5;
					shadowProj.z /= (lightsData[NonUniformResourceIndex(i)].shadowFar - lightsData[NonUniformResourceIndex(i)].shadowNear);
					shadowProj.z -= 0.005;
					shadowProj.xy *= shadowAtlasData[asuint(lightsData[NonUniformResourceIndex(i)].data1.z)].size;
					shadowProj.xy += shadowAtlasData[asuint(lightsData[NonUniformResourceIndex(i)].data1.z)].offset;
				}

				float shadow = ShadowMap(shadowMap, diffuseSampler, shadowProj.xyz);
				shadowMask *= shadow;
			}

			shadowMask = clamp(shadowMask, 0.0f, 1.0f);

			float3 tmp = tex.xyz * diffuseLight * shadowMask * lightsData[NonUniformResourceIndex(i)].color.xyz;
			result += float4(tmp.xyz, 0.0f);
		}
		else if (lightsData[NonUniformResourceIndex(i)].type == 1)
		{
			SpotLight spotLight = (SpotLight)lightsData[NonUniformResourceIndex(i)];

			float3 lightVec = lightsData[NonUniformResourceIndex(i)].position.xyz - position;
			float lightDist = length(lightVec);
			float3 lightDir = normalize(lightVec);
			float attenuation = 1.0f / (1.0f + lightDist * spotLight.spotLinearAtt + lightDist * lightDist * spotLight.spotQuadraticAtt);
			float spot = dot(-lightDir, spotLight.spotDirection.xyz) > spotLight.spotFalloff ? 1.0f : 0.0f;
			float diffuseLight = max(dot(lightDir, normals), 0.0f) * attenuation * spot;
			float shadowMask = 1.0f;

			if (spotLight.shadowID >= 0)
			{
				float4 positionProj = mul(shadowAtlasData[spotLight.shadowID].shadowMatrix, input.mPositionWS);
				positionProj.xy /= positionProj.w;
				positionProj.y = -positionProj.y;
				positionProj.x *= 0.5f;
				positionProj.x += 0.5f;
				positionProj.y *= 0.5f;
				positionProj.y += 0.5f;
				positionProj.z /= (spotLight.shadowFar - spotLight.shadowNear);
				positionProj.z -= 0.005;

				positionProj.xy *= shadowAtlasData[spotLight.shadowID].size;
				positionProj.xy += shadowAtlasData[spotLight.shadowID].offset;

				float shadow = ShadowMap(shadowMap, diffuseSampler, positionProj.xyz);

				shadowMask *= shadow;
			}
			shadowMask = clamp(shadowMask, 0.0f, 1.0f);

			float3 tmp = tex.xyz * diffuseLight * shadowMask * lightsData[NonUniformResourceIndex(i)].color.xyz;
			result += float4(tmp.xyz, 0.0f);
		}
	}

	voxel[texCoord.xyz] = voxelInput[texCoord.xyz] * 0.9 + float4(result.xyz, 1.0f) * 0.1;

	return float4(tex.xyz, 1.0f);
}