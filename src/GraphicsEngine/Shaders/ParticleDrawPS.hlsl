//#include "ParticleDrawVS.hlsl"
#include "ParticleDrawGS.hlsl"

Texture2D diffuseMap : register(t0);

SamplerState linearSample : register(s0);

struct PixelOut
{
	float4 Color : SV_Target0;
};

PixelOut main(GeometryOut pIn)
{
	PixelOut pOut;

	pOut.Color = diffuseMap.Sample(linearSample, pIn.Tex);

	clip(pOut.Color.w < 0.9f ? -1 : 1);

	return pOut;
}