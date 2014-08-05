#include "ParticleDef.hlsli"

cbuffer cbPerFrame : register(b0)
{
	float4x4 viewProj;
	float particleRadius;
	float3 padding;
};

struct VertexIn
{
	uint VertexID : SV_VertexID;
};

struct VertexOut
{
	//float4 PosH : SV_POSITION;
	float4 PosW : SV_POSITION;
	float Radius : RADIUS;
};

StructuredBuffer<SphereParticle> particleBuffer : register(u0);

VertexOut main(VertexIn vIn)
{
	VertexOut vOut;

	//float4 PosW = float4(particleBuffer[vIn.VertexID].position.xyz, 1.0f);
	//vOut.PosH = mul(PosW, viewProj);

	vOut.PosW = float4(particleBuffer[vIn.VertexID].position.xyz, 1.0f);
	//vOut.Radius = particleBuffer[vIn.VertexID].radius;
	vOut.Radius = particleRadius;

	return vOut;
}