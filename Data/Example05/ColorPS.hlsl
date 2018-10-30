struct Input
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

float4 PS(Input input) : SV_TARGET
{
	return input.color;
}