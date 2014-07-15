//-------------------------------------------------------------------------
// Effect for calculating lights
//-------------------------------------------------------------------------

//If you are changing this, change the MAXPLIGHTS, MAXDLIGHTS and MAXSLIGHTS in GraphichsEngingeImpl.h
#define MAX_POINT_LIGHTS 16
#define MAX_DIR_LIGHTS 2
#define MAX_SPOT_LIGHTS 32

#define MAX_MATERIALS 64

// Suffixes: L (Local), W (World), V (View), H (Homogeneous)

// Keep mirrored with C++ implementation (LightDef.h)
struct PLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Attenuation;
	float Padding;
};

struct DLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Direction;
	float Padding;
};

struct SLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Direction;
	float Spot;

	float3 Attenuation;
	float Padding;
};

struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float4 Reflect;
};

//=============================================================================
// Point light
//=============================================================================
void ComputePLight(
	Material mat,		// Material
	PLight light,	// Point light source
	float3 pos,			// Surface position
	float3 normal,		// Surface normal
	float3 toEye,		// Surface point being lit to the eye

	out float4 ambient,
	out float4 diffuse,
	out float4 specular)
{
	// Initialize outputs
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Define light vector (Direction from surface point to point light source)
	float3 lightVec = light.Position - pos;

		// Calculate distance from surface to light
		float dist = length(lightVec);

	// Test if out of range
	if (dist > light.Range)
		return;

	// Normalize light vector
	lightVec /= dist;

	// Ambient
	ambient = mat.Ambient * light.Ambient;

	// Begin calculating diffuse and specular
	float diffuseFactor = dot(lightVec, normal);

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
			float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

		diffuse = diffuseFactor * mat.Diffuse * light.Diffuse;
		specular = specFactor * mat.Specular * light.Specular;
	}

	// Attenuate
	float atten = 1.0f / dot(light.Attenuation, float3(1.0f, dist, dist*dist));

	diffuse *= atten;
	specular *= atten;
}

void ComputePLight_Deferred(
	float4 specular,		// Material (specular)
	PLight light,	// Point light source
	float3 pos,			// Surface position
	float3 normal,		// Surface normal
	float3 toEye,		// Surface point being lit to the eye

	out float4 diffuseOut,
	out float4 specularOut)
{
	// Initialize outputs
	//ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuseOut = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specularOut = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Define light vector (Direction from surface point to point light source)
	float3 lightVec = light.Position - pos;

	// Calculate distance from surface to light
	float dist = length(lightVec);

	// Test if out of range
	if (dist > light.Range)
		return;

	// Normalize light vector
	lightVec /= dist;

	// Ambient
	//ambient = mat.Ambient * light.Ambient;

	// Begin calculating diffuse and specular
	float diffuseFactor = dot(lightVec, normal);

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), specular.w);

		diffuseOut = diffuseFactor * light.Diffuse;
		specularOut = specFactor * specular * light.Specular;
	}

	// Attenuate
	float atten = 1.0f / dot(light.Attenuation, float3(1.0f, dist, dist*dist));

	diffuseOut *= atten;
	specularOut *= atten;
}

void ComputePLight_Deferred_Ambient(
	float4 specular,		// Material (specular)
	PLight light,	// Point light source
	float3 pos,			// Surface position
	float3 normal,		// Surface normal
	float3 toEye,		// Surface point being lit to the eye

	out float4 ambientOut,
	out float4 diffuseOut,
	out float4 specularOut)
{
	// Initialize outputs
	ambientOut = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuseOut = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specularOut = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Define light vector (Direction from surface point to point light source)
	float3 lightVec = light.Position - pos;

		// Calculate distance from surface to light
		float dist = length(lightVec);

	// Test if out of range
	if (dist > light.Range)
		return;

	// Normalize light vector
	lightVec /= dist;

	// Ambient
	//ambient = mat.Ambient * light.Ambient;

	// Begin calculating diffuse and specular
	float diffuseFactor = dot(lightVec, normal);

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
			float specFactor = pow(max(dot(v, toEye), 0.0f), specular.w);

		ambientOut = light.Ambient;
		diffuseOut = diffuseFactor * light.Diffuse;
		specularOut = specFactor * specular * light.Specular;
	}

	// Attenuate
	float atten = 1.0f / dot(light.Attenuation, float3(1.0f, dist, dist*dist));

	ambientOut *= atten;
	diffuseOut *= atten;
	specularOut *= atten;
}

