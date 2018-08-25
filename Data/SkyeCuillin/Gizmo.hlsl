cbuffer cameraBuffer : register(b0)
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
	float4 pad[8];
};

struct Input
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

Input VS(uint id : SV_VertexID,
	float4 position : POSITION,
	float4 color : COLOR)
{
	Input result;
	
	result.position = mul(projectionMatrix, mul(viewMatrix, position));
	result.color = color;

	return result;
}

float4 PS(Input input) : SV_TARGET
{
	return input.color;
}