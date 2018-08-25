inline float DiffuseLambert(in float3 lightDir, in float3 normals)
{
	return max(dot(lightDir, normals), 0.0f);
}

inline float DiffuseOrenNayar(in float3 lightDir, in float3 viewDir, in float3 normals, in float roughness)
{
	float ndotv = saturate(dot(normals, viewDir));
	float ndotl = saturate(dot(normals, lightDir));

	float A = 1.0f - 0.5f * roughness / (roughness + 0.33f);
	float B = 0.45f * roughness / (roughness + 0.09f);
	
	float angleVN = acos(ndotv);
	float angleLN = acos(ndotl);
	
	float alpha = max(angleVN, angleLN);
	float beta = min(angleVN, angleLN);
	
	float gamma = dot(-viewDir - (normals * ndotv), lightDir - (normals * ndotl));

	return max(0.0, ndotl) * (A + B * max(0.0, gamma) * sin(alpha) * tan(beta));
}

inline float VisibilityTerm(float roughness, float ndotv, float ndotl)
{
	float alpha = roughness * roughness;
	float visV = ndotl * sqrt(ndotv * (ndotv - ndotv * alpha) + alpha);
	float visL = ndotv * sqrt(ndotl * (ndotl - ndotl * alpha) + alpha);
	return 0.5f / max(visV + visL, 0.00001f);
}

inline float DistributionTerm(float roughness, float ndoth)
{
	float alpha = roughness * roughness;
	float d = (ndoth * alpha - ndoth) * ndoth + 1.0f;
	return alpha / (d * d * 3.141592f);
}

inline float FresnelTerm(float vdoth)
{
	return pow(abs(1.0f - vdoth), 5.0f);
}

inline float3 SpecularGGX(float3 lightDir, float3 viewDir, float3 halfDir, float3 normals, float3 albedo, float metallic, float ao, float roughnessE, float roughnessL)
{
	float vdoth = saturate(dot(viewDir, halfDir));
	float ndoth = saturate(dot(normals, halfDir));
	float ndotv = saturate(dot(normals, viewDir));
	float ndotl = saturate(dot(normals, lightDir));

	float3 f0 = float3(0.04, 0.04, 0.04);
	f0 = lerp(f0, albedo, metallic);

	float ff = FresnelTerm(vdoth);

	float3 lightF = f0 * (1.0f - ff) + (float3(1.0f, 1.0f, 1.0f) - f0) * ff;
	float lightD = DistributionTerm(roughnessL, ndoth);
	float lightV = VisibilityTerm(roughnessL, ndotv, ndotl);
	float3 specularLight = lightF * (lightD * lightV * 3.141592f * ndotl);
	specularLight *= clamp(pow(abs(ndotv + ao), roughnessE), 0.0f, 1.0f);

	return specularLight;
}