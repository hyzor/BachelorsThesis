#include "ParticleDef.hlsli"
#include "ParticleSystemDef.hlsli"
#include "ParticleSystemCS_Helper.hlsli"

cbuffer cbConstants : register(b0)
{
	float g_fSmoothlen;
	float g_fDensityCoef;
	float2 padding;

	float3 originPosW;
	float padding2;

	uint3 gridSize;
	float padding3;

	float3 cellSize;
	float padding4;
};

StructuredBuffer<Particle> Particles : register(t0);
RWStructuredBuffer<ParticleDensity> ParticlesDensity : register(u0);

StructuredBuffer<uint2> GridIndices : register(t1);

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

[numthreads(BLOCKSIZE, 1, 1)]
void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
	const unsigned int ID = DTid.x;
	const float h_sq = g_fSmoothlen * g_fSmoothlen;
	float3 position = Particles[ID].position;

	float density = 0.0f;

	// Calculate the density based on neighbors from the 8 adjacent cells
	// and current cell
	int3 gridPos = CalcGridPos(position, originPosW, cellSize);
	uint gridHash = CalcGridHash(gridPos, gridSize);

	// Iterate through every neighboring cell (including the current cell)
	for (int z = -1; z <= 1; ++z)
	{
		for (int y = -1; y <= 1; ++y)
		{
			for (int x = -1; x <= 1; ++x)
			{
				int3 neighborGridPos = gridPos + int3(x, y, z);
				uint neighborGridHash = CalcGridHash(
				neighborGridPos,
				gridSize);

				uint2 start_end = GridIndices[neighborGridHash];

				for (unsigned int i = start_end.x; i < start_end.y; ++i)
				{
					float3 neighborPos = Particles[i].position;

					float3 diff = neighborPos - position;
					float r_sq = dot(diff, diff);

					if (r_sq < h_sq)
					{
							density += CalculateDensity(r_sq);
					}
				}
			}
		}
	}

	// Update density with the calculated one
	ParticlesDensity[ID].density = density;
}