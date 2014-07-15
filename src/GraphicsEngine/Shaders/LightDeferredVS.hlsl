cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
};

struct VertexIn
{
	float4 Pos : POSITION;
	float2 Tex : TEXCOORD0;
};

struct VertexOut
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};

VertexOut main(VertexIn vIn)
{
	VertexOut vOut;

	vIn.Pos.w = 1.0f;

	// Transform to homogeneous clip space
	vOut.Pos = mul(vIn.Pos, gWorldViewProj);

	// Store texture coordinates for pixel shader
	vOut.Tex = vIn.Tex;

	return vOut;
}