#include "IDB_SHADER_COMMON_TYPES"

struct PixelInputType
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXTURE0;
};

// Uniforms
cbuffer PerObject : register(b0)
{
	PerObject PO;
}
cbuffer PerFrame : register(b1)
{
	PerFrame PF;
}

PixelInputType VS_Shadow(VSInputP IN)
{
	// Projection and View - from light
	const float4x4 mvp = mul(PF.Projection, mul(PF.View, PO.Model));

	PixelInputType PIT;
	PIT.position = mul(mvp, float4(IN.position.xyz, 1.0f));
	PIT.depthPosition = PIT.position;

	return PIT;
}

float4 PS_Shadow(PixelInputType VSOutput) : SV_TARGET
{
	float depthValue = (VSOutput.depthPosition.z / VSOutput.depthPosition.w);
	return float4(depthValue, 1.0f, 1.0f, 1.0f);
}