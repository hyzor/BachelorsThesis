#include "ParticleSystemDef.hlsli"

RWStructuredBuffer<uint2> GridIndicesRW : register(u0);

[numthreads(BLOCKSIZE, 1, 1)]
void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
	GridIndicesRW[DTid.x] = uint2(0, 0);
}