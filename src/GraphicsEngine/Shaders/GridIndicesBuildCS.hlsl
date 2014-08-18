#include "ParticleSystemDef.hlsli"
#include "ParticleSystemCS_Helper.hlsli"

cbuffer cbConstants : register(b0)
{
	uint g_iNumElements;
	float3 padding;
};

RWStructuredBuffer<uint2> GridIndicesRW : register(u0);
StructuredBuffer<uint2> GridKeyValueRO : register(t0);

[numthreads(BLOCKSIZE, 1, 1)]
void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
	// The grid key value pair is now a sorted list consisting of
	// (grid hash, particle id)
	// Example: 0:(0,1), 1:(0,4), 2:(1,6), 3:(1,3), 4:(2,7), 5:(3,9)
	const unsigned int G_ID = DTid.x; // Grid ID (Key value pair) to operate on
	unsigned int G_ID_PREV = (G_ID == 0) ? g_iNumElements : G_ID; G_ID_PREV--;
	unsigned int G_ID_NEXT = G_ID + 1;
	if (G_ID_NEXT == g_iNumElements) { G_ID_NEXT = 0; }

	uint cell = GridKeyValueRO[G_ID].x;
	uint cell_prev = GridKeyValueRO[G_ID_PREV].x;
	uint cell_next = GridKeyValueRO[G_ID_NEXT].x;

	if (cell != cell_prev)
	{
		// The cell starts at this index in our grid
		GridIndicesRW[cell].x = G_ID;
	}

	if (cell != cell_next)
	{
		// The cell ends at this index in our grid
		GridIndicesRW[cell].y = G_ID + 1;
	}
}