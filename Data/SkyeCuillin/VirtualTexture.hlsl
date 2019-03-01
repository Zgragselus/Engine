cbuffer inputBuffer : register(b0)
{
	uint2 coordinates;
	uint2 resolution;
};

Texture2D<float> TileTexture : register(t0);
RWTexture2D<float> VirtualTexture : register(u0);

[numthreads(8, 8, 1)]
void CopyTile(uint GI : SV_GroupIndex, uint3 DTid : SV_DispatchThreadID)
{
	int3 uv = int3(DTid.x, DTid.y, 0);

	float v = TileTexture.Load(uv);
	VirtualTexture[coordinates + uv.xy] = v;
}