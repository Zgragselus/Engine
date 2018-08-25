struct ShadowVirtualTextureRecord
{
	float4x4 shadowMatrix;
	float2 offset;
	float2 size;
};

struct Light
{
	float4 position;
	float4 positionVS;
	float4 color;
	float4 data0;
	float4 data1;
	float4 data2;
	unsigned int type;
	int shadowID;
	float shadowNear;
	float shadowFar;
	float shadowResolution;
	float shadowScale;
	int shadowFilter;
	float offset;
	float4 data3;
};

struct SpotLight
{
	float4 position;
	float4 positionVS;
	float4 color;
	float4 spotDirection;
	float4 spotDirectionVS;
	float spotAngle;
	float spotFalloff;
	float spotLinearAtt;
	float spotQuadraticAtt;
	unsigned int type;
	int shadowID;
	float shadowNear;
	float shadowFar;
	float shadowResolution;
	float shadowScale;
	float pad[2];
	float4 data3;
};

struct PointLight
{
	float4 position;
	float4 positionVS;
	float4 color;
	float pointLinearAtt;
	float pointQuadraticAtt;
	unsigned int cubeShadowID[6];
	unsigned int unionPad[4];
	unsigned int type;
	int shadowID;
	float shadowNear;
	float shadowFar;
	float shadowResolution;
	float shadowScale;
	float pad[2];
	float4 data3;
};

struct DirectionalLight
{
	float4 position;
	float4 positionVS;
	float4 color;
	float4 dirDirection;
	float4 dirDirectionVS;
	unsigned int unionPad[4];
	unsigned int type;
	unsigned int pad[3];
	float4 data3;
};

inline float ShadowMap(Texture2D<float2> tex, SamplerState state, float3 projCoord)
{
	return tex.SampleLevel(state, projCoord.xy, 0.0f).x < projCoord.z ? 0.0f : 1.0f;
}

inline float ShadowMapBilinear(Texture2D<float2> tex, SamplerState state, float3 projCoord, float resolution, float pixelSize)
{
	float2 grad = frac(projCoord.xy * resolution + 0.5f);
	float4 tmp = tex.Gather(state, projCoord.xy);
	tmp.x = tmp.x < projCoord.z ? 0.0f : 1.0f;
	tmp.y = tmp.y < projCoord.z ? 0.0f : 1.0f;
	tmp.z = tmp.z < projCoord.z ? 0.0f : 1.0f;
	tmp.w = tmp.w < projCoord.z ? 0.0f : 1.0f;
	return lerp(lerp(tmp.w, tmp.z, grad.x), lerp(tmp.x, tmp.y, grad.x), grad.y);
}

inline float CubicHermite(float A, float B, float C, float D, float t)
{
	float t2 = t * t;
	float t3 = t * t*t;
	float a = -A / 2.0 + (3.0*B) / 2.0 - (3.0*C) / 2.0 + D / 2.0;
	float b = A - (5.0*B) / 2.0 + 2.0*C - D / 2.0;
	float c = -A / 2.0 + C / 2.0;
	float d = B;

	return a * t3 + b * t2 + c * t + d;
}

inline float ShadowMapBicubic(Texture2D<float2> tex, SamplerState state, float3 projCoord, float resolution, float pixelSize)
{
	float c_onePixel = pixelSize;
	float c_twoPixels = 2.0 * pixelSize;

	float2 grad = frac(projCoord.xy * resolution + 0.5f);

	float2 pixel = projCoord.xy * resolution + 0.5f;

	float2 fracTmp = frac(pixel);
	pixel = floor(pixel) / resolution - float2(c_onePixel / 2.0, c_onePixel / 2.0);

	float C00 = tex.SampleLevel(state, pixel + float2(-c_onePixel, -c_onePixel), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C10 = tex.SampleLevel(state, pixel + float2(0.0, -c_onePixel), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C20 = tex.SampleLevel(state, pixel + float2(c_onePixel, -c_onePixel), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C30 = tex.SampleLevel(state, pixel + float2(c_twoPixels, -c_onePixel), 0.0).x  < projCoord.z ? 0.0f : 1.0f;

	float C01 = tex.SampleLevel(state, pixel + float2(-c_onePixel, 0.0), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C11 = tex.SampleLevel(state, pixel + float2(0.0, 0.0), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C21 = tex.SampleLevel(state, pixel + float2(c_onePixel, 0.0), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C31 = tex.SampleLevel(state, pixel + float2(c_twoPixels, 0.0), 0.0).x  < projCoord.z ? 0.0f : 1.0f;

	float C02 = tex.SampleLevel(state, pixel + float2(-c_onePixel, c_onePixel), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C12 = tex.SampleLevel(state, pixel + float2(0.0, c_onePixel), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C22 = tex.SampleLevel(state, pixel + float2(c_onePixel, c_onePixel), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C32 = tex.SampleLevel(state, pixel + float2(c_twoPixels, c_onePixel), 0.0).x  < projCoord.z ? 0.0f : 1.0f;

	float C03 = tex.SampleLevel(state, pixel + float2(-c_onePixel, c_twoPixels), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C13 = tex.SampleLevel(state, pixel + float2(0.0, c_twoPixels), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C23 = tex.SampleLevel(state, pixel + float2(c_onePixel, c_twoPixels), 0.0).x  < projCoord.z ? 0.0f : 1.0f;
	float C33 = tex.SampleLevel(state, pixel + float2(c_twoPixels, c_twoPixels), 0.0).x  < projCoord.z ? 0.0f : 1.0f;

	float CP0X = CubicHermite(C00, C10, C20, C30, fracTmp.x);
	float CP1X = CubicHermite(C01, C11, C21, C31, fracTmp.x);
	float CP2X = CubicHermite(C02, C12, C22, C32, fracTmp.x);
	float CP3X = CubicHermite(C03, C13, C23, C33, fracTmp.x);

	return CubicHermite(CP0X, CP1X, CP2X, CP3X, fracTmp.y);
}

inline float ShadowMapPCF(Texture2D<float2> tex, SamplerState state, float3 projCoord, float resolution, float pixelSize, int filterSize)
{
	float shadow = 0.0f;
	float2 grad = frac(projCoord.xy * resolution + 0.5f);

	for (int i = -filterSize; i <= filterSize; i++)
	{
		for (int j = -filterSize; j <= filterSize; j++)
		{
			float4 tmp = tex.Gather(state, projCoord.xy + float2(i, j) * float2(pixelSize, pixelSize));
			tmp.x = tmp.x < projCoord.z ? 0.0f : 1.0f;
			tmp.y = tmp.y < projCoord.z ? 0.0f : 1.0f;
			tmp.z = tmp.z < projCoord.z ? 0.0f : 1.0f;
			tmp.w = tmp.w < projCoord.z ? 0.0f : 1.0f;
			shadow += lerp(lerp(tmp.w, tmp.z, grad.x), lerp(tmp.x, tmp.y, grad.x), grad.y);
		}
	}

	return shadow / (float)((2 * filterSize + 1) * (2 * filterSize + 1));
}

/*static const uint PCSS_SampleCount = 16;
static const float PCSS_SampleRadius = 1.0f;
static float2 PCSS_Samples[PCSS_SampleCount] =
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
};*/
static const uint PCSS_SampleCount = 32;
static const float PCSS_SampleRadius = 1.0f;
static const float2 PCSS_Samples[PCSS_SampleCount] = {
	float2(0.06407013, 0.05409927),
	float2(0.7366577, 0.5789394),
	float2(-0.6270542, -0.5320278),
	float2(-0.4096107, 0.8411095),
	float2(0.6849564, -0.4990818),
	float2(-0.874181, -0.04579735),
	float2(0.9989998, 0.0009880066),
	float2(-0.004920578, -0.9151649),
	float2(0.1805763, 0.9747483),
	float2(-0.2138451, 0.2635818),
	float2(0.109845, 0.3884785),
	float2(0.06876755, -0.3581074),
	float2(0.374073, -0.7661266),
	float2(0.3079132, -0.1216763),
	float2(-0.3794335, -0.8271583),
	float2(-0.203878, -0.07715034),
	float2(0.5912697, 0.1469799),
	float2(-0.88069, 0.3031784),
	float2(0.5040108, 0.8283722),
	float2(-0.5844124, 0.5494877),
	float2(0.6017799, -0.1726654),
	float2(-0.5554981, 0.1559997),
	float2(-0.3016369, -0.3900928),
	float2(-0.5550632, -0.1723762),
	float2(0.925029, 0.2995041),
	float2(-0.2473137, 0.5538505),
	float2(0.9183037, -0.2862392),
	float2(0.2469421, 0.6718712),
	float2(0.3916397, -0.4328209),
	float2(-0.03576927, -0.6220032),
	float2(-0.04661255, 0.7995201),
	float2(0.4402924, 0.3640312),
};

inline float Random(float2 co)
{
	return frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453);
}

/*inline float PCSS_BlockerDistance(Texture2D<float2> tex, SamplerState state, float3 projCoord, float pixelSize, float lightSize)
{
	int size = 2;
	int blockers = 0;
	float avgBlockerDistance = 0.0f;
	for (int i = 0; i < (int)PCSS_SampleCount; i++)
	{
		float2 rand = Random(projCoord.xy + projCoord.xy * PCSS_Samples[i] + PCSS_Samples[i]) * 2.0 - 1.0;
		float z = tex.SampleLevel(state, projCoord.xy + rand * lightSize * float2(pixelSize, pixelSize) + PCSS_Samples[i] * lightSize * float2(pixelSize, pixelSize), 0.0f).x;
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

inline float ShadowMapPCSS(Texture2D<float2> tex, SamplerState state, float3 projCoord, float resolution, float pixelSize, float lightSize)
{
	int size = 2;

	float blockerDistance = PCSS_BlockerDistance(tex, state, projCoord, pixelSize, lightSize);
	float penumbraSize = max((projCoord.z - blockerDistance) * lightSize / blockerDistance, 0.0f);

	float shadow = 0.0f;

	for (int i = 0; i < (int)PCSS_SampleCount; i++)
	{
		float2 rand = Random(projCoord.xy + projCoord.xy * PCSS_Samples[i] + PCSS_Samples[i]);
		shadow += (tex.SampleLevel(state, projCoord.xy +
			(PCSS_Samples[i] + clamp((rand * 2.0f - 1.0f) * penumbraSize, -1.0f, 1.0f)) * float2(pixelSize, pixelSize) * 1.0f * penumbraSize +
			clamp(rand * 2.0f - 1.0f, 0.0f, 1.0f) * float2(pixelSize, pixelSize), 0.0f).x < projCoord.z) ? 0.0f : 1.0f;
	}

	return shadow / (float)PCSS_SampleCount;
}*/

inline float PCSS_Noise(float3 pos)
{
	float3 noiseSkew = pos + 0.2127f + pos.x * pos.y * pos.z * 0.3713f;
	float3 noiseRnd = 4.789f * sin(489.123f * (noiseSkew));
	return frac(noiseRnd.x * noiseRnd.y * noiseRnd.z * (1.0 + noiseSkew.x));
}

inline float2 PCSS_Rotate(float2 pos, float2 rotationTrig)
{
	return float2(pos.x * rotationTrig.x - pos.y * rotationTrig.y, pos.y * rotationTrig.x + pos.x * rotationTrig.y);
}

inline float2 PCSS_BlockerDistance(Texture2D<float2> tex, SamplerState state, float3 projCoord, float searchUV, float2 rotationTrig)
{
	int blockers = 0;
	float avgBlockerDistance = 0.0f;
	for (int i = 0; i < (int)PCSS_SampleCount; i++)
	{
		float2 offset = PCSS_Samples[i] * searchUV;
		offset = PCSS_Rotate(offset, rotationTrig);

		float z = tex.SampleLevel(state, projCoord.xy + offset, 0.0f).x;
		if (z < projCoord.z)
		{
			blockers++;
			avgBlockerDistance += z;
		}
	}

	avgBlockerDistance /= blockers;

	return float2(avgBlockerDistance, (float)blockers);
}

inline float PCSS_PCFFilter(Texture2D<float2> tex, SamplerState state, float3 projCoord, float filterRadiusUV, float penumbra, float2 rotationTrig, float2 grad)
{
	float sum = 0.0f;
	for (int i = 0; i < (int)PCSS_SampleCount; i++)
	{
		float2 offset = PCSS_Samples[i] * filterRadiusUV;
		offset = PCSS_Rotate(offset, rotationTrig);

		/*float4 tmp = tex.Gather(state, projCoord.xy + offset);
		tmp.x = tmp.x < projCoord.z ? 0.0f : 1.0f;
		tmp.y = tmp.y < projCoord.z ? 0.0f : 1.0f;
		tmp.z = tmp.z < projCoord.z ? 0.0f : 1.0f;
		tmp.w = tmp.w < projCoord.z ? 0.0f : 1.0f;
		sum += lerp(lerp(tmp.w, tmp.z, grad.x), lerp(tmp.x, tmp.y, grad.x), grad.y);*/
		sum += tex.SampleLevel(state, projCoord.xy + offset, 0.0f).x < projCoord.z ? 0.0f : 1.0f;
	}
	sum /= (float)PCSS_SampleCount;
	return sum;
}

inline float ShadowMapPCSS(Texture2D<float2> tex, SamplerState state, float3 projCoord, float resolution, float pixelSize, float lightSize)
{
	float2 uv = projCoord.xy;
	float depth = projCoord.z;
	float zAwareDepth = depth;

	float rotationAngle = Random(projCoord.xy) * 3.1415926;
	float2 rotationTrig = float2(cos(rotationAngle), sin(rotationAngle));

	float searchSize = lightSize / resolution * 100.0 * saturate(zAwareDepth - .02) / zAwareDepth;
	float2 blockerInfo = PCSS_BlockerDistance(tex, state, projCoord, searchSize, rotationTrig);

	if (blockerInfo.y < 1.0)
	{
		return 1.0f;
	}
	else
	{
		float penumbra = max(zAwareDepth - blockerInfo.x, 0.0);
		float filterRadiusUV = penumbra * lightSize / resolution * 100.0;

		float2 grad = frac(projCoord.xy * resolution + 0.5f);

		float shadow = PCSS_PCFFilter(tex, state, projCoord, filterRadiusUV, penumbra, rotationTrig, grad);

		return shadow;
	}
}

inline float PCMLSM_ShadowMapMIP(Texture2D<float2> tex, SamplerState state, float3 projCoord, int level, float resolution, float pixelSize, int filterSize)
{
	int levelResolution = (int)resolution >> level;
	float levelSize = pixelSize * pow(2.0, level);

	float shadow = 0.0f;
	float2 grad = frac(projCoord.xy * levelResolution);

	for (int i = -filterSize; i <= filterSize; i++)
	{
		for (int j = -filterSize; j <= filterSize; j++)
		{
			float4 tmp;
			tmp.x = tex.SampleLevel(state, projCoord.xy + float2(i, j) * float2(levelSize, levelSize) + float2(0.0f, 0.0f), level).x < projCoord.z ? 0.0f : 1.0f;
			tmp.y = tex.SampleLevel(state, projCoord.xy + float2(i, j) * float2(levelSize, levelSize) + float2(levelSize, 0.0f), level).x < projCoord.z ? 0.0f : 1.0f;
			tmp.z = tex.SampleLevel(state, projCoord.xy + float2(i, j) * float2(levelSize, levelSize) + float2(levelSize, levelSize), level).x < projCoord.z ? 0.0f : 1.0f;
			tmp.w = tex.SampleLevel(state, projCoord.xy + float2(i, j) * float2(levelSize, levelSize) + float2(0.0f, levelSize), level).x < projCoord.z ? 0.0f : 1.0f;
			shadow += lerp(lerp(tmp.x, tmp.y, grad.x), lerp(tmp.w, tmp.z, grad.x), grad.y);
		}
	}

	return shadow / (float)((2 * filterSize + 1) * (2 * filterSize + 1));
}

inline float PCMLSM_ShadowMapMIPLerp(Texture2D<float2> tex, SamplerState state, float3 projCoord, float level, float resolution, float pixelSize, int filterSize)
{
	int l = (int)level;
	float t0 = PCMLSM_ShadowMapMIP(tex, state, projCoord, l, resolution, pixelSize, filterSize);
	float t1 = PCMLSM_ShadowMapMIP(tex, state, projCoord, l + 1, resolution, pixelSize, filterSize);
	return lerp(t0, t1, level - (float)l);
}

float PCMLSM_Blocker(Texture2D<float2> tex, SamplerState state, float3 projCoord, float uvLightSize)
{
	int size = 2;
	int blockers = 0;
	float avgBlockerDistance = 0.0f;
	for (int i = 0; i < (int)PCSS_SampleCount; i++)
	{
		float z = tex.SampleLevel(state, projCoord.xy + PCSS_Samples[i] * uvLightSize * float2(1.0f / 512.0f, 1.0f / 512.0f), 0.0f).x;
		if (z < projCoord.z)
		{
			blockers++;
			avgBlockerDistance += z;
		}

		z = tex.SampleLevel(state, projCoord.xy + PCSS_Samples[i] * uvLightSize * float2(1.0f / 256.0f, 1.0f / 256.0f), 1.0f).x;
		if (z < projCoord.z)
		{
			blockers++;
			avgBlockerDistance += z;
		}

		z = tex.SampleLevel(state, projCoord.xy + PCSS_Samples[i] * uvLightSize * float2(1.0f / 128.0f, 1.0f / 128.0f), 2.0f).x;
		if (z < projCoord.z)
		{
			blockers++;
			avgBlockerDistance += z;
		}

		/*float zmax = 0.0;
		for (float j = 1.0f; j < 4.0f; j += 1.0f)
		{
			float z = tex.SampleLevel(state, projCoord.xy + PCSS_Samples[i] * uvLightSize * float2(1.0f / 128.0f, 1.0f / 128.0f), j).x;
			if (z > zmax)
			{
				zmax = z;
			}
		}

		if (zmax < projCoord.z)
		{
			blockers++;
			avgBlockerDistance += zmax;
		}*/
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

inline float ShadowMapPCMLSM(Texture2D<float2> tex, SamplerState state, float3 projCoord, float resolution, float pixelSize, float lightSize)
{
	float blockerDistance = PCMLSM_Blocker(tex, state, projCoord, lightSize);
	float penumbraSize = max((projCoord.z - blockerDistance) * lightSize * 0.25 / blockerDistance, 0.0f);
	return PCMLSM_ShadowMapMIPLerp(tex, state, projCoord, clamp(penumbraSize * 4.0f, 0.0f, 6.0f), resolution, pixelSize, 3.0);
}
