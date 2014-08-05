#include "ParticleDef.hlsli"
#include "ParticleSystemDef.hlsli"
#include "ParticleSystemCS_Helper.hlsli"

cbuffer cbConstants : register(b0)
{
	float g_fSmoothlen;
	float g_fDensityCoef;
	//float2 padding;

	float3 originPosW;
	uint3 gridSize;
	float3 cellSize;
	float padding;
};

//--------------------------------------------------------------------------------------
// Density Calculation
// Author: Microsoft Corporation
//--------------------------------------------------------------------------------------
float CalculateDensity(float r_sq)
{
	const float h_sq = g_fSmoothlen * g_fSmoothlen;
	// Implements this equation:
	// W_poly6(r, h) = 315 / (64 * pi * h^9) * (h^2 - r^2)^3
	// g_fDensityCoef = fParticleMass * 315.0f / (64.0f * PI * fSmoothlen^9)
	return g_fDensityCoef * (h_sq - r_sq) * (h_sq - r_sq) * (h_sq - r_sq);
}

StructuredBuffer<Particle> Particles : register(t0);
RWStructuredBuffer<ParticleDensity> ParticlesDensity : register(u0);

StructuredBuffer<uint2> GridIndices : register(t1);

[numthreads(BLOCKSIZE, 1, 1)]
void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
	const unsigned int ID = DTid.x;
	const float h_sq = g_fSmoothlen * g_fSmoothlen;
	float3 position = Particles[ID].position;

	float density = 0;

	// Calculate the density based on neighbors from the 8 adjacent cells and current cell
	int3 gridPos = CalcGridPos(position, originPosW, cellSize);
	uint gridHash = CalcGridHash(gridPos, gridSize);

	// Update density with the calculated one
	ParticlesDensity[ID].density = density;
}