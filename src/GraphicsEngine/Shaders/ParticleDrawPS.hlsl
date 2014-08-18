//#include "ParticleDrawVS.hlsl"
#include "ParticleDrawGS.hlsl"

Texture2D diffuseMap : register(t0);

SamplerState linearSample : register(s0);

struct PixelOut
{
	float4 Color : SV_Target0;
	float4 Normal: SV_Target1;
};

PixelOut main(GeometryOut pIn)
{
	PixelOut pOut;

	pOut.Color = diffuseMap.Sample(linearSample, pIn.Tex);

	clip(pOut.Color.w < 0.9f ? -1 : 1);

	float3 normal = float3(0.0f, 0.0f, 0.0f);
	normal.xy = pIn.Tex * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);
	float mag = dot(normal.xy, normal.xy);

	if (mag > 1.0f)
		discard;

	normal.z = sqrt(1.0f - mag);

	//pOut.Normal.xyz = float3(0.0f, 1.0f, -1.0f);
	//pOut.Normal.w = 1.0f;
	//pOut.Normal.xyz = 1.0f;
	//pOut.Normal.w = 1.0f;
	pOut.Normal.xyz = normal;
	pOut.Normal.w = 1.0f;

	return pOut;
}