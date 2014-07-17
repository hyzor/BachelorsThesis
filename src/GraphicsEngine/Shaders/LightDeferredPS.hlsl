#include "LightDeferredVS.hlsl"
#include "LightDef.hlsli"
#include "Common.hlsli"

cbuffer cLightBuffer : register(b0)
{
	PLight gPLights[MAX_POINT_LIGHTS];
	DLight gDirLights[MAX_DIR_LIGHTS];
	SLight gSLights[MAX_SPOT_LIGHTS];

	int gPLightCount;
	int gDirLightCount;
	int gSLightCount;
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

float4 main(VertexOut pIn) : SV_TARGET
{
	float4 diffuse;
	float3 normal;
	float4 specular;
	float3 positionW;
	float shadowFactor;
	float diffuseMultiplier;
	int materialIndex;

	float4 litColor;
	
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
	litColor = diffuse;
	float4 ambient_Lights = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse_Lights = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular_Lights = float4(0.0f, 0.0f, 0.0f, 0.0f);

		if (gSkipLighting == 1)
			diffuse_Lights = float4(1.0f, 1.0f, 1.0f, 1.0f);

	if (gSkipLighting == 0)
	{
		float4 A, D, S;

		// Begin calculating lights
		for (int i = 0; i < gDirLightCount; ++i)
		{
			ComputeDLight(curMat, gDirLights[i], normal, toEye, A, D, S);
			ambient_Lights += A * shadowFactor;
			diffuse_Lights += D * shadowFactor;
			specular_Lights += S * shadowFactor;
		}

		for (int j = 0; j < gPLightCount; ++j)
		{
			ComputePLight(curMat, gPLights[j], positionW, normal, toEye, A, D, S);
			ambient_Lights += A;
			diffuse_Lights += D;
			specular_Lights += S;
		}

		for (int k = 0; k < gSLightCount; ++k)
		{
			ComputeSLight(curMat, gSLights[k], positionW, normal, toEye, A, D, S);
			ambient_Lights += A;
			diffuse_Lights += D;
			specular_Lights += S;
		}
	}

	litColor = float4(diffuse.xyz * (ambient_Lights.xyz + diffuse_Lights.xyz) + specular_Lights.xyz, 1.0f);

	return litColor;
}