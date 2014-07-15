#include "ParticleDef.hlsli"
#include "ParticleSystemDef.hlsli"

cbuffer cbConstants : register(b0)
{
	uint groupDimX;
	uint groupDimY;
	uint numParticles;
	int padding;
}

cbuffer cbPerFrame : register(b1)
{
	float frameTime;
	float3 padding2;
}

RWStructuredBuffer<SphereParticle> srcParticleBuffer : register(u0);

[numthreads(BLOCKSIZE, 1, 1)]
void main(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
	// Calculate the ID of this thread
	uint ID = groupID.x * BLOCKSIZE + groupID.y * groupDimX * BLOCKSIZE + groupID.z * groupDimX * groupDimY * BLOCKSIZE + groupIndex;

	// Every thread renders a particle, check if the ID is less than the number of particles
	if (ID < numParticles)
	{
		// Get the current particle from the buffer
		SphereParticle p = srcParticleBuffer[ID];

		// Do something with this particle

		// Return updated particle to the buffer
		srcParticleBuffer[ID] = p;
	}
}