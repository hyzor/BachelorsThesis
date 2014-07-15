#include "SkyDeferredVS.hlsl"

// Cube map
TextureCube gCubeMap : register(t0);

SamplerState samTriLinearSam : register(s0);

struct PixelOut
{
	float4 Color : SV_Target0;
	float4 Normal : SV_Target1;
};

PixelOut main(VertexOut pIn)// : SV_TARGET
{
	PixelOut pOut;

	pOut.Color = gCubeMap.Sample(samTriLinearSam, pIn.PosL);

	// Default material
	pOut.Color.w = 1.0f;

	// No shadow
	pOut.Normal.xyz = 1.0f;

	pOut.Normal.w = 1.0f;

	// Gamma correct color (make it linear)
	pOut.Color.xyz = pow(pOut.Color.xyz, 2.2f);

	return pOut;
}