/*
void ComputePLight_Deferred_Material(
	Material mat,		// Material
	PLight light,	// Point light source
	float3 pos,			// Surface position
	float3 normal,		// Surface normal
	float3 toEye,		// Surface point being lit to the eye

	out float4 ambientOut,
	out float4 diffuseOut,
	out float4 specularOut)
{
	// Initialize outputs
	ambientOut = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuseOut = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specularOut = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Define light vector (Direction from surface point to point light source)
	float3 lightVec = light.Position - pos;

	// Calculate distance from surface to light
	float dist = length(lightVec);

	// Test if out of range
	if (dist > light.Range)
		return;

	// Normalize light vector
	lightVec /= dist;

	// Ambient
	ambientOut = mat.Ambient * light.Ambient;

	// Begin calculating diffuse and specular
	float diffuseFactor = dot(lightVec, normal);

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), specular.w);

		//ambientOut = light.Ambient;

		diffuseOut = diffuseFactor * mat.Diffuse * light.Diffuse;
		specularOut = specFactor * mat.Specular * light.Specular;
	}

	// Attenuate
	float atten = 1.0f / dot(light.Attenuation, float3(1.0f, dist, dist*dist));

	//ambientOut *= atten;
	diffuseOut *= atten;
	specularOut *= atten;
}
*/

//=============================================================================
// Directional light
//=============================================================================
void ComputeDLight(
	Material mat,
	DLight light,
	float3 normal,
	float3 toEye,

	out float4 ambient,
	out float4 diffuse,
	out float4 specular)
{
	// Initialize outputs
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Light vector aims opposite the direction the light rays travel
	float3 lightVec = -light.Direction;

	ambient = mat.Ambient * light.Ambient;

	float diffuseFactor = dot(lightVec, normal);

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

		diffuse = diffuseFactor * mat.Diffuse * light.Diffuse;
		specular = specFactor * mat.Specular * light.Specular;
	}
}

void ComputeDLight_Deferred(
	float4 specular,
	DLight light,
	float3 normal,
	float3 toEye,

	out float4 diffuseOut,
	out float4 specularOut)
{
	// Initialize outputs
	//ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuseOut = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specularOut = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Light vector aims opposite the direction the light rays travel
	float3 lightVec = -light.Direction;

	//ambient = mat.Ambient * light.Ambient;

	float diffuseFactor = dot(lightVec, normal);

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
		float specFactor = pow(max(dot(v, toEye), 0.0f), specular.w);

		diffuseOut = diffuseFactor * light.Diffuse;
		specularOut = specFactor * specular * light.Specular;
	}
}

void ComputeDLight_Deferred_Ambient(
	float4 specular,
	DLight light,
	float3 normal,
	float3 toEye,

	out float4 ambientOut,
	out float4 diffuseOut,
	out float4 specularOut)
{
	// Initialize outputs
	ambientOut = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuseOut = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specularOut = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Light vector aims opposite the direction the light rays travel
	float3 lightVec = -light.Direction;

		//ambient = mat.Ambient * light.Ambient;

		float diffuseFactor = dot(lightVec, normal);

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
			float specFactor = pow(max(dot(v, toEye), 0.0f), specular.w);

		//ambientOut = ambient;
		ambientOut = light.Ambient;
		diffuseOut = diffuseFactor * light.Diffuse;
		specularOut = specFactor * specular * light.Specular;
	}
}

