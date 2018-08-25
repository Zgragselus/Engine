#include "Voxel.hlsli"

cbuffer inputBuffer : register(b0)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4 offset;
	float4 scale;
	float4 cameraPos;
	float4 aabbMin;
	float4 aabbMax;
	float4 pad[3];
};

Texture2D diffuse : register(t0);
//Texture3D voxel : register(t1);
SamplerState diffuseSampler : register(s0);
SamplerState voxelSampler : register(s1);
StructuredBuffer<unsigned int> octreeNodes : register(t1);
Texture3D octreeData : register(t2);

struct Input
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal : TEXCOORD1;
	float3 voxelCoord : TEXCOORD2;
	float3 worldPos : TEXCOORD3;
};

Input VS(uint id : SV_VertexID, 
	float3 position : POSITION, 
	float3 normal : NORMAL,
	float2 texCoord : TEXCOORD0)
{
	Input result;

	result.worldPos = position - cameraPos.xyz;
	result.voxelCoord = (position.xyz + offset.xyz) * scale.xyz * 2.0f - 1.0f;
	result.position = mul(projectionMatrix, mul(viewMatrix, float4(position, 1.0f)));
	result.texCoord = texCoord;
	result.normal = normal;

	return result;
}

/*float3 ScaleAndBias(const float3 p)
{
	return p.xyz * 256.0;
}

float3 ComputeDeltas(const float3 p, const float3 d)
{
	float3 delta = float3(length(d.xyz / d.xxx), length(d.xyz / d.yyy), length(d.xyz / d.zzz));

	return delta;
}

float3 ComputeIntersections(const float3 direction, const float3 delta)
{
	return abs(delta / direction);
}

float4 SampleVoxel(int x, int y, int z)
{
	return voxel.Load(int4(x, y, z, 0));
}

float4 TraceTest(float3 from, float3 dir)
{
	float3 origin = ScaleAndBias(from);
	dir = normalize(dir);
	float rayOrigin[3] = { origin.x, origin.y, origin.z };
	float rayDirection[3] = { dir.x, dir.y, dir.z };
	int coord[3] = { int(rayOrigin[0]), int(rayOrigin[1]), int(rayOrigin[2]) };
	float deltaDist[3];
	float next[3];
	int step[3];
	float4 result = float4(0.0f, 0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 3; i++) 
	{
		float x = (rayDirection[0] / rayDirection[i]);
		float y = (rayDirection[1] / rayDirection[i]);
		float z = (rayDirection[2] / rayDirection[i]);
		deltaDist[i] = sqrt(x * x + y * y + z * z);
		if (rayDirection[i] < 0.0f) 
		{
			step[i] = -1;
			next[i] = (rayOrigin[i] - coord[i]) * deltaDist[i];
		}
		else 
		{
			step[i] = 1;
			next[i] = (coord[i] + 1.0f - rayOrigin[i]) * deltaDist[i];
		}
	}

	while (result.a < 1.0f)
	{
		int side = 0;
		for (int i = 1; i < 3; i++)
		{
			if (next[side] > next[i])
			{
				side = i;
			}
		}

		next[side] += deltaDist[side];
		coord[side] += step[side];
		rayOrigin[side] += (float)step[side];

		if (coord[side] < 0 || coord[side] >= 256) 
		{
			break;
		}

		result = voxel.SampleLevel(voxelSampler, float3(coord[0], coord[1], coord[2]) / 512.0, 0.0f);
	}

	return result;
}*/

float simple_noise(in float2 co)
{
	return frac(sin(dot(co.xy, float2(12.9898f, 78.233f))) * 43758.5453f);
}

