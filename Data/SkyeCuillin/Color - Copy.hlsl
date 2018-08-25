cbuffer inputBuffer : register(b0)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4 offset;
	float4 scale;
	float4 cameraPos;
	float4 pad[5];
};

Texture2D diffuse : register(t0);
Texture3D voxel : register(t1);
SamplerState diffuseSampler : register(s0);
//SamplerState voxelSampler : register(s1);

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
}

float simple_noise(in float2 co)
{
	return frac(sin(dot(co.xy, float2(12.9898f, 78.233f))) * 43758.5453f);
}

float4 voxel_fetch(in float3 sample_pos, in float3 vV, in float mip)
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

float4 PS(Input input) : SV_TARGET
{
	float3 n = normalize(input.normal);
	float light = max(dot(n, normalize(float3(-0.2f, 1.0f, 0.3f))), 0.0f);

	float4 tex = diffuse.Sample(diffuseSampler, input.texCoord);
	//float3 voxCoord = input.voxelCoord.xyz * 0.5 + 0.5;
	//float4 vox = voxel.SampleLevel(voxelSampler, float3(voxCoord.x, voxCoord.y, voxCoord.z), 0.0f);

	//float3 r = normalize(reflect(normalize(input.worldPos), n));
	//float4 test = TraceTest(voxCoord + n / 512.0, r);
	//float4 test = trace_cone(voxCoord + n / 512.0, r, 1.0, 1.0, 0.0);
	//float ao = trace_shadow_cone(voxCoord + n / 512.0, n, 1.0, 0.2, 1.0);

	return float4(tex.xyz * min(light + 0.2f, 1.0f), tex.w);
	//return float4(ao, ao, ao, 1.0f);
}