//=============================================================================
// Spot light
//=============================================================================
void ComputeSLight(
	Material mat,
	SLight light,
	float3 pos,
	float3 normal,
	float3 toEye,

	out float4 ambient,
	out float4 diffuse,
	out float4 specular)
{
	// Initialize outputs.
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Vector from surface to light
	float3 lightVec = light.Position - pos;

		// Distance from surface to light
		float dist = length(lightVec);

	// If distance is further than lights range, return
	if (dist > light.Range)
		return;

	// Normalize light vector
	lightVec /= dist;

	// Ambient
	ambient = mat.Ambient * light.Ambient;

	// Add diffuse and specular
	float diffuseFactor = dot(lightVec, normal);

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
			float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);

		diffuse = diffuseFactor * mat.Diffuse * light.Diffuse;
		specular = specFactor * mat.Specular * light.Specular;
	}

	// Scale by SLight factor
	float spot = pow(max(dot(-lightVec, light.Direction), 0.0f), light.Spot);

	// Scale by attenuation factor
	float attenuation = spot / dot(light.Attenuation, float3(1.0f, dist, dist*dist));

	ambient *= spot;
	diffuse *= attenuation;
	specular *= attenuation;
}

void ComputeSLight_Deferred(
	float4 specular,
	SLight light,
	float3 pos,
	float3 normal,
	float3 toEye,

	out float4 ambientOut,
	out float4 diffuseOut,
	out float4 specularOut)
{
	// Initialize outputs.
	ambientOut = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuseOut = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specularOut = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Vector from surface to light
	float3 lightVec = light.Position - pos;

		// Distance from surface to light
		float dist = length(lightVec);

	// If distance is further than lights range, return
	if (dist > light.Range)
		return;

	// Normalize light vector
	lightVec /= dist;

	// Ambient
	ambientOut = light.Ambient;

	// Add diffuse and specular
	float diffuseFactor = dot(lightVec, normal);

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		float3 v = reflect(-lightVec, normal);
			float specFactor = pow(max(dot(v, toEye), 0.0f), specular.w);

		diffuseOut = diffuseFactor * light.Diffuse;
		specularOut = specFactor * specular * light.Specular;
	}

	// Scale by SLight factor
	float spot = pow(max(dot(-lightVec, light.Direction), 0.0f), light.Spot);

	// Scale by attenuation factor
	float attenuation = spot / dot(light.Attenuation, float3(1.0f, dist, dist*dist));

	ambientOut *= spot;
	diffuseOut *= attenuation;
	specularOut *= attenuation;
}

//====================================================================================
// Function which determines if a pixel is in a shadow.
// Essentially the factor is a scalar in the range 0 to 1.
// Though, with PCF, the factor is between 0 and 1 (the point is partially in shadow).
//====================================================================================
static const float SMAP_SIZE = 2048.0f; // KEEP IN MIND!
static const float SMAP_DX = 1.0f / SMAP_SIZE;

float CalcShadowFactor(SamplerComparisonState samShadow,
	Texture2D shadowMap,
	float4 shadowPosH)
{
	// Complete projection by dividing with w
	shadowPosH.xyz /= shadowPosH.w;

	// Transform from NDC space to texture space.
	// (NOT NEEDED IF USING SHADOWTRANSFORM MATRIX)
	/*
	shadowPosH.x = +0.5f*shadowPosH.x + 0.5f;
	shadowPosH.y = -0.5f*shadowPosH.y + 0.5f;
	*/

	// Depth in NDC space
	float depth = shadowPosH.z;

	// Texel size
	const float dx = SMAP_DX;

	float percentLit = 0.0f;

	const float2 offsets[9] =
	{
		float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
	};

	// Each SampleCmpLevelZero does a 4-tap PCF.
	// We call SampleCmpLevelZero in a 3x3 box filter pattern.
	[unroll]
	for (int i = 0; i < 9; ++i)
	{
		percentLit += shadowMap.SampleCmpLevelZero(samShadow,
			shadowPosH.xy + offsets[i], depth).r;
	}

	// Average the samples
	return percentLit /= 9.0f;
}

//===================================================================================
// Transforms a normal sample to world space
//===================================================================================
float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float4 tangentW)
{
	// Uncompress each component from [0,1] to [-1,1]
	float3 normalT = 2.0f * normalMapSample - 1.0f;

		// Build orthonormal basis
		float3 N = unitNormalW;
		float3 T = normalize(tangentW.xyz - dot(tangentW.xyz, N) * N);
		float3 B = tangentW.w * cross(N, T);

		float3x3 TBN = float3x3(T, B, N);

		// Transform from tangent space to world space
		float3 bumpedNormalW = mul(normalT, TBN);

		return bumpedNormalW;
}