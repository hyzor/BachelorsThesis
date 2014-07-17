#include "ParticleDrawVS.hlsl"

Texture2D diffuseMap : register(t0);

SamplerState linearSample : register(s0);

struct PixelOut
{
	float4 Color : SV_Target0;
};

PixelOut main(VertexOut pIn)
{
	PixelOut pOut;

	pOut.Color = float4(1.0f, 1.0f, 1.0f, 1.0f);

	return pOut;
}