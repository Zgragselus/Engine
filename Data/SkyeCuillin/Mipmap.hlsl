//#include "Mipmap.hlsli"

SamplerState srcSampler : register(s0);
Texture2D<float4> srcLevel : register(t0);
RWTexture2D<float4> mipLevel1 : register(u0);
RWTexture2D<float4> mipLevel2 : register(u1);
RWTexture2D<float4> mipLevel3 : register(u2);
RWTexture2D<float4> mipLevel4 : register(u3);

cbuffer InputMiplevels : register(b0)
{
	uint srcMiplevel;
	uint miplevels;
	float2 texelSize;
}

groupshared float4 tmp[8];

void StoreColor(uint idx, float4 color)
{
	tmp[idx] = color;
}

float4 LoadColor(uint idx)
{
	return tmp[idx];
}

[numthreads(2, 2, 1)]
void GenerateMipmaps(uint GI : SV_GroupIndex, uint3 DTid : SV_DispatchThreadID)
{
	float4 src0;
	float4 src1;
	float4 src2;
	float4 src3;
	float2 uv = (DTid.xy + 0.5f) * texelSize;
	src0 = srcLevel.SampleLevel(srcSampler, uv, srcMiplevel);
	StoreColor(GI, src0);
	GroupMemoryBarrierWithGroupSync();

	if (GI == 0)
	{
		src1 = LoadColor(GI + 0x01);
		src2 = LoadColor(GI + 0x02);
		src3 = LoadColor(GI + 0x03);

		//mipLevel1[DTid.xy / 2] = float4(min(min(src0.x, src1.x), min(src2.x, src3.x)), max(max(src0.x, src1.x), max(src2.x, src3.x)), 0.0f, 0.0f);
		mipLevel1[DTid.xy / 2] = (src0 + src1 + src2 + src3) / 4;
	}
}

[numthreads(2, 2, 1)]
void GenerateMipmapsMinMax(uint GI : SV_GroupIndex, uint3 DTid : SV_DispatchThreadID)
{
	float4 src0;
	float4 src1;
	float4 src2;
	float4 src3;
	float2 uv = (DTid.xy + 0.5f) * texelSize;
	src0 = srcLevel.SampleLevel(srcSampler, uv, srcMiplevel);
	StoreColor(GI, src0);
	GroupMemoryBarrierWithGroupSync();

	if (GI == 0)
	{
		src1 = LoadColor(GI + 0x01);
		src2 = LoadColor(GI + 0x02);
		src3 = LoadColor(GI + 0x03);

		mipLevel1[DTid.xy / 2] = float4(min(min(src0.x, src1.x), min(src2.x, src3.x)), max(max(src0.x, src1.x), max(src2.x, src3.x)), 0.0f, 0.0f);
	}
}