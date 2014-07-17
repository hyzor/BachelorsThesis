#include "ParticleDef.hlsli"

cbuffer cbPerFrame
{
	float4x4 viewProj;
};

struct VertexIn
{
	uint VertexID : SV_VertexID;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
};

StructuredBuffer<SphereParticle> particleBuffer : register(u0);

VertexOut main(VertexIn vIn)
{
	VertexOut vOut;

	float4 PosW = float4(particleBuffer[vIn.VertexID].position.xyz, 1.0f);

	vOut.PosH = mul(PosW, viewProj);

	//vOut.PosH.xyz = PosW; // multiplicate with viewProj matrix
	//vOut.PosH.w = 1.0f;

	return vOut;
}