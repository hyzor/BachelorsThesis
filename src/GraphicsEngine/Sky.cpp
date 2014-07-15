//------------------------------------------------------------------------------------
// File: Sky.cpp
//
// This class renders a sky using a cube map
//------------------------------------------------------------------------------------

#include "Sky.h"
#include "GeometryGenerator.h"
#include "Effects.h"
#include "Vertex.h"

// Must be included last!
#include "common/debug.h"

Sky::Sky(ID3D11Device* device, TextureManager *textureManager, const std::string& cubeMapFileName, float skySphereRadius)
{
	// Create texture
	mCubeMapSRV = textureManager->CreateStaticTexture(cubeMapFileName);

	// Generate sphere
	GeometryGenerator::MeshData sphere;
	GeometryGenerator geoGen;
	geoGen.CreateSphere(skySphereRadius, 30, 30, sphere);

	std::vector<XMFLOAT3> vertices(sphere.vertices.size());

	for(size_t i = 0; i < sphere.vertices.size(); ++i)
	{
		vertices[i] = sphere.vertices[i].position;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = (UINT)(sizeof(XMFLOAT3) * vertices.size());
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];

	//HR(device->CreateBuffer(&vbd, &vinitData, &mVertexBuffer));
	device->CreateBuffer(&vbd, &vinitData, &mVertexBuffer);


	mIndexCount = (UINT)sphere.indices.size();

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(USHORT) * mIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.StructureByteStride = 0;
	ibd.MiscFlags = 0;

	std::vector<USHORT> indices16;
	indices16.assign(sphere.indices.begin(), sphere.indices.end());

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices16[0];

	device->CreateBuffer(&ibd, &iinitData, &mIndexBuffer);
}

Sky::Sky(ID3D11Device* device, TextureManager *textureManager, float skySphereRadius)
{
	// Generate sphere
	GeometryGenerator::MeshData sphere;
	GeometryGenerator geoGen;
	geoGen.CreateSphere(skySphereRadius, 30, 30, sphere);

	std::vector<XMFLOAT3> vertices(sphere.vertices.size());

	for (size_t i = 0; i < sphere.vertices.size(); ++i)
	{
		vertices[i] = sphere.vertices[i].position;
	}

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = (UINT)(sizeof(XMFLOAT3)* vertices.size());
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];

	//HR(device->CreateBuffer(&vbd, &vinitData, &mVertexBuffer));
	device->CreateBuffer(&vbd, &vinitData, &mVertexBuffer);


	mIndexCount = (UINT)sphere.indices.size();

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(USHORT)* mIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.StructureByteStride = 0;
	ibd.MiscFlags = 0;

	std::vector<USHORT> indices16;
	indices16.assign(sphere.indices.begin(), sphere.indices.end());

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices16[0];

	device->CreateBuffer(&ibd, &iinitData, &mIndexBuffer);

	mCubeMapSRV = nullptr;
}


Sky::~Sky(void)
{
	ReleaseCOM(mVertexBuffer);
	ReleaseCOM(mIndexBuffer);
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Sky::GetCubeMapSRV()
{
	return mCubeMapSRV;
}

void Sky::Draw(ID3D11DeviceContext* dc, const Camera& cam, SkyDeferredShader* skyShader)
{
	// Center sky about eye in world space
	XMFLOAT3 eyePos = cam.GetPosition();
	XMMATRIX T = XMMatrixTranslation(eyePos.x, eyePos.y, eyePos.z);

	XMMATRIX WVP = XMMatrixMultiply(T, cam.GetViewProjMatrix());

	skyShader->SetActive(dc);
	skyShader->SetWorldViewProj(WVP);
	skyShader->SetCubeMap(dc, mCubeMapSRV.Get());
	skyShader->UpdatePerFrame(dc);

	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;
	dc->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	dc->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	dc->IASetInputLayout(InputLayouts::Position);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dc->RSSetState(RenderStates::mNoCullRS);
	dc->OMSetDepthStencilState(RenderStates::mLessEqualDSS, 0);

	dc->DrawIndexed(mIndexCount, 0, 0);
}

void Sky::SetTexture(const std::string& fileName, TextureManager* textureManager)
{
	textureManager->DeleteTexture(mCubeMapSRV.Get());
	mCubeMapSRV = textureManager->CreateTexture(fileName);
}