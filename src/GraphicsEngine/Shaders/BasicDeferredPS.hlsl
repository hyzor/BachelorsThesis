#include "BasicDeferredVS.hlsl"
#include "LightDef.hlsli"

cbuffer cPerObject : register(b0)
{
	Material gMaterial;

	unsigned int gGlobalMaterialIndex;
	float3 padding;
}

Texture2D gDiffuseMap : register(t0);

SamplerState samLinear : register(s0);
SamplerState samAnisotropic : register(s1);
SamplerComparisonState samShadow : register(s2);

struct PixelOut
{
	float4 Color : SV_Target0;
	float4 Normal : SV_Target1;
};

PixelOut main(VertexOut pIn)
{
	PixelOut pOut;

	pOut.Color = float4(0.0f, 0.0f, 0.0f, 0.0f);
	pOut.Normal = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float shadowFactor = 1.0f;
	float shadowFactorBlend = 1.0f;

	pOut.Color = gMaterial.Diffuse;
	pOut.Normal = float4(pIn.NormalW, 0.0f);

	// Output material index
	pOut.Normal.w = shadowFactor;

	pOut.Color.w = (float)gGlobalMaterialIndex / 255.0f;
	
	// Gamma correct color (make it linear)
	pOut.Color.xyz = pow(pOut.Color.xyz, 2.2f);

	return pOut;
}
