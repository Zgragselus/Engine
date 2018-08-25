#ifndef NON_POWER_OF_TWO
#define NON_POWER_OF_TWO 0
#endif

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

groupshared float4 tmp[64];

void StoreColor(uint idx, float4 color)
{
	tmp[idx] = color;
}

float4 LoadColor(uint idx)
{
	return tmp[idx];
}

[numthreads(8, 8, 1)]
void GenerateMipmaps(uint GI : SV_GroupIndex, uint3 DTid : SV_DispatchThreadID)
{
	// One bilinear sample is insufficient when scaling down by more than 2x.
	// You will slightly undersample in the case where the source dimension
	// is odd.  This is why it's a really good idea to only generate mips on
	// power-of-two sized textures.  Trying to handle the undersampling case
	// will force this shader to be slower and more complicated as it will
	// have to take more source texture samples.
#if NON_POWER_OF_TWO == 0
	float2 uv = (DTid.xy + 0.5f) * texelSize;
	float4 src1 = srcLevel.SampleLevel(srcSampler, uv, srcMiplevel);
#elif NON_POWER_OF_TWO == 1
	// > 2:1 in X dimension
	// Use 2 bilinear samples to guarantee we don't undersample when downsizing by more than 2x
	// horizontally.
	float2 uv1 = (DTid.xy + float2(0.25, 0.5)) * texelSize;
	float2 offset = texelSize * float2(0.5, 0.0);
	float4 src1 = 0.5 * (srcLevel.SampleLevel(srcSampler, uv1, srcMiplevel) +
		srcLevel.SampleLevel(srcSampler, uv1 + offset, srcMiplevel));
#elif NON_POWER_OF_TWO == 2
	// > 2:1 in Y dimension
	// Use 2 bilinear samples to guarantee we don't undersample when downsizing by more than 2x
	// vertically.
	float2 uv1 = (DTid.xy + float2(0.5, 0.25)) * texelSize;
	float2 offset = texelSize * float2(0.0, 0.5);
	float4 src1 = 0.5 * (srcLevel.SampleLevel(srcSampler, uv1, srcMiplevel) +
		srcLevel.SampleLevel(srcSampler, uv1 + offset, srcMiplevel));
#elif NON_POWER_OF_TWO == 3
	// > 2:1 in in both dimensions
	// Use 4 bilinear samples to guarantee we don't undersample when downsizing by more than 2x
	// in both directions.
	float2 uv1 = (DTid.xy + float2(0.25, 0.25)) * texelSize;
	float2 offset = texelSize * 0.5;
	float4 src1 = srcLevel.SampleLevel(srcSampler, uv1, srcMiplevel);
	src1 += srcLevel.SampleLevel(srcSampler, uv1 + float2(offset.x, 0.0), srcMiplevel);
	src1 += srcLevel.SampleLevel(srcSampler, uv1 + float2(0.0, offset.y), srcMiplevel);
	src1 += srcLevel.SampleLevel(srcSampler, uv1 + float2(offset.x, offset.y), srcMiplevel);
	src1 *= 0.25;
#endif

	mipLevel1[DTid.xy] = PackColor(Src1);

	// A scalar (constant) branch can exit all threads coherently.
	if (miplevels == 1)
		return;

	// Without lane swizzle operations, the only way to share data with other
	// threads is through LDS.
	StoreColor(GI, Src1);

	// This guarantees all LDS writes are complete and that all threads have
	// executed all instructions so far (and therefore have issued their LDS
	// write instructions.)
	GroupMemoryBarrierWithGroupSync();

	// With low three bits for X and high three bits for Y, this bit mask
	// (binary: 001001) checks that X and Y are even.
	if ((GI & 0x9) == 0)
	{
		float4 src2 = LoadColor(GI + 0x01);
		float4 src3 = LoadColor(GI + 0x08);
		float4 src4 = LoadColor(GI + 0x09);
		src1 = 0.25 * (src1 + src2 + src3 + src4);

		mipLevel2[DTid.xy / 2] = PackColor(src1);
		StoreColor(GI, src1);
	}

	if (miplevels == 2)
		return;

	GroupMemoryBarrierWithGroupSync();

	// This bit mask (binary: 011011) checks that X and Y are multiples of four.
	if ((GI & 0x1B) == 0)
	{
		float4 src2 = LoadColor(GI + 0x02);
		float4 src3 = LoadColor(GI + 0x10);
		float4 src4 = LoadColor(GI + 0x12);
		src1 = 0.25 * (src1 + src2 + src3 + src4);

		mipLevel3[DTid.xy / 4] = PackColor(src1);
		StoreColor(GI, src1);
	}

	if (miplevels == 3)
		return;

	GroupMemoryBarrierWithGroupSync();

	// This bit mask would be 111111 (X & Y multiples of 8), but only one
	// thread fits that criteria.
	if (GI == 0)
	{
		float4 src2 = LoadColor(GI + 0x04);
		float4 src3 = LoadColor(GI + 0x20);
		float4 src4 = LoadColor(GI + 0x24);
		src1 = 0.25 * (src1 + src2 + src3 + src4);

		mipLevel4[DTid.xy / 8] = PackColor(src1);
	}
}