#include "LightDeferredVS.hlsl"
#include "LightDef.hlsli"
#include "Common.hlsli"

cbuffer cLightBuffer : register(b0)
{
	PLight gPointLights[MAX_POINT_LIGHTS];
	DLight gDirLights[MAX_DIR_LIGHTS];
	SLight gSpotLights[MAX_SPOT_LIGHTS];

	int gPointLightCount;
	int gDirLightCount;
	int gSpotLightCount;
	int gSkipProcessing;

	float3 gEyePosW;
	int gSkipLighting;

	float4x4 gCamViewProjInv;

	Material gMaterials[MAX_MATERIALS];
};

Texture2D gDiffuseTexture : register(t0);
Texture2D gNormalTexture : register(t1);
Texture2D gDepthTexture : register(t2);

SamplerState samLinear : register(s0);
SamplerState samAnisotropic : register(s1);
SamplerComparisonState samShadow : register(s2);
SamplerState samPoint : register(s3);

struct PixelOut
{
	float4 LitColor : SV_Target0;
};

PixelOut main(VertexOut pIn)
{
	PixelOut pOut;

	float4 diffuse;
	float3 normal;
	float4 specular;
	float3 positionW;
	float shadowFactor;
	int materialIndex;

	diffuse.xyz = gDiffuseTexture.Sample(samLinear, pIn.Tex).xyz;
	normal = gNormalTexture.Sample(samLinear, pIn.Tex).xyz;
	shadowFactor = gNormalTexture.Sample(samLinear, pIn.Tex).w;
	float matFloat = gDiffuseTexture.Sample(samPoint, pIn.Tex).w * 255.0f;
	materialIndex = round(matFloat);

	Material curMat;

	// Set this to a default material
	if (materialIndex < 0 || materialIndex > MAX_MATERIALS - 1)
	{
		curMat.Ambient = float4(0.1f, 0.1f, 0.1f, 0.1f);
		curMat.Diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
		curMat.Specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
		curMat.Reflect = float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	else
	{
		curMat = gMaterials[materialIndex];
	}

	diffuse.w = 1.0f;

	// World pos reconstruction
	float depth = gDepthTexture.Sample(samLinear, pIn.Tex).x;

	float4 H = float4(pIn.Tex.x * 2.0f - 1.0f, (1.0f - pIn.Tex.y) * 2.0f - 1.0f, depth, 1.0f);

	float4 D_transformed = mul(H, gCamViewProjInv);

	positionW = (D_transformed / D_transformed.w).xyz;

	// The toEye vector is used in lighting
	float3 toEye = gEyePosW - positionW;

	// Direction of eye to pixel world position
	float3 eyeDir = positionW - gEyePosW;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye);

	// Normalize
	toEye /= distToEye;

	//--------------------------------------------------
	// Lighting
	//--------------------------------------------------
	//float4 litColor = diffuse;
	pOut.LitColor = diffuse;

	float4 ambient_Lights = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse_Lights = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 specular_Lights = float4(0.0f, 0.0f, 0.0f, 0.0f);

	if (gSkipLighting == 1)
		diffuse_Lights = float4(1.0f, 1.0f, 1.0f, 1.0f);

	float4 A, D, S;

	if (gSkipLighting == 0)
	{
		// Begin calculating lights
		for (int i = 0; i < gDirLightCount; ++i)
		{
			//ComputeDLight_Deferred_Ambient(specular, gDirLights[i], normal, toEye, A, D, S);
			ComputeDLight(curMat, gDirLights[i], normal, toEye, A, D, S);
			ambient_Lights += A * shadowFactor;
			diffuse_Lights += D * shadowFactor;
			specular_Lights += S * shadowFactor;
		}

		for (int j = 0; j < gPointLightCount; ++j)
		{
			//ComputePLight_Deferred_Ambient(specular, gPointLights[j], positionW, normal, toEye, A, D, S);
			ComputePLight(curMat, gPointLights[j], positionW, normal, toEye, A, D, S);
			ambient_Lights += A;
			diffuse_Lights += D;
			specular_Lights += S;
		}

		for (int k = 0; k < gSpotLightCount; ++k)
		{
			//ComputeSLight_Deferred(specular, gSpotLights[k], positionW, normal, toEye, A, D, S);
			ComputeSLight(curMat, gSpotLights[k], positionW, normal, toEye, A, D, S);
			ambient_Lights += A;
			diffuse_Lights += D;
			specular_Lights += S;
		}
	}

	pOut.LitColor = float4(diffuse.xyz * (ambient_Lights.xyz + diffuse_Lights.xyz) + specular_Lights.xyz, 1.0f);

	// Tone mapping
	pOut.LitColor.xyz = Uncharted2Tonemap(pOut.LitColor.xyz);

	// Gamma encode final lit color
	pOut.LitColor.xyz = pow(pOut.LitColor.xyz, 1.0f / 2.2f);

	return pOut;
}