/*float4 voxel_fetch(in float3 sample_pos, in float3 vV, in float mip)
{
	float4 result = voxel.SampleLevel(voxelSampler, sample_pos, mip);
	return result;
}

float4 trace_cone(in float3 origin, in float3 dir, in float cone_ratio, in float max_dist, in float bias)
{
	// minimum diameter is half the sample size to avoid hitting empty space
	float min_voxel_diameter = 0.5 / 512.0;
	float min_voxel_diameter_inv = 1.0 / min_voxel_diameter;

	float4 accum = 0.f;

	// max step counter
	uint steps = 0;

	// push out the starting point to avoid self-intersection
	float dist = min_voxel_diameter;

	dist *= bias * cone_ratio;
	
	while (dist < max_dist && accum.w < 1.0)
	{
		float sample_diameter = max(min_voxel_diameter, cone_ratio * dist);

		float sample_lod = log2(sample_diameter * min_voxel_diameter_inv);

		float3 sample_pos = origin + dir * dist;
		
		float n = simple_noise(origin.xz) * sample_lod;
		float nn = 1.0 + n * sample_diameter;

		dist += sample_diameter;

		float4 sample_value = voxel_fetch(sample_pos, -dir, sample_lod);

		float a = 1.0 - accum.w;
		accum += sample_value * a;

		steps++;
	}

	return accum;
}


float trace_shadow_cone(in float3 P, in float3 V, in float cone_ratio, in float max_dist, in float step_mult)
{
	float min_voxel_diameter = 0.5 / 512.0;
	float min_voxel_diameter_inv = 1.0 / min_voxel_diameter;

	float accum = 0.f;

	// push out the starting point to avoid self-intersection
	float dist = min_voxel_diameter;
	
	while (dist <= max_dist && accum < 1.0f)
	{
		float sample_diameter = max(min_voxel_diameter, cone_ratio * dist);

		float sample_lod = log2(sample_diameter * min_voxel_diameter_inv);

		float3 sample_pos = P + V * dist;
		
		dist += sample_diameter * step_mult;

		float sample_value = voxel.SampleLevel(voxelSampler, sample_pos, sample_lod).a * 0.1;

		float a = 1.0 - accum;
		accum += sample_value * a;
	}

	float shadowing = saturate(1.0 - accum);

	return shadowing;
}*/

float4 sample_voxel(float3 coord, float level)
{
	float scale = 512.0 / pow(2.0, (float)level);

	uint4 position = uint4(coord.x * 512.0, coord.y * 512.0, coord.z * 512.0, 0);
	float3 posTmp = coord * 512.0 - float3(position.xyz);
	uint dimensions = 512;
	int3 nodeMin = int3(0, 0, 0);
	int3 nodeMax = int3(512, 512, 512);

	int currentId = 0;
	uint node = octreeNodes[currentId];
	uint child = 0;
	bool flag = true;

	int offsetX;
	int offsetY;
	int offsetZ;

	for (uint i = 0; i < (uint)level; i++)
	{
		dimensions /= 2;

		if ((node & 0x80000000) == 0)
		{
			flag = false;
			break;
		}

		child = (node & 0x7FFFFFFF);

		offsetX = 0;
		offsetY = 0;
		offsetZ = 0;
		if (((int)position.x - nodeMin.x) >= (int)dimensions)
		{
			offsetX = 1;
		}
		if (((int)position.y - nodeMin.y) >= (int)dimensions)
		{
			offsetY = 1;
		}
		if (((int)position.z - nodeMin.z) >= (int)dimensions)
		{
			offsetZ = 1;
		}

		child += offsetX + 2 * offsetY + 4 * offsetZ;
		nodeMin.x += dimensions * offsetX;
		nodeMin.y += dimensions * offsetY;
		nodeMin.z += dimensions * offsetZ;
		nodeMax.x = nodeMin.x + dimensions;
		nodeMax.y = nodeMin.y + dimensions;
		nodeMax.z = nodeMin.z + dimensions;
		currentId = child;
		node = octreeNodes[currentId];
	}

	uint width;
	uint height;
	uint depth;
	uint levels;
	octreeData.GetDimensions(0, width, height, depth, levels);
	float3 brickCoord = float3(VoxelToTexture(currentId));

	dimensions /= 2;
	if (((int)position.x - nodeMin.x) >= (int)dimensions)
	{
		brickCoord.x += 0.5f;
	}
	else
	{
		brickCoord.x -= 0.5f;
	}

	if (((int)position.y - nodeMin.y) >= (int)dimensions)
	{
		brickCoord.y += 0.5f;
	}
	else
	{
		brickCoord.y -= 0.5f;
	}
	
	if (((int)position.z - nodeMin.z) >= (int)dimensions)
	{
		brickCoord.z += 0.5f;
	}
	else
	{
		brickCoord.z -= 0.5f;
	}

	brickCoord += posTmp * 0.5f;

	float3 texCoord = float3(brickCoord.x / (float)width, brickCoord.y / (float)height, brickCoord.z / (float)depth);

	float4 result = float4(0.0, 0.0, 0.0, 0.0);

	if (flag == true)
	{
		//result = UnpackRGBA(octreeData[currentId]);
		//result = octreeData.Load(int4(VoxelToTexture(currentId), 0));
		result = octreeData.SampleLevel(voxelSampler, texCoord, 0.0f);
	}

	return result;
}

