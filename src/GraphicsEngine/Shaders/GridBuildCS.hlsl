#include "ParticleDef.hlsli"
#include "ParticleSystemDef.hlsli"
#include "ParticleSystemCS_Helper.hlsli"

cbuffer cbConstants : register(b0)
{
	uint3 gridSize;
	uint numCells;

	float3 cellSize;
	float padding;

	float3 originPosW;
	float padding2;
};

StructuredBuffer<SphereParticle> Particles : register(t0);

RWStructuredBuffer<uint2> GridKeyValuePair : register(u0);

[numthreads(BLOCKSIZE, 1, 1)]
void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
	const unsigned int ID = DTid.x;

	float3 position = Particles[ID].position;

	// Calculate which grid cell this particle belongs to
	int3 gridPos = CalcGridPos(position, originPosW, cellSize);
	// Grid hash means cell index in this case
	uint gridHash = CalcGridHash(gridPos, gridSize);

	GridKeyValuePair[ID] = uint2(gridHash, ID);
}