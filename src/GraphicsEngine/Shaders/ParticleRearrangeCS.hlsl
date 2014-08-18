#include "ParticleSystemCS_Helper.hlsli"
#include "ParticleSystemDef.hlsli"
#include "ParticleDef.hlsli"

RWStructuredBuffer<SphereParticle> ParticlesRW : register(u0);
StructuredBuffer<SphereParticle> ParticlesRO : register(t0);

//StructuredBuffer<unsigned int> GridRO : register(t1);
StructuredBuffer<uint2> GridKeyValuePairRO : register(t1);

[numthreads(BLOCKSIZE, 1, 1)]
void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
	// Current grid ID
	const unsigned int ID = DTid.x;

	// Sorted key-value (hash, pId) pair ( (0,1) (0,3) (1,2) (1,7) (2,6) )
	const unsigned int G_ID = GridKeyValuePairRO[ID].y;
	
	// We rearrange the particle list to match the sorted 
	// grid key-value pair list.
	// Particles are now arranged so that the particle with the lowest 
	// grid hash is first and the particle with the highest grid hash is last

	ParticlesRW[ID] = ParticlesRO[G_ID];
}