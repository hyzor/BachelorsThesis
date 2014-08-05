#include "ParticleDef.hlsli"
#include "ParticleSystemDef.hlsli"
#include "ParticleSystemCS_Helper.hlsli"

cbuffer cbConstants : register(b0)
{
	uint groupDimX;
	uint groupDimY;
	uint numParticles;
	int padding;
}

StructuredBuffer<SphereParticle> Particles : register(t0);
StructuredBuffer<uint2> GridIndices : register(t1);

/*
// Function that calculates the resulting force from a collision between two spheres
float3 CollideSpheres(SphereParticle sphereA, SphereParticle sphereB)
{
	float3 relativePos = sphereB.position - sphereA.position;
	float dist = length(relativePos);
	float collisionRange = sphereA.radius + sphereB.radius;

	float3 force = float3(0.0f, 0.0f, 0.0f);

	if (dist < collisionRange)
	{
		// Calculate force
	}

	return force;
}
*/

[numthreads(BLOCKSIZE, 1, 1)]
void main(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
	// Calculate the ID of this thread
	uint ID = groupID.x * BLOCKSIZE + groupID.y * groupDimX * BLOCKSIZE + groupID.z * groupDimX * groupDimY * BLOCKSIZE + groupIndex;

	// Every thread renders a particle, check if the ID is less than the number of particles
	if (ID < numParticles)
	{
		// This particle should now have been assigned to a cell. We now proceed to perform collision detection
		// against every other neighboring particle by accessing the current cell and neighboring cells to the cell this particle belongs to.
		//SphereParticle p = Particles[ID];
		//float3 acceleration = p.acceleration;

		// Step 1: Iterate all the particles in the current cell and see if there is any collision.


		// Step 2: Iterate all the particles in the neighboring cells to see if there is any collision.

	}
}