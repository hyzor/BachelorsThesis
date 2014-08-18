#include "ParticleDef.hlsli"
#include "ParticleSystemDef.hlsli"
#include "ParticleSystemCS_Helper.hlsli"

cbuffer cbConstants : register(b0)
{
	uint groupDimX;
	uint groupDimY;
	uint numParticles;
	int padding;

	/*
	uint3 gridSize;
	uint numCells;

	float cellSize;
	float3 originPosW;
	*/

	float gravity;
	float3 boundaries;
}

cbuffer cbPerFrame : register(b1)
{
	float dt;
	float3 padding2;
	/*
	float3 boundaries;

	float gravity;
	float3 padding2;
	*/
}

RWStructuredBuffer<SphereParticle> srcParticleBuffer : register(u0); // Unsorted particle list
StructuredBuffer<SphereParticle> ParticlesRO : register(t0); // Sorted particle list

StructuredBuffer<ParticleForces> ParticlesForces : register(t1);

//RWStructuredBuffer<unsigned int> Grid : register(u1); // Grid with hash and value (particle ID) pairs

[numthreads(BLOCKSIZE, 1, 1)]
//void main(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
	// Calculate the ID of this thread
	//uint ID = groupID.x * BLOCKSIZE + groupID.y * groupDimX * BLOCKSIZE + groupID.z * groupDimX * groupDimY * BLOCKSIZE + groupIndex;

	const unsigned int ID = DTid.x;

	// Every thread renders a particle, check if the ID is less than the number of particles
	if (ID < numParticles)
	{
		// DEM/SPH
		float3 position = ParticlesRO[ID].position;
		float3 velocity = ParticlesRO[ID].velocity;
		float3 acceleration = ParticlesForces[ID].acceleration;
		acceleration = acceleration + float3(0.0f, -(gravity), 0.0f);

		// Gravity only
		//float3 position = srcParticleBuffer[ID].position;
		//float3 velocity = srcParticleBuffer[ID].velocity;
		//float3 acceleration = float3(0.0f, -(gravity), 0.0f);

		float3 oldVelocity = velocity;

		velocity = velocity + acceleration * dt;
		position = position + (oldVelocity + velocity) * 0.5f * dt;

		float boundaryDamping = -0.225f;

		if (position.x > boundaries.x)
		{
			position.x = boundaries.x;
			//p.velocity.x = p.velocity.x * -1.0f;
			velocity.x = velocity.x * boundaryDamping;
			//p.time = 0.0f;
		}
		else if (position.x < -boundaries.x)
		{
			position.x = -boundaries.x;
			velocity.x = velocity.x * boundaryDamping;
			//p.velocity.x = p.velocity.x * -1.0f;
			//p.time = 0.0f;
		}

		if (position.y > boundaries.y)
		{
			position.y = boundaries.y;
			velocity.y = velocity.y * boundaryDamping;
			//p.velocity.y = p.velocity.y * -1.0f;
			//p.time = 0.0f;
		}
		else if (position.y < -boundaries.y)
		{
			position.y = -boundaries.y;
			velocity.y = velocity.y * boundaryDamping;
			//p.acceleration.y = (p.acceleration.y / p.mass) * -1.0f;
			//p.velocity.y = p.velocity.y * -1.0f;
			//p.time = 0.0f;
		}

		if (position.z > boundaries.z)
		{
			position.z = boundaries.z;
			velocity.z = velocity.z * boundaryDamping;
			//p.time = 0.0f;
		}
		else if (position.z < -boundaries.z)
		{
			position.z = -boundaries.z;
			velocity.z = velocity.z * boundaryDamping;

			//float dist = dot(p.position, float3(0.0f, 0.0f, -boundaries.z));
			//p.acceleration += min(dist, 0) * -3000.0f;
			//p.time = 0.0f;
		}
		
		// g_fWallStiffness = 3000.0f

		// Find neighboring particles (thus eligible for collision testing)
		
		// Step 1: Calculate which grid cell this particle belongs to
		//int3 gridPos = CalcGridPos(p.position, originPosW, cellSize);
		//uint gridHash = CalcGridHash(gridPos, gridSize); // Grid hash means cell index in this case

		// Step 2: Assign this particle to the newly found cell
		//p.gridHash = gridHash;
		//Grid[ID] = GridConstructKeyValuePair(gridHash, ID);

		// Step 3 (EXTERNAL STEP): We now have to sort the whole particle array by their cell IDs (grid hash), and for every cell set
		// the start and end indices in our particle array.

		// Return updated particle to the buffer
		//srcParticleBuffer[ID] = p;

		srcParticleBuffer[ID].position = position;
		srcParticleBuffer[ID].velocity = velocity;
	}
}