float4 sample_svo(in float3 coord, in float miplevel, out float maxLevel, out int nodeSample)
{
	float levelinterp = frac(miplevel);
	int lowerLevel = max(int(floor(miplevel)), 0);
	int level = int(ceil(miplevel));
	uint4 position = uint4(coord.x * 512.0, coord.y * 512.0, coord.z * 512.0, 0);
	uint dimensions = 512;
	int3 nodeMin = int3(0, 0, 0);
	int3 nodeMax = int3(512, 512, 512);

	int currentId = 0;
	int parentId = 0;
	uint node = octreeNodes[currentId];
	uint child = 0;
	bool flag = true;

	int offsetX;
	int offsetY;
	int offsetZ;

	maxLevel = 512.0f;
	
	for (uint i = 0; i < (uint)level; i++)
	{
		dimensions /= 2;

		if ((node & 0x80000000) == 0)
		{
			flag = false;
			break;
		}

		child = (node & 0x7FFFFFFF);

		offsetX = 0;
		offsetY = 0;
		offsetZ = 0;
		if (((int)position.x - nodeMin.x) >= (int)dimensions)
		{
			offsetX = 1;
		}
		if (((int)position.y - nodeMin.y) >= (int)dimensions)
		{
			offsetY = 1;
		}
		if (((int)position.z - nodeMin.z) >= (int)dimensions)
		{
			offsetZ = 1;
		}

		child += offsetX + 2 * offsetY + 4 * offsetZ;
		nodeMin.x += dimensions * offsetX;
		nodeMin.y += dimensions * offsetY;
		nodeMin.z += dimensions * offsetZ;
		nodeMax.x = nodeMin.x + dimensions;
		nodeMax.y = nodeMin.y + dimensions;
		nodeMax.z = nodeMin.z + dimensions;
		parentId = currentId;
		currentId = child;
		node = octreeNodes[currentId];

		maxLevel /= 2.0f;
	}

	uint width;
	uint height;
	uint depth;
	uint levels;
	octreeData.GetDimensions(0, width, height, depth, levels);
	float3 brickCoord = float3(VoxelToTexture(currentId));
	float3 posTmp = frac(coord * 512.0 / dimensions) * 2.0f - 1.0f;
	brickCoord += 0.5f;
	brickCoord.x += posTmp.x;
	brickCoord.y += posTmp.y;
	brickCoord.z += posTmp.z;

	uint parentDimensions = min(512, dimensions * 2);
	float3 parentBrickCoord = float3(VoxelToTexture(parentId));
	float3 parentOffset = frac(coord * 512.0 / parentDimensions) * 2.0f - 1.0f;
	parentBrickCoord += 0.5f;
	parentBrickCoord.x += parentOffset.x;
	parentBrickCoord.y += parentOffset.y;
	parentBrickCoord.z += parentOffset.z;

	float3 texCoord = float3(brickCoord.x / (float)width, brickCoord.y / (float)height, brickCoord.z / (float)depth);
	float3 parentCoord = float3(parentBrickCoord.x / (float)width, parentBrickCoord.y / (float)height, parentBrickCoord.z / (float)depth);
	float4 result = float4(0.0, 0.0, 0.0, 0.0);

	if (flag == true)
	{
		maxLevel = 1.0f;
		result = lerp(octreeData.SampleLevel(voxelSampler, parentCoord, 0.0f), octreeData.SampleLevel(voxelSampler, texCoord, 0.0f), levelinterp);
	}

	nodeSample = currentId;

	return result;
}

bool3 lessThanEqual(float3 a, float3 b)
{
	return bool3(a.x <= b.x, a.y <= b.y, a.z <= b.z);
}

