Texture2DMS<float4, SamplesMSAA> diffuse : register(t0);
Texture2DMS<float, SamplesMSAA> depth : register(t1);
Texture2D<float> initialLuminance : register(t2);
Texture2D<float> adaptLuminanceIn : register(t3);
Texture2D<float> adaptLuminanceOut : register(t4);
Texture2D<float4> bloomPass : register(t5);
Texture2D<float4> bloomResult : register(t6);
Texture2D<float4> lensDirt : register(t7);
SamplerState diffuseSampler : register(s0);

cbuffer TonemapParameters : register(b0)
{
	uint2 dimensions;
	float2 offset;
	float DeltaTime;
	float Tau;
	float ShoulderStrength;
	float LinearStrength;
	float LinearAngle;
	float ToeStrength;
	float ToeNumerator;
	float ToeDenominator;
	float LinearWhite;
	float BloomBlurSigma;
	float BloomThreshold;
	float BloomMagnitude;
}

struct Input
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

Input VS(uint id : SV_VertexID, 
	float3 position : POSITION,
	float2 texCoord : TEXCOORD0)
{
	Input result;

	result.position = float4(position, 1.0f);
	result.texCoord = texCoord;

	return result;
}

struct Output
{
	float4 color : SV_TARGET;
	float depth : SV_DEPTH;
};

float CalcLuminance(float3 color)
{
	return max(dot(color, float3(0.299f, 0.587f, 0.114f)), 0.0001f);
}

float GetAvgLuminance(Texture2D<float> lumTex, float2 texCoord)
{
	return exp(lumTex.SampleLevel(diffuseSampler, texCoord, 10.0).x);
}

float3 ToneMapReinhard(float3 color)
{
	float pixelLuminance = CalcLuminance(color);
	float toneMappedLuminance = pixelLuminance / (pixelLuminance + 1);
	return toneMappedLuminance * pow(abs(color / pixelLuminance), 1.0f);
}

float3 ToneMapFilmicALU(float3 color)
{
	color = max(0, color - 0.004f);
	color = (color * (6.2f * color + 0.5f)) / (color * (6.2f * color + 1.7f) + 0.06f);

	// 1/2.2 baked in
	return pow(color, 2.2f);
}

// Function used by the Uncharte2D tone mapping curve
float3 U2Func(float3 x)
{
	float A = ShoulderStrength;
	float B = LinearStrength;
	float C = LinearAngle;
	float D = ToeStrength;
	float E = ToeNumerator;
	float F = ToeDenominator;
	return ((x*(A*x + C * B) + D * E) / (x*(A*x + B) + D * F)) - E / F;
}

// Applies the Uncharted 2 filmic tone mapping curve
float3 ToneMapFilmicU2(float3 color)
{
	float3 numerator = U2Func(color);
	float3 denominator = U2Func(LinearWhite);

	return numerator / denominator;
}

float3 CalcExposedColor(float3 color, float avgLuminance, float threshold, out float exposure)
{
	exposure = 0.0f;
	//float keyValue = 1.03f - (2.0f / (2 + log10(avgLuminance + 1)));
	float keyValue = 1.23f - (2.0f / (2 + log10(avgLuminance + 1)));
	float linearExposure = (keyValue / avgLuminance);
	exposure = log2(max(linearExposure, 0.0001f));
	exposure -= threshold;
	return exp2(exposure) * color;
}

float3 ToneMap(float3 color, float avgLuminance, float threshold, out float exposure)
{
	float pixelLuminance = CalcLuminance(color);
	color = CalcExposedColor(color, avgLuminance, threshold, exposure);
	//color = ToneMapReinhard(color);
	color = ToneMapFilmicU2(color);
	return color;
}

// Creates the luminance map for the scene
float4 LuminanceMap(Input input) : SV_Target
{
	// Sample the input
	uint2 coord = uint2(input.texCoord.x * dimensions.x, input.texCoord.y * dimensions.y);
	float3 color = diffuse.Load(coord, 0).xyz;

	// calculate the luminance using a weighted average
	float luminance = CalcLuminance(color);

	return float4(luminance, 1.0f, 1.0f, 1.0f);
}

// Slowly adjusts the scene luminance based on the previous scene luminance
float4 AdaptLuminance(Input input) : SV_Target
{
	float lastLum = exp(adaptLuminanceIn.Sample(diffuseSampler, input.texCoord).x);
	float currentLum = initialLuminance.Sample(diffuseSampler, input.texCoord).x;

	// Adapt the luminance using Pattanaik's technique    
	float adaptedLum = lastLum + (currentLum - lastLum) * (1 - exp(-DeltaTime * Tau));

	return float4(log(adaptedLum), 1.0f, 1.0f, 1.0f);
}

