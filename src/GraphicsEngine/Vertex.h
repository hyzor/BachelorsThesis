#ifndef VERTEX_H
#define VERTEX_H

#include "Direct3D.h"
#include "ShaderHandler.h"

// Vertices
namespace Vertex
{
	struct Position
	{
		XMFLOAT3 position;
	};

	struct Position2D
	{
		XMFLOAT2 position;
	};

	struct PosTex
	{
		XMFLOAT3 position;
		XMFLOAT2 texCoord;
	};

	struct PosNormalTex
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 texCoord;
	};

	struct Particle
	{
		XMFLOAT3 InitialPos;
		XMFLOAT3 InitialVel;
		XMFLOAT2 Size;
		float Age;
		unsigned int Type;
	};

	struct PosNormalTexTan
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 texCoord;
		XMFLOAT4 tangentU;
	};

	struct PosNormalTexTanSkinned
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 texCoord;
		XMFLOAT4 tangentU;
		XMFLOAT3 weights;
		BYTE boneIndices[4];
	};

	struct PosNormalTexTargets4
	{
		XMFLOAT3 positionBase;
		XMFLOAT3 normalBase;
		XMFLOAT2 texCoordBase;

		XMFLOAT3 positionTarget1;
		XMFLOAT3 normalTarget1;
		XMFLOAT2 texCoordTarget1;

		XMFLOAT3 positionTarget2;
		XMFLOAT3 normalTarget2;
		XMFLOAT2 texCoordTarget2;

		XMFLOAT3 positionTarget3;
		XMFLOAT3 normalTarget3;
		XMFLOAT2 texCoordTarget3;

		XMFLOAT3 positionTarget4;
		XMFLOAT3 normalTarget4;
		XMFLOAT2 texCoordTarget4;
	};
}

class ShaderHandler;

// Input layout descriptions
class InputLayoutDesc
{
public:
	static const D3D11_INPUT_ELEMENT_DESC Position[1];
	static const D3D11_INPUT_ELEMENT_DESC Position2D[1];
	static const D3D11_INPUT_ELEMENT_DESC PosTex[2];
	static const D3D11_INPUT_ELEMENT_DESC PosNormalTex[3];
	static const D3D11_INPUT_ELEMENT_DESC Particle[5];
	static const D3D11_INPUT_ELEMENT_DESC PosNormalTexTan[4];
	static const D3D11_INPUT_ELEMENT_DESC PosNormalTexTanSkinned[6];
	static const D3D11_INPUT_ELEMENT_DESC PosNormalTexTargets4[15];
};

// Input layouts
class InputLayouts
{
public:
	InputLayouts();
	~InputLayouts();
	//static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static void CreateInputLayout(ID3D11Device* device,
		Shader* shader,
		const D3D11_INPUT_ELEMENT_DESC* layoutDesc,
		UINT layoutDescSize,
		ID3D11InputLayout** inputLayout);

	static ID3D11InputLayout* Position;
	static ID3D11InputLayout* Position2D;
	static ID3D11InputLayout* PosTex;
	static ID3D11InputLayout* PosNormalTex;
	static ID3D11InputLayout* Particle;
	static ID3D11InputLayout* PosNormalTexTan;
	static ID3D11InputLayout* PosNormalTexTanSkinned;
	static ID3D11InputLayout* PosNormalTexTargets4;
};

#endif