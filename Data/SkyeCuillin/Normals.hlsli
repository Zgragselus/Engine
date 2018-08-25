inline float2 EncodeSpheremap(float3 normal)
{
	//float f = sqrt(8.0f * normal.z + 8.0f);
	float f = pow(8.0f * normal.z + 8.0f, 0.5f);
	return normal.xy / f + 0.5f;
	
	/*float scale = 1.7777;
	float2 enc = normal.xy / (normal.z + 1);
	enc /= scale;
	enc = enc * 0.5 + 0.5;
	return enc;*/

	//return normal.xy;

	/*float2 spherical;

	spherical.x = atan2(normal.y, normal.x) / 3.14159f;
	spherical.y = normal.z;

	return spherical * 0.5f + 0.5f;*/
}

inline float3 DecodeSpheremap(float2 encoded)
{
	float2 fenc = encoded * 4.0f - 2.0f;
	float f = dot(fenc, fenc);
	//float g = sqrt(1.0f - f / 4.0f);
	float g = pow(1.0f - f / 4.0f, 0.5f);
	float3 n;
	n.xy = fenc * g;
	n.z = 1.0f - f / 2.0f;
	return n;

	/*float scale = 1.7777;
	float3 nn =
		float3(encoded.xy, 0.0) * float3(2 * scale, 2 * scale, 0) +
		float3(-scale, -scale, 1);
	float g = 2.0 / dot(nn.xyz, nn.xyz);
	float3 n;
	n.xy = g * nn.xy;
	n.z = g - 1;
	return n;*/

	/*float3 n = float3(encoded.x, encoded.y, 0.0f);
	n.z = 1.0 - sqrt(n.x * n.x + n.y * n.y);
	return normalize(n);*/

	/*float2 sinCosTheta, sinCosPhi;

	encoded = encoded * 2.0f - 1.0f;
	sincos(encoded.x * 3.14159f, sinCosTheta.x, sinCosTheta.y);
	sinCosPhi = float2(sqrt(1.0 - encoded.y * encoded.y), encoded.y);

	return float3(sinCosTheta.y * sinCosPhi.x, sinCosTheta.x * sinCosPhi.x, sinCosPhi.y);*/
}