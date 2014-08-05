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
	/*
	for (uint i = 0; i < 4; i++)
	{
		GeometryOut gOut;
		gOut.PosH = mul(gIn[0].PosW, viewProj);
		output.Append(gOut);
	}
	*/

	//float4 posW = gIn[0].PosW;
	GeometryOut gOut;

	/*
	// Sphere top vertex
	PosW.y = PosW.y + gIn[0].Radius;
	gOut.PosH = mul(PosW, viewProj);
	output.Append(gOut);

	// Sphere bottom vertex
	PosW = gIn[0].PosW;
	PosW.y = PosW.y - gIn[0].Radius;
	gOut.PosH = mul(PosW, viewProj);
	output.Append(gOut);

	// Sphere left vertex
	PosW = gIn[0].PosW;
	PosW.x = PosW.x - gIn[0].Radius;
	gOut.PosH = mul(PosW, viewProj);
	output.Append(gOut);

	// Sphere right vertex
	PosW = gIn[0].PosW;
	PosW.x = PosW.x + gIn[0].Radius;
	gOut.PosH = mul(PosW, viewProj);
	output.Append(gOut);
	*/

	float3 look = normalize(eyePosW - gIn[0].PosW.xyz);
	float3 right = normalize(cross(float3(0.0f, 1.0f, 0.0f), look));
	float3 up = cross(look, right);

		/*
		float4 normal = float4(gIn[0].PosW.xyz - eyePos, 1.0f);
		normal = mul(normal, view);

		float3 rightAxis = cross(float3(0.0f, 1.0f, 0.0f), normal.xyz);
		float3 upAxis = cross(normal.xyz, rightAxis);
		rightAxis = normalize(rightAxis);
		upAxis = normalize(upAxis);

		float4 rightVector = float4(rightAxis.xyz, 1.0f);
		float4 upVector = float4(upAxis.xyz, 1.0f);
		*/

		//gIn[0].PosW = mul(gIn[0].PosW, view);

	float4 v[4];
	v[0] = float4(gIn[0].PosW.xyz + right * gIn[0].Radius - up * gIn[0].Radius, 1.0f);
	v[1] = float4(gIn[0].PosW.xyz + right * gIn[0].Radius + up * gIn[0].Radius, 1.0f);
	v[2] = float4(gIn[0].PosW.xyz - right * gIn[0].Radius - up * gIn[0].Radius, 1.0f);
	v[3] = float4(gIn[0].PosW.xyz - right * gIn[0].Radius + up * gIn[0].Radius, 1.0f);

	//gOut.PosH = float4(gIn[0].PosW.xyz + right * (gIn[0].Radius) + up * (gIn[0].Radius), 1.0f);
	gOut.Tex = float2(0.0f, 1.0f);
	gOut.PosH = mul(v[0], viewProj);
	output.Append(gOut);

	//gOut.PosH = float4(gIn[0].PosW.xyz + right * (gIn[0].Radius) + up * (-gIn[0].Radius), 1.0f);
	gOut.Tex = float2(1.0f, 1.0f);
	gOut.PosH = mul(v[1], viewProj);
	output.Append(gOut);

	//gOut.PosH = float4(gIn[0].PosW.xyz + right * (-gIn[0].Radius) + up * (gIn[0].Radius), 1.0f);
	gOut.Tex = float2(0.0f, 0.0f);
	gOut.PosH = mul(v[2], viewProj);
	output.Append(gOut);

	//gOut.PosH = float4(gIn[0].PosW.xyz + right * (-gIn[0].Radius) + up * (-gIn[0].Radius), 1.0f);
	gOut.Tex = float2(1.0f, 0.0f);
	gOut.PosH = mul(v[3], viewProj);
	output.Append(gOut);
}