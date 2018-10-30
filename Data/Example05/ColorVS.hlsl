struct Input
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

Input VS(float3 position : POSITION, float4 color : COLOR)
{
	Input result;

	result.position = float4(position, 1.0f);
	result.color = color;

	return result;
}