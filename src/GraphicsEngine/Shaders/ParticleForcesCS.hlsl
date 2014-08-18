#include "ParticleDef.hlsli"
#include "ParticleSystemDef.hlsli"
#include "ParticleSystemCS_Helper.hlsli"

cbuffer cbConstants : register(b0)
{
	// SPH method parameters
	float g_fPressureStiffness;
	float g_fRestDensity;
	float g_fSmoothlen;
	float g_fGradPressureCoef;
	float g_fLapViscosityCoef;

	float3 originPosW;

	uint3 gridSize;
	float g_fSphereRadius;

	float3 cellSize;
	float padding3;

	// DEM method parameters
	float g_fParamsSpring;
	float g_fParamsDamping;
	float g_fParamsShear;
	float g_fParamsAttraction;
};

StructuredBuffer<Particle> Particles : register(t0);
StructuredBuffer<ParticleDensity> ParticlesDensity : register(t1);

RWStructuredBuffer<ParticleForces> ParticlesForces : register(u0);

StructuredBuffer<uint2> GridIndices : register(t2);

[numthreads(BLOCKSIZE, 1, 1)]
void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
	const unsigned int ID = DTid.x;

	float3 position = Particles[ID].position;
	float3 velocity = Particles[ID].velocity;

	int3 gridPos = CalcGridPos(position, originPosW, cellSize);

	float3 acceleration = float3(0.0f, 0.0f, 0.0f);

	for (int z = -1; z <= 1; z++)
	{
		for (int y = -1; y <= 1; y++)
		{
			for (int x = -1; x <= 1; x++)
			{
				int3 neighbourGridPos = gridPos + int3(x, y, z);
				uint neighborGridHash = CalcGridHash(
				neighbourGridPos,
				gridSize);

				uint2 start_end = GridIndices[neighborGridHash];

				for (unsigned int i = start_end.x; i < start_end.y; ++i)
				{
					if (i != ID)
					{
						Particle curParticle = Particles[i];
						float3 relPos = curParticle.position - position;
						float dist = length(relPos);
						float collideDist = g_fSphereRadius 
							+ g_fSphereRadius;

						float3 force = float3(0.0f, 0.0f, 0.0f);

						if (dist < collideDist)
						{
							float3 norm = relPos / dist;

							// Relative velocity
							float3 relVel = curParticle.velocity
								- velocity;
							// Relative tangential velocity
							float3 tanVel = relVel 
								- (dot(relVel, norm) * norm);
							// Spring force
							force = -g_fParamsSpring 
								* (collideDist - dist) * norm;
							// Dashpot (damping) force
							force += g_fParamsDamping * relVel;
							// Tangential shear force
							force += g_fParamsShear * tanVel;
							// Attraction
							force += g_fParamsAttraction * relPos;

							acceleration += force;
						}
					}
				}
			}
		}
	}

	ParticlesForces[ID].acceleration = acceleration;
}

//--------------------------------------------------------------------------------------
// Force Calculations
// Author: Microsoft Corporation
//--------------------------------------------------------------------------------------
float CalculatePressure(float density)
{
	// Implements this equation:
	// Pressure = B * ((rho / rho_0)^y  - 1)
	return g_fPressureStiffness * max(pow(density / g_fRestDensity, 3) - 1, 0);
}

float3 CalculateGradPressure(float r, float P_pressure, float N_pressure,
	float N_density, float3 diff)
{
	const float h = g_fSmoothlen;
	float avg_pressure = 0.5f * (N_pressure + P_pressure);
	// Implements this equation:
	// W_spkiey(r, h) = 15 / (pi * h^6) * (h - r)^3
	// GRAD( W_spikey(r, h) ) = -45 / (pi * h^6) * (h - r)^2
	// g_fGradPressureCoef = fParticleMass * -45.0f / (PI * fSmoothlen^6)
	return g_fGradPressureCoef * avg_pressure / N_density * (h - r) * (h - r) / r
		* (diff);
}

float3 CalculateLapVelocity(float r, float3 P_velocity, float3 N_velocity,
	float N_density)
{
	const float h = g_fSmoothlen;
	float3 vel_diff = (N_velocity - P_velocity);
		// Implements this equation:
		// W_viscosity(r, h) = 15 / (2 * pi * h^3) * (-r^3 / (2 * h^3) + r^2 / h^2 +
		// h / (2 * r) - 1)
		// LAPLACIAN( W_viscosity(r, h) ) = 45 / (pi * h^6) * (h - r)
		// g_fLapViscosityCoef = fParticleMass * fViscosity * 45.0f / 
		// (PI * fSmoothlen^6)
		return g_fLapViscosityCoef / N_density * (h - r) * vel_diff;
}

[numthreads(BLOCKSIZE, 1, 1)]
void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
	const unsigned int ID = DTid.x;

	// Get current particle properties
	float3 position = Particles[ID].position;
	float3 velocity = Particles[ID].velocity;
	float density = ParticlesDensity[ID].density;
	float pressure = CalculatePressure(density);

	const float h_sq = g_fSmoothlen * g_fSmoothlen;

	float3 acceleration = float3(0.0f, 0.0f, 0.0f);

	// Calculate acceleration based on neighbors from the 8 adjacent cells
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

					if (r_sq < h_sq && ID != i)
					{
						float3 neighborVel = Particles[i].velocity;
						float neighborDensity = 
							ParticlesDensity[i].density;
						float neighborPressure = 
							CalculatePressure(neighborDensity);
						float r = sqrt(r_sq);

						// Pressure term
						acceleration += CalculateGradPressure(
							r,
							pressure,
							neighborPressure,
							neighborDensity,
							diff);

						acceleration += CalculateLapVelocity(
							r,
							velocity,
							neighborVel,
							neighborDensity);
					}
				}
			}
		}
	}

	// Update forces with the calculated ones
	ParticlesForces[ID].acceleration = acceleration / density;
}