// Calculates the gaussian blur weight for a given distance and sigmas
float CalcGaussianWeight(int sampleDist, float sigma)
{
	float g = 1.0f / sqrt(2.0f * 3.14159 * sigma * sigma);
	return (g * exp(-(sampleDist * sampleDist) / (2 * sigma * sigma)));
}

float4 BloomFirstPass(Input input) : SV_Target
{
	// Sample the input
	uint2 coord = uint2(input.texCoord.x * dimensions.x, input.texCoord.y * dimensions.y);
	float3 color = float3(0.0f, 0.0f, 0.0f);

	float avgLuminance = GetAvgLuminance(adaptLuminanceOut, input.texCoord);
	float exposure = 0.0f;

	for (int i = -16; i <= 16; i++)
	{
		float weight = CalcGaussianWeight(i, BloomBlurSigma);
		float3 tmp = diffuse.Load(coord + uint2(i * 2, 0), 0).xyz;

		tmp.x = max(tmp.x, 0.0f);
		tmp.y = max(tmp.y, 0.0f);
		tmp.z = max(tmp.z, 0.0f);

		color += ToneMap(tmp * weight, avgLuminance, BloomThreshold, exposure);
	}

	return float4(color.xyz, 1.0f);
}

float4 BloomSecondPass(Input input) : SV_Target
{
	float3 color = float3(0.0f, 0.0f, 0.0f);

	for (int i = -16; i <= 16; i++)
	{
		float weight = CalcGaussianWeight(i, BloomBlurSigma);
		color += bloomPass.Sample(diffuseSampler, input.texCoord + float2(0.0f, i * offset.y * 2.0)).xyz * weight;
	}

	return float4(color, 1.0f);
}

float3 LensFlare(in float2 coord)
{
	float2 texcoord = -coord + float2(1.0f, 1.0f);
	float2 ghostVec = (float2(0.5f, 0.5f) - texcoord) * 0.6f;

	// sample ghosts:  
	float3 result = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 5; ++i) {
		float2 offset = frac(texcoord + ghostVec * float(i));

		float weight = length(float2(0.5f, 0.5f) - offset) / length(float2(0.5f, 0.5f));
		weight = pow(max(1.0f - weight, 0.0f), 10.0f);

		result += max(float3(0.0f, 0.0f, 0.0f), bloomResult.Sample(diffuseSampler, offset).xyz - 0.7f) * 4.0f * weight;
	}

	return result;
}

Output PS(Input input)
{
	Output result;

	uint2 coord = uint2(input.texCoord.x * dimensions.x, input.texCoord.y * dimensions.y);
	
	float3 tex = float3(0.0f, 0.0f, 0.0f);
	float z = 0.0f;
	float avgLuminance = GetAvgLuminance(adaptLuminanceOut, input.texCoord);
	float exposure = 0;
	float3 dirt = pow(abs(lensDirt.Sample(diffuseSampler, input.texCoord).xyz), 2.2);
#if SamplesMSAA == 1
	float3 color = ToneMap(diffuse.Load(coord, 0).xyz, avgLuminance, 0, exposure);
	float3 bloom = bloomResult.Sample(diffuseSampler, input.texCoord).xyz;
	float3 lensFlare = LensFlare(input.texCoord);
	color = color + bloom * BloomMagnitude + lensFlare * dirt * 0.5f;
	//color = color + bloom * BloomMagnitude;
	//color = diffuse.Load(coord, 0).xyz;
	color.x = pow(max(color.x, 0.0f), 1.0f / 2.2f);
	color.y = pow(max(color.y, 0.0f), 1.0f / 2.2f);
	color.z = pow(max(color.z, 0.0f), 1.0f / 2.2f);
	tex = color;
	z = depth.Load(coord, 0);
#else
	for (uint i = 0; i < SamplesMSAA; i++)
	{
		//float3 color = ToneMap(diffuse.Load(coord, i).xyz, luminance, 0, exposure);
		float3 color = ToneMap(diffuse.Load(coord, i).xyz, avgLuminance, 1.0, exposure);
		float3 bloom = bloomResult.Sample(diffuseSampler, input.texCoord).xyz;
		 color = color + bloom * BloomMagnitude;
		//color = diffuse.Load(coord, i).xyz;

		color.x = pow(max(color.x, 0.0f), 1.0f / 2.2f);
		color.y = pow(max(color.y, 0.0f), 1.0f / 2.2f);
		color.z = pow(max(color.z, 0.0f), 1.0f / 2.2f);
		
		//float3 color = diffuse.Load(coord, i).xyz * luminance;
		tex += color;
		z += depth.Load(coord, i);
	}
	tex *= 1.0f / float(SamplesMSAA);
	z *= 1.0f / float(SamplesMSAA);
#endif
		
	result.color = float4(tex.xyz, 1.0f);
	result.depth = z;

	return result;
}