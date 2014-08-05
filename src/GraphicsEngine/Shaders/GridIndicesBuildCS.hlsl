#include "ParticleSystemDef.hlsli"
#include "ParticleSystemCS_Helper.hlsli"

cbuffer cbConstants : register(b0)
{
	uint g_iNumElements;
	float3 padding;
};

RWStructuredBuffer<uint2> GridIndicesRW : register(u0);
StructuredBuffer<unsigned int> GridRO : register(t0);

[numthreads(BLOCKSIZE, 1, 1)]
void main(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
	const unsigned int G_ID = DTid.x; // Grid ID to operate on
	unsigned int G_ID_PREV = (G_ID == 0) ? g_iNumElements : G_ID; G_ID_PREV--;
	unsigned int G_ID_NEXT = G_ID + 1; if (G_ID_NEXT == g_iNumElements) { G_ID_NEXT = 0; }

	unsigned int cell = GridGetKey(GridRO[G_ID]);
	unsigned int cell_prev = GridGetKey(GridRO[G_ID_PREV]);
	unsigned int cell_next = GridGetKey(GridRO[G_ID_NEXT]);

	if (cell != cell_prev)
	{
		// I'm the start of a cell
		GridIndicesRW[cell].x = G_ID;
	}

	if (cell != cell_next)
	{
		// I'm the end of a cell
		GridIndicesRW[cell].y = G_ID + 1;
	}
}