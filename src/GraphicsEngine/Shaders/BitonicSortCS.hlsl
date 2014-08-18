//--------------------------------------------------------------------------------------
// File: BitonicSortCS.hlsl
//
// The code in this file originated from the FluidCS11 sample by Microsoft Corporation in
// the file ComputeShaderSort11.hlsl
//
// This file contains the compute shaders to perform GPU sorting using DirectX 11.
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#define BITONIC_BLOCK_SIZE 1024

cbuffer cBuffer : register(b0)
{
	unsigned int g_iLevel;
	unsigned int g_iLevelMask;
	float2 padding;
};

StructuredBuffer<uint2> Input : register(t0);
RWStructuredBuffer<uint2> Data : register(u0);

groupshared uint2 shared_data[BITONIC_BLOCK_SIZE];

[numthreads(BITONIC_BLOCK_SIZE, 1, 1)]
void main(uint3 Gid : SV_GroupID,
	uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID,
	uint GI : SV_GroupIndex)
{
	// Load shared data
	shared_data[GI] = Data[DTid.x];
	GroupMemoryBarrierWithGroupSync();

	// Sort the shared data
	for (unsigned int j = g_iLevel >> 1; j > 0; j >>= 1)
	{
		uint2 result = ((shared_data[GI & ~j].x <= shared_data[GI | j].x)
			== (bool)(g_iLevelMask & DTid.x)) ?
			shared_data[GI ^ j] : shared_data[GI];
		GroupMemoryBarrierWithGroupSync();
		shared_data[GI] = result;
		GroupMemoryBarrierWithGroupSync();
	}

	// Store shared data
	Data[DTid.x] = shared_data[GI];
}