inline uint PackRGB10A2(uint4 vec)
{
	uint data = 0;
	data += (vec.x & 0x3ff) << 22;
	data += (vec.y & 0x3ff) << 12;
	data += (vec.z & 0x3ff) << 2;
	data += (vec.w & 0x3);
	return data;
}

inline uint4 UnpackRGB10A2(uint data)
{
	uint4 vec;
	vec.w = data & 0x3;
	data >>= 2;
	vec.z = data & 0x3ff;
	data >>= 10;
	vec.y = data & 0x3ff;
	data >>= 10;
	vec.x = data & 0x3ff;
	return vec;
}

inline float4 UnpackRGBA(uint v) 
{
	float4 rgba = float4(v >> 24, (v & 0x00ff0000) >> 16, (v & 0x0000ff00) >> 8, v & 0x000000ff) / 255.0;
	return rgba;
}

inline uint PackRGBA(float4 rgba) 
{
	uint v = (uint)(rgba.x * 255.0) << 24;
	v += (uint)(rgba.y * 255.0) << 16;
	v += (uint)(rgba.z * 255.0) << 8;
	v += (uint)(rgba.w * 255.0);
	return v;
}

inline int3 VoxelToTexture(uint idx)
{
	return int3(idx % 512, (idx / 512) % 512, idx / (512 * 512)) * 3 + int3(1, 1, 1);
}

inline uint TextureToVoxel(uint3 coord)
{
	return (coord.x + coord.y * 512 + coord.z * 512 * 512);
}