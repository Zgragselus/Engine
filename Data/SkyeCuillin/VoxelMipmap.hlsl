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

groupshared float4 tmp[8];

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

// Naive version to generate single mipmap level from the previous one
//
// Runs in 2x2x2 workgroup
[numthreads(2, 2, 2)]
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

	// For first thread in workgroup only
	if (GI == 0)
	{
		// Load all 8 colors from shared memory
		src1 = LoadColor(GI + 0x01);
		src2 = LoadColor(GI + 0x02);
		src3 = LoadColor(GI + 0x03);
		src4 = LoadColor(GI + 0x04);
		src5 = LoadColor(GI + 0x05);
		src6 = LoadColor(GI + 0x06);
		src7 = LoadColor(GI + 0x07);

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
		mipLevel1[DTid / 2] = src0;
	}
}