#include "ParticleDef.hlsli"
#include "ParticleSystemDef.hlsli"
#include "ParticleSystemCS_Helper.hlsli"

cbuffer cbConstants : register(b0)
{
	float g_fPressureStiffness;
	float g_fRestDensity;
	float g_fSmoothlen;
	float g_fGradPressureCoef;
	float g_fLapViscosityCoef;
	
	float3 originPosW;
	uint3 gridSize;
	float3 cellSize;
	float2 padding;
};

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

float2 CalculateGradPressure(float r, float P_pressure, float N_pressure, float N_density, float2 diff)
{
	const float h = g_fSmoothlen;
	float avg_pressure = 0.5f * (N_pressure + P_pressure);
	// Implements this equation:
	// W_spkiey(r, h) = 15 / (pi * h^6) * (h - r)^3
	// GRAD( W_spikey(r, h) ) = -45 / (pi * h^6) * (h - r)^2
	// g_fGradPressureCoef = fParticleMass * -45.0f / (PI * fSmoothlen^6)
	return g_fGradPressureCoef * avg_pressure / N_density * (h - r) * (h - r) / r * (diff);
}

float2 CalculateLapVelocity(float r, float2 P_velocity, float2 N_velocity, float N_density)
{
	const float h = g_fSmoothlen;
	float2 vel_diff = (N_velocity - P_velocity);
		// Implements this equation:
		// W_viscosity(r, h) = 15 / (2 * pi * h^3) * (-r^3 / (2 * h^3) + r^2 / h^2 + h / (2 * r) - 1)
		// LAPLACIAN( W_viscosity(r, h) ) = 45 / (pi * h^6) * (h - r)
		// g_fLapViscosityCoef = fParticleMass * fViscosity * 45.0f / (PI * fSmoothlen^6)
		return g_fLapViscosityCoef / N_density * (h - r) * vel_diff;
}

StructuredBuffer<Particle> Particles : register(t0);
StructuredBuffer<ParticleDensity> ParticlesDensity : register(t1);

RWStructuredBuffer<ParticleForces> ParticlesForces : register(u0);

StructuredBuffer<uint2> GridIndices : register(t2);

[numthreads(BLOCKSIZE, 1, 1)]
void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
	const unsigned int ID = DTid.x;

	// Get current particle properties
	float3 position = Particles[ID].position;
	float3 velocity = Particles[ID].velocity;
	float density = ParticlesDensity[ID].density;

	float3 acceleration = float3(0.0f, 0.0f, 0.0f);

	// Calculate acceleration based on neighbors from the 8 adjacent cells and current cell
	int3 gridPos = CalcGridPos(position, originPosW, cellSize);
	uint gridHash = CalcGridHash(gridPos, gridSize);

	// Update forces with the calculated ones
	//ParticlesForces[ID].acceleration = acceleration / density;
	ParticlesForces[ID].acceleration = acceleration;
}