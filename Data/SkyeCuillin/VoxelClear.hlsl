cbuffer InputColor : register(b0)
{
	float3 color;
}

RWTexture3D<float4> voxel : register(u0);

[numthreads(8, 8, 8)]
void CS(uint GI : SV_GroupIndex, uint3 DTid : SV_DispatchThreadID)
{
	voxel[DTid] = float4(color, 0.0f);
}