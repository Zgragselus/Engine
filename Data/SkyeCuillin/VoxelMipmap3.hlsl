cbuffer InputDimensions : register(b0)
{
	uint3 dimensions;
}

cbuffer InputMiplevels : register(b1)
{
	uint srcMiplevel;
	uint miplevels;
	float texelSize;
}

SamplerState srcSampler : register(s0);
Texture3D<float4> srcLevel : register(t0);
RWTexture3D<float4> mipLevel1 : register(u0);
RWTexture3D<float4> mipLevel2 : register(u1);
RWTexture3D<float4> mipLevel3 : register(u2);

groupshared float4 tmp[512];

void StoreColor(uint idx, float4 color)
{
	tmp[idx] = color;
}

float4 LoadColor(uint idx)
{
	return tmp[idx];
}

float HasVoxel(float4 color)
{
	return color.a > 0.0f ? 1.0f : 0.0f;
}

// Naive version to generate two mipmap levels at most from the source one
//
// Runs in 4x4x4 workgroup
[numthreads(8, 8, 8)]
void GenerateMipmaps(uint GI : SV_GroupIndex, uint3 DTid : SV_DispatchThreadID)
{
	// Each thread in workgroup loads single voxel and stores it on groupshared memory
	float4 src0;
	float4 src1;
	float4 src2;
	float4 src3;
	float4 src4;
	float4 src5;
	float4 src6;
	float4 src7;
	float3 uvw = (DTid.xyz + 0.5f) * texelSize;
	src0 = srcLevel.SampleLevel(srcSampler, uvw, (float)srcMiplevel);
	StoreColor(GI, src0);
	GroupMemoryBarrierWithGroupSync();

	// 001001001b 
	if ((GI & 0x49) == 0)
	{
		src1 = LoadColor(GI + 0x01);
		src2 = LoadColor(GI + 0x08);
		src3 = LoadColor(GI + 0x09);
		src4 = LoadColor(GI + 0x40);
		src5 = LoadColor(GI + 0x41);
		src6 = LoadColor(GI + 0x48);
		src7 = LoadColor(GI + 0x49);

		// Perform mipmapping function
		float div = HasVoxel(src0) + HasVoxel(src1) + HasVoxel(src2) + HasVoxel(src3) + HasVoxel(src4) + HasVoxel(src5) + HasVoxel(src6) + HasVoxel(src7);

		if (div == 0.0f)
		{
			src0 = 0.0f;
		}
		else
		{
			src0 = (src0 + src1 + src2 + src3 + src4 + src5 + src6 + src7) / div;
		}

		// Store value + write into shared memory
		mipLevel1[DTid / 2] = src0;
		StoreColor(GI, src0);
	}

	// If we are going to generate just 1 mipmap, return here
	if (miplevels == 1)
	{
		return;
	}

	GroupMemoryBarrierWithGroupSync();

	// 011011011b
	if ((GI & 0xDB) == 0)
	{
		// Load all 8 colors from shared memory
		src1 = LoadColor(GI + 0x02);
		src2 = LoadColor(GI + 0x10);
		src3 = LoadColor(GI + 0x12);
		src4 = LoadColor(GI + 0x80);
		src5 = LoadColor(GI + 0x82);
		src6 = LoadColor(GI + 0x90);
		src7 = LoadColor(GI + 0x92);

		// Perform mipmapping function
		float div = HasVoxel(src0) + HasVoxel(src1) + HasVoxel(src2) + HasVoxel(src3) + HasVoxel(src4) + HasVoxel(src5) + HasVoxel(src6) + HasVoxel(src7);

		if (div == 0.0f)
		{
			src0 = 0.0f;
		}
		else
		{
			src0 = (src0 + src1 + src2 + src3 + src4 + src5 + src6 + src7) / div;
		}

		// Store value + write into shared memory
		mipLevel2[DTid / 4] = src0;
		StoreColor(GI, src0);
	}

	// If we are going to generate just 1 mipmap, return here
	if (miplevels == 2)
	{
		return;
	}

	GroupMemoryBarrierWithGroupSync();

	// For first thread in workgroup only
	if (GI == 0)
	{
		// Load all 8 colors from shared memory
		src1 = LoadColor(GI + 0x04);
		src2 = LoadColor(GI + 0x20);
		src3 = LoadColor(GI + 0x24);
		src4 = LoadColor(GI + 0x100);
		src5 = LoadColor(GI + 0x104);
		src6 = LoadColor(GI + 0x120);
		src7 = LoadColor(GI + 0x124);

		// Perform mipmapping function
		float div = HasVoxel(src0) + HasVoxel(src1) + HasVoxel(src2) + HasVoxel(src3) + HasVoxel(src4) + HasVoxel(src5) + HasVoxel(src6) + HasVoxel(src7);

		if (div == 0.0f)
		{
			src0 = 0.0f;
		}
		else
		{
			src0 = (src0 + src1 + src2 + src3 + src4 + src5 + src6 + src7) / div;
		}

		// Store value
		mipLevel3[DTid / 8] = src0;
	}
}