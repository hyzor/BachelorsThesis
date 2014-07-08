#include "Vertex.h"

// Must be included last!
#include "common/debug.h"

//===========================================================================
// Input layout descriptions
//===========================================================================
const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Position[1] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Position2D[1] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::PosTex[2] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::PosNormalTex[3] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::Particle[5] = 
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"AGE", 0, DXGI_FORMAT_R32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TYPE", 0, DXGI_FORMAT_R32_UINT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::PosNormalTexTan[4] = 
{
	{"POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TANGENT",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::PosNormalTexTanSkinned[6] = 
{
	{"POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"NORMAL",       0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",     0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TANGENT",      0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"WEIGHTS",      0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"BONEINDICES",  0, DXGI_FORMAT_R8G8B8A8_UINT,   0, 60, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc::PosNormalTexTargets4[15] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	{ "POSITION", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 56, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	{ "POSITION", 2, DXGI_FORMAT_R32G32B32_FLOAT, 0, 64, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 2, DXGI_FORMAT_R32G32B32_FLOAT, 0, 76, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 2, DXGI_FORMAT_R32G32_FLOAT, 0, 88, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	{ "POSITION", 3, DXGI_FORMAT_R32G32B32_FLOAT, 0, 96, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 3, DXGI_FORMAT_R32G32B32_FLOAT, 0, 108, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 3, DXGI_FORMAT_R32G32_FLOAT, 0, 120, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	{ "POSITION", 4, DXGI_FORMAT_R32G32B32_FLOAT, 0, 128, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 4, DXGI_FORMAT_R32G32B32_FLOAT, 0, 140, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 4, DXGI_FORMAT_R32G32_FLOAT, 0, 152, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

//===========================================================================
// Input layouts
//===========================================================================
ID3D11InputLayout* InputLayouts::Position = 0;
ID3D11InputLayout* InputLayouts::Position2D = 0;
ID3D11InputLayout* InputLayouts::PosTex = 0;
ID3D11InputLayout* InputLayouts::PosNormalTex = 0;
ID3D11InputLayout* InputLayouts::Particle = 0;
ID3D11InputLayout* InputLayouts::PosNormalTexTan = 0;
ID3D11InputLayout* InputLayouts::PosNormalTexTanSkinned = 0;
ID3D11InputLayout* InputLayouts::PosNormalTexTargets4 = 0;

void InputLayouts::DestroyAll()
{
	ReleaseCOM(Position);
	ReleaseCOM(Position2D);
	ReleaseCOM(PosTex);
	ReleaseCOM(PosNormalTex);
	ReleaseCOM(Particle);
	ReleaseCOM(PosNormalTexTan);
	ReleaseCOM(PosNormalTexTanSkinned);
	ReleaseCOM(PosNormalTexTargets4);
}

void InputLayouts::CreateInputLayout(ID3D11Device* device,
	Shader* shader,
	const D3D11_INPUT_ELEMENT_DESC* layoutDesc,
	UINT layoutDescSize,
	ID3D11InputLayout** inputLayout)
{
	device->CreateInputLayout(layoutDesc, layoutDescSize, shader->Buffer->GetBufferPointer(), shader->Buffer->GetBufferSize(), inputLayout);
}

InputLayouts::InputLayouts()
{

}

InputLayouts::~InputLayouts()
{

}
