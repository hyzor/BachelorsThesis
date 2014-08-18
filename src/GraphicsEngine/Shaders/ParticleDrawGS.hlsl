cbuffer cbPerFrame : register(b0)
{
	float4x4 viewProj;
	float4x4 view;
	float4x4 proj;
	float3 eyePosW;
};

struct GeometryIn
{
	float4 PosW : SV_POSITION;
	float Radius : RADIUS;
};

struct GeometryOut
{
	float4 PosH : SV_POSITION;
	float2 Tex : TEXCOORD;
};

[maxvertexcount(4)]
void main(point GeometryIn gIn[1] : SV_POSITION,
	inout TriangleStream<GeometryOut> output)
{
	GeometryOut gOut;

	float3 look = normalize(eyePosW - gIn[0].PosW.xyz);
	float3 right = normalize(cross(float3(0.0f, 1.0f, 0.0f), look));
	float3 up = cross(look, right);

	float radius = gIn[0].Radius;

	float4 v[4];
	v[0] = float4(gIn[0].PosW.xyz + right * radius - up * radius, 1.0f);
	v[1] = float4(gIn[0].PosW.xyz + right * radius + up * radius, 1.0f);
	v[2] = float4(gIn[0].PosW.xyz - right * radius - up * radius, 1.0f);
	v[3] = float4(gIn[0].PosW.xyz - right * radius + up * radius, 1.0f);

	gOut.Tex = float2(0.0f, 1.0f);
	gOut.PosH = mul(v[0], viewProj);
	output.Append(gOut);

	gOut.Tex = float2(1.0f, 1.0f);
	gOut.PosH = mul(v[1], viewProj);
	output.Append(gOut);

	gOut.Tex = float2(0.0f, 0.0f);
	gOut.PosH = mul(v[2], viewProj);
	output.Append(gOut);

	gOut.Tex = float2(1.0f, 0.0f);
	gOut.PosH = mul(v[3], viewProj);
	output.Append(gOut);
}