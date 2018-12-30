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

inline float DistributionGGX(float3 N, float3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = max(a * a, 0.0001f);
	float NdotH = max(dot(N, H), 0.0f);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
	denom = max(3.141593f * denom * denom, 0.0001f);

	return num / denom;
}

inline float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0f);
	float k = (r*r) / 8.0f;

	float num = NdotV;
	float denom = NdotV * (1.0f - k) + k;

	return num / denom;
}

inline float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0f);
	float NdotL = max(dot(N, L), 0.0f);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

inline float3 fresnelSchlick(float cosTheta, float3 F0)
{
	return F0 + (1.0f - F0) * pow(1.0f - cosTheta, 5.0f);
}

inline float3 fresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
	return F0 + (max(float3(1.0f - roughness, 1.0f - roughness, 1.0f - roughness), F0) - F0) * pow(1.0f - cosTheta, 5.0f);
}

inline float3 BRDF(float3 N, float3 V, float3 L, float3 H, float3 albedo, float metallic, float roughness)
{
	float3 F0 = float3(0.04f, 0.04f, 0.04f);
	F0 = lerp(F0, albedo, metallic);

	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	float3 F = fresnelSchlick(max(dot(H, V), 0.0f), F0);

	float3 kS = F;
	float3 kD = float3(1.0f, 1.0f, 1.0f) - kS;
	kD *= 1.0f - metallic;

	float3 numerator = NDF * G * F;
	float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f);
	float3 specular = numerator / max(denominator, 0.0001f);

	float NdotL = max(dot(N, L), 0.0f);

	return (kD * albedo / 3.141593f + specular) * NdotL;
}