float4 traversal(in float3 origin, in float3 direction)
{
	/*uint MAX_RAY_STEPS = 64;
	
	float3 mapPos = float3(floor(origin.x * 512.0f), floor(origin.y * 512.0f), floor(origin.z * 512.0f));
	float dirLen = length(direction);
	float3 deltaDist = abs(float3(dirLen, dirLen, dirLen) / direction);
	int3 rayStep = int3(sign(direction));
	float3 sideDist = (sign(direction) * (float3(mapPos) - (origin * 512.0f)) + (sign(direction) * 0.5) + 0.5) * deltaDist;
	bool3 mask = bool3(false, false, false);
	float level = 8.0f;

	float4 result = float4(0.0f, 0.0f, 0.0f, 0.0f);

	for (uint i = 0; i < MAX_RAY_STEPS; i++)
	{
		result = sample_svo(mapPos / 512.0f, 7.999f, level);
		result.z = min(frac(sideDist.x), min(frac(sideDist.y), frac(sideDist.z)));
		if (result.w > 0.5f)
		{
			break;
		}

		mask = lessThanEqual(sideDist.xyz, min(sideDist.yzx, sideDist.zxy));
		sideDist += float3(mask) * deltaDist;
		mapPos += int3(mask) * rayStep;
	}
	return result;*/

	uint MAX_RAY_STEPS = 512;

	float4 result = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 voxel = float3(floor(origin.x * 512.0f), floor(origin.y * 512.0f), floor(origin.z * 512.0f));
	float3 step = sign(direction);
	float3 tmax = (voxel - origin * 512.0f) / direction;
	float3 tdelta = 1.0f / abs(direction);
	float level;
	float hitT = 0.0f;
	int prevNode;
	int node = 0;

	int steps = 0;
	for (uint i = 0; i < MAX_RAY_STEPS; i++)
	{
		if (voxel.x < 0.0f || voxel.y < 0.0f || voxel.z < 0.0f ||
			voxel.x > 512.0f || voxel.y > 512.0f || voxel.z > 512.0f)
		{
			break;
		}

		/*if (prevNode != node)
		{
			steps++;
		}
		prevNode = node;*/

		result = sample_svo(voxel / 512.0f, 7.999f, level, node);
		if (result.w > 0.5f)
		{
			break;
		}

		bool c1 = tmax.x < tmax.y;
		bool c2 = tmax.x < tmax.z;
		bool c3 = tmax.y < tmax.z;

		if (c1 && c2)
		{
			voxel.x += step.x;
			tmax.x += tdelta.x;
			hitT = tmax.x;
		}
		else if (c3 && !c1)
		{
			voxel.y += step.y;
			tmax.y += tdelta.y;
			hitT = tmax.y;
		}
		else
		{
			voxel.z += step.z;
			tmax.z += tdelta.z;
			hitT = tmax.z;
		}

		steps++;
	}

	/*result.x = (float)steps / 64.0f;
	result.y = (float)steps / 512.0f;
	result.z = 0.0f;*/

	return result;
}

float4 trace_cone(in float3 origin, in float3 dir, in float cone_ratio, in float max_dist, in float bias)
{
	// minimum diameter is half the sample size to avoid hitting empty space
	float min_voxel_diameter = 0.5 / 512.0;
	float min_voxel_diameter_inv = 1.0 / min_voxel_diameter;

	float4 accum = 0.f;

	// max step counter
	uint steps = 0;

	// push out the starting point to avoid self-intersection
	float dist = min_voxel_diameter;

	dist *= bias * cone_ratio;
	float level;
	int node;

	while (dist < max_dist && accum.w < 1.0)
	{
		float sample_diameter = max(min_voxel_diameter, cone_ratio * dist);

		float sample_lod = log2(sample_diameter * min_voxel_diameter_inv);

		float3 sample_pos = origin + dir * dist;

		float n = simple_noise(origin.xz) * sample_lod;
		float nn = 1.0 + n * sample_diameter;

		dist += sample_diameter;

		float4 sample_value = sample_svo(sample_pos, 7.999 - clamp(sample_lod, 0.0, 7.999), level, node);

		float a = 1.0 - accum.w;
		accum += sample_value * a;

		steps++;
	}

	return accum;
}

float trace_shadow_cone(in float3 P, in float3 V, in float cone_ratio, in float max_dist, in float step_mult)
{
	float min_voxel_diameter = 0.5 / 512.0;
	float min_voxel_diameter_inv = 1.0 / min_voxel_diameter;

	float accum = 0.f;

	// push out the starting point to avoid self-intersection
	float dist = min_voxel_diameter;

	while (dist <= max_dist && accum < 1.0f)
	{
		float sample_diameter = max(min_voxel_diameter, cone_ratio * dist * 0.5f);

		float sample_lod = log2(sample_diameter * min_voxel_diameter_inv);

		float3 sample_pos = P + V * dist;

		dist += sample_diameter * step_mult;
		//dist += min_voxel_diameter * step_mult;

		//float sample_value = sample_svo(sample_pos, 7.999 - clamp(sample_lod, 0.0, 7.999)).a * 0.05;

		float a = 1.0 - accum;
		//accum += sample_value * a;
	}

	float shadowing = saturate(1.0 - accum);

	return shadowing;
}

float4 stack_traversal(in float3 origin, in float3 direction)
{
	float4 stack[1024];
	uint stackPos = 0;
	uint level = 0;
	uint node = octreeNodes[0];

	float3 t0 = (float3(0.0f, 0.0f, 0.0f) - origin) / direction;
	float3 t1 = (float3(1.0f, 1.0f, 1.0f) - origin) / direction;
		
	int i = 0;
	bool hit = false;
	float hitT = 10000000.0f;
	uint hitNode = 0;

	// Search for leaves
	[allow_uav_condition] while (true)
	{
		// If current node is empty
		if ((node & 0x80000000) == 0)
		{
			// Nothing in stack -> exit out
			if (stackPos == 0)
			{
				break;
			}

			// Otherwise, get nonprocessed node from stack and continue
			stackPos--;
			float4 tmp0 = stack[stackPos * 2];
			float4 tmp1 = stack[stackPos * 2 + 1];
			t0 = tmp0.xyz;
			t1 = tmp1.xyz;
			node = asuint(tmp0.w);
			level = asuint(tmp1.w);
			continue;
		}

		// Calculate entry and exit point into node
		float3 tnear = min(t0, t1);
		float3 tfar = max(t0, t1);
		float tenter = max(max(tnear.x, 0.0f), max(tnear.y, tnear.z));
		float texit = min(tfar.x, min(tfar.y, tfar.z));

		// If ray misses the node OR is further than current nearest
		if ((tenter > texit) || (texit < 0.0) || (tenter > hitT))
		{
			// Nothing in stack -> exit out
			if (stackPos == 0)
			{
				break;
			}

			// Otherwise, get nonprocessed node from stack and continue
			stackPos--;
			float4 tmp0 = stack[stackPos * 2];
			float4 tmp1 = stack[stackPos * 2 + 1];
			t0 = tmp0.xyz;
			t1 = tmp1.xyz;
			node = asuint(tmp0.w);
			level = asuint(tmp1.w);
			continue;
		}

		// If we are on the target leaf level
		if (level == 6)
		{
			if (texit < hitT)
			{
				hitT = texit;
				hitNode = node;
				hit = true;
			}

			if (stackPos == 0)
			{
				break;
			}

			// Otherwise, get nonprocessed node from stack and continue
			stackPos--;
			float4 tmp0 = stack[stackPos * 2];
			float4 tmp1 = stack[stackPos * 2 + 1];
			t0 = tmp0.xyz;
			t1 = tmp1.xyz;
			node = asuint(tmp0.w);
			level = asuint(tmp1.w);
			continue;
		}

		// Otherwise we're gonna search children
		// Calculate splitting planes
		float3 tM = (t0 + t1) * 0.5f;

		// Insert children onto stack (one is put directly to work
		// (X, -Y, -Z)
		stack[stackPos * 2].xyz = float3(tM.x, t0.y, t0.z);
		stack[stackPos * 2].w = asfloat(octreeNodes[(node & 0x7fffffff) + 1]);
		stack[stackPos * 2 + 1].xyz = float3(t1.x, tM.y, tM.z);
		stack[stackPos * 2 + 1].w = asfloat(level + 1);
		stackPos++;

		// (-X, Y, -Z)
		stack[stackPos * 2].xyz = float3(t0.x, tM.y, t0.z);
		stack[stackPos * 2].w = asfloat(octreeNodes[(node & 0x7fffffff) + 2]);
		stack[stackPos * 2 + 1].xyz = float3(tM.x, t1.y, tM.z);
		stack[stackPos * 2 + 1].w = asfloat(level + 1);
		stackPos++;

		// (X, Y, -Z)
		stack[stackPos * 2].xyz = float3(tM.x, tM.y, t0.z);
		stack[stackPos * 2].w = asfloat(octreeNodes[(node & 0x7fffffff) + 3]);
		stack[stackPos * 2 + 1].xyz = float3(t1.x, t1.y, tM.z);
		stack[stackPos * 2 + 1].w = asfloat(level + 1);
		stackPos++;

		// (-X, -Y, Z)
		stack[stackPos * 2].xyz = float3(t0.x, t0.y, tM.z);
		stack[stackPos * 2].w = asfloat(octreeNodes[(node & 0x7fffffff) + 4]);
		stack[stackPos * 2 + 1].xyz = float3(tM.x, tM.y, t1.z);
		stack[stackPos * 2 + 1].w = asfloat(level + 1);
		stackPos++;

		// (X, -Y, Z)
		stack[stackPos * 2].xyz = float3(tM.x, t0.y, tM.z);
		stack[stackPos * 2].w = asfloat(octreeNodes[(node & 0x7fffffff) + 5]);
		stack[stackPos * 2 + 1].xyz = float3(t1.x, tM.y, t1.z);
		stack[stackPos * 2 + 1].w = asfloat(level + 1);
		stackPos++;

		// (-X, Y, Z)
		stack[stackPos * 2].xyz = float3(t0.x, tM.y, tM.z);
		stack[stackPos * 2].w = asfloat(octreeNodes[(node & 0x7fffffff) + 6]);
		stack[stackPos * 2 + 1].xyz = float3(tM.x, t1.y, t1.z);
		stack[stackPos * 2 + 1].w = asfloat(level + 1);
		stackPos++;

		// (X, Y, Z)
		stack[stackPos * 2].xyz = float3(tM.x, tM.y, tM.z);
		stack[stackPos * 2].w = asfloat(octreeNodes[(node & 0x7fffffff) + 7]);
		stack[stackPos * 2 + 1].xyz = float3(t1.x, t1.y, t1.z);
		stack[stackPos * 2 + 1].w = asfloat(level + 1);
		stackPos++;

		// First child can be searched immediately
		node = octreeNodes[(node & 0x7fffffff)];
		t0 = t0;
		t1 = tM;
		level++;
	}

	if (hit == false)
	{
		return float4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	else
	{
		return octreeData[VoxelToTexture(hitNode & 0x7fffffff)];
	}
}

float4 PS(Input input) : SV_TARGET
{
	float3 n = normalize(input.normal);
	float light = max(dot(n, normalize(float3(-0.2f, 1.0f, 0.3f))), 0.0f);
	float3 r = normalize(reflect(normalize(input.worldPos), n));

	float4 tex = diffuse.Sample(diffuseSampler, input.texCoord);
	float3 voxCoord = input.voxelCoord.xyz * 0.5 + 0.5;
	//float4 vox = sample_voxel(voxCoord, 4.0);
	//float4 vox = voxel.SampleLevel(voxelSampler, float3(voxCoord.x, voxCoord.y, voxCoord.z), 0.0f);

	//float4 test = TraceTest(voxCoord + n / 512.0, r);
	//float4 test = trace_cone(voxCoord + n / 512.0, r, 0.1, 1.0, 0.0);
	//float4 test = sample_svo(voxCoord, 7.999);
	float4 test = traversal(voxCoord + n / 256.0, r);
	//float ao = trace_shadow_cone(voxCoord + n / 512.0, n, 0.9, 0.5, 0.5);
	//ao = pow(ao, 2.0);
	//ao = min(1.0f, ao + 0.2f);

	/*float4 test = float4(0.0f, 0.0f, 0.0f, 0.0f);
	if (n.y > 0.95)
	{
		//test = 0.5f * trace_cone(voxCoord + n / 512.0, normalize(r + 0.2 * (tex.xyz * 2.0 - 1.0)), 0.05, 1.0, 0.0) + 0.5f * tex;
		test = 0.8f * trace_cone(voxCoord + n / 512.0, r, 0.05, 1.0, 0.01) + 0.2f * tex;
		//test = traversal(voxCoord + n / 512.0, r);
	}
	else
	{
		test = tex;
	}*/

	//return float4(ao, ao, ao, tex.w);
	//return float4(tex.xyz * ao, tex.w);
	return float4(test.xyz, tex.w);
	//return float4(tex.xyz * min(light + 0.2f, 1.0f), tex.w);
	//return float4(r, tex.w);
	//return float4(ao, ao, ao, 1.0f);
}