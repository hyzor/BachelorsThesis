#include "ParticleDef.hlsli"
#include "ParticleSystemDef.hlsli"
#include "ParticleSystemCS_Helper.hlsli"

cbuffer cbConstants : register(b0)
{
	uint3 gridSize;
	uint numCells;

	float3 cellSize;
	float3 originPosW;
};

StructuredBuffer<SphereParticle> Particles : register(t0);
RWStructuredBuffer<unsigned int> Grid : register(u0);

[numthreads(BLOCKSIZE, 1, 1)]
void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
//void main(uint3 groupID : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
	const unsigned int ID = DTid.x;
	//uint ID = groupID.x * BLOCKSIZE + groupID.y * groupDimX * BLOCKSIZE + groupID.z * groupDimX * groupDimY * BLOCKSIZE + groupIndex;

	float3 position = Particles[ID].position;

	// Step 1: Calculate which grid cell this particle belongs to
	int3 gridPos = CalcGridPos(position, originPosW, cellSize);
	uint gridHash = CalcGridHash(gridPos, gridSize); // Grid hash means cell index in this case

	// Step 2: Assign this particle to the newly found cell
	//p.gridHash = gridHash;
	Grid[ID] = GridConstructKeyValuePair(gridHash, ID);
}