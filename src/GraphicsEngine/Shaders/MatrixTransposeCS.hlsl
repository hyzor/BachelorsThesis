//--------------------------------------------------------------------------------------
// File: MatrixTransposeCS.hlsl
//
// The code in this file originated from the FluidCS11 sample by Microsoft Corporation in
// the file ComputeShaderSort11.hlsl
//
// This file contains the compute shaders to perform GPU sorting using DirectX 11.
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#define TRANSPOSE_BLOCK_SIZE 16

StructuredBuffer<unsigned int> Input : register(t0);
RWStructuredBuffer<unsigned int> Data : register(u0);

cbuffer cBuffer : register(b0)
{
	unsigned int g_iWidth;
	unsigned int g_iHeight;
	float2 padding;
};

groupshared unsigned int transpose_shared_data[TRANSPOSE_BLOCK_SIZE * TRANSPOSE_BLOCK_SIZE];

[numthreads(TRANSPOSE_BLOCK_SIZE, TRANSPOSE_BLOCK_SIZE, 1)]
void main(uint3 Gid : SV_GroupID,
	uint3 DTid : SV_DispatchThreadID,
	uint3 GTid : SV_GroupThreadID,
	uint GI : SV_GroupIndex)
{
	transpose_shared_data[GI] = Input[DTid.y * g_iWidth + DTid.x];
	GroupMemoryBarrierWithGroupSync();
	uint2 XY = DTid.yx - GTid.yx + GTid.xy;
	Data[XY.y * g_iHeight + XY.x] = transpose_shared_data[GTid.x * TRANSPOSE_BLOCK_SIZE + GTid.y];
}