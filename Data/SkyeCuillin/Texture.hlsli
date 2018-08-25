float2 ParallaxMapping(float2 texCoord, float3 viewDir)
{
	float heightOffset = height.SampleLevel(anisoSampler, texCoord, 0.0f).x;
	float2 p = viewDir.xy / viewDir.z * (heightOffset * 0.004);
	return texCoord + p;
}

float2 SteepParallaxMapping(float2 texCoord, float3 viewDir)
{
	const float minLayers = 8.0;
	const float maxLayers = 32.0;
	float numLayers = lerp(maxLayers, minLayers, abs(dot(float3(0.0, 0.0, 1.0), viewDir)));

	float layerDepth = 1.0f / numLayers;
	float currentLayerDepth = 0.0f;

	float2 p = viewDir.xy * 0.02;
	float2 delta = p / numLayers;

	float2 currentCoords = texCoord;
	float currentDepth = 1.0 - height.SampleLevel(anisoSampler, currentCoords, 0.0f).x;

	while (currentLayerDepth < currentDepth)
	{
		currentCoords -= delta;
		currentDepth = 1.0 - height.SampleLevel(anisoSampler, currentCoords, 0.0f).x;
		currentLayerDepth += layerDepth;
	}

	return currentCoords;
}

float2 ParallaxOcclusionMapping(float2 texCoord, float3 viewDir)
{
	const float minLayers = 8.0;
	const float maxLayers = 32.0;
	float numLayers = lerp(maxLayers, minLayers, abs(dot(float3(0.0, 0.0, 1.0), viewDir)));

	float layerDepth = 1.0f / numLayers;
	float currentLayerDepth = 0.0f;

	float2 p = viewDir.xy * 0.05;
	float2 delta = p / numLayers;

	float2 currentCoords = texCoord;
	float currentDepth = 1.0 - height.SampleLevel(anisoSampler, currentCoords, 0.0f).x;

	while (currentLayerDepth < currentDepth)
	{
		currentCoords -= delta;
		currentDepth = 1.0 - height.SampleLevel(anisoSampler, currentCoords, 0.0f).x;
		currentLayerDepth += layerDepth;
	}

	float2 prevCoords = currentCoords + delta;

	float afterDepth = currentDepth - currentLayerDepth;
	float beforeDepth = 1.0 - height.SampleLevel(anisoSampler, prevCoords, 0.0f).x - currentLayerDepth + layerDepth;

	float weight = afterDepth / (afterDepth - beforeDepth);
	float2 finalCoords = prevCoords * weight + currentCoords * (1.0 - weight);

	return finalCoords;
}