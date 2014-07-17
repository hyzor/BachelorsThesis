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
	float dt;
	float3 boundaries;

	float gravity;
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

		// Update (or predict particle position), acceleration is based on gravity times particle time
		//float3 acceleration = float3(0.0f, -gravity * p.time, 0.0f) / p.mass;
		float3 oldVelocity = p.velocity;

		// Update velocity and position
		p.velocity = p.velocity + p.acceleration * dt;
		p.position = p.position + (oldVelocity + p.velocity) * 0.5f * dt;

		p.acceleration = float3(0.0f, -gravity * p.time, 0.0f) / p.mass;
		p.time += dt;

		if (p.position.x > boundaries.x)
		{
			p.position.x = boundaries.x;
			p.velocity.x = p.velocity.x * -1.0f;
			//p.time = 0.0f;
		}
		else if (p.position.x < -boundaries.x)
		{
			p.position.x = -boundaries.x;
			p.velocity.x = p.velocity.x * -1.0f;
			//p.time = 0.0f;
		}

		if (p.position.y > boundaries.y)
		{
			p.position.y = boundaries.y;
			p.velocity.y = p.velocity.y * -1.0f;
			//p.time = 0.0f;
		}
		else if (p.position.y < -boundaries.y)
		{
			p.position.y = -boundaries.y;
			p.velocity.y = p.velocity.y * -1.0f;
			//p.time = 0.0f;
		}

		if (p.position.z > boundaries.z)
		{
			p.position.z = boundaries.z;
			p.velocity.z = p.velocity.z * -1.0f;
			//p.time = 0.0f;
		}
		else if (p.position.z < -boundaries.z)
		{
			p.position.z = -boundaries.z;
			p.velocity.z = p.velocity.z * -1.0f;
			//p.time = 0.0f;
		}

		// Find neighboring particles (thus eligible for collision testing)

		// Return updated particle to the buffer
		srcParticleBuffer[ID] = p;
	}
}