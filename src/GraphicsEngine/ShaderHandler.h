#ifndef SHADERHANDLER_H_
#define SHADERHANDLER_H_

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <d3dcompiler.h>

#include "Direct3D.h"
#include "LightDef.h"
#include "RenderStates.h"

#include "common/util.h"
#include "DirectXUtil.h"

//If you are changing this, change the MAX_POINT_LIGHTS, MAX_DIR_LIGHTS and MAX_SPOT_LIGHTS in LightDef.hlsli
#define MAX_POINT_LIGHTS 16
#define MAX_DIR_LIGHTS 2
#define MAX_SPOT_LIGHTS 32
#define MAX_CASC 4
#define MAX_MATERIALS 64

using namespace DirectX;

// Shader interface
// This shader object contains pointers to loaded compiled shaders and handles the
// updating of constant buffers etc.
class IShader
{
public:
	void* operator new(size_t size)
	{
		void* p = _aligned_malloc(size, 16);

		if (!p)
			throw std::bad_alloc();

		return p;
	}

	void operator delete(void* p)
	{
		_aligned_free(p);
	}

	IShader();
	~IShader();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout,
		ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader);
	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout,
		ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11GeometryShader* geometryShader);
	bool Init(ID3D11Device* device, ID3D11ComputeShader* computeShader);

	void SetActive(ID3D11DeviceContext* dc);

	const ID3D11VertexShader* GetVertexShader();
	const ID3D11PixelShader* GetPixelShader();
	const ID3D11GeometryShader* GetGeometryShader();
	const ID3D11ComputeShader* GetComputeShader();

	//virtual void UpdatePerFrame(ID3D11DeviceContext* dc) = 0;

	void UpdatePerFrame(ID3D11DeviceContext* dc);
	void UpdatePerObj(ID3D11DeviceContext* dc);

	template<typename bufferVariables>
	static HRESULT CreateBuffer(ID3D11Device* device, ID3D11Buffer** buffer, bufferVariables* bufferVars);

	template<typename bufferVariables>
	static HRESULT CreateConstantBuffer(ID3D11Device* device, ID3D11Buffer** buffer, bufferVariables* bufferVars);

	template<typename bufferVariables>
	static HRESULT UpdateBuffer(ID3D11DeviceContext* dc, ID3D11Buffer* buffer, bufferVariables* bufferVars);

protected:

	ID3D11InputLayout* mInputLayout;

	struct VertexShaderWrapper
	{
		ID3D11VertexShader* VertexShader;
		ID3D11Buffer* PerObjBuffer;
		ID3D11Buffer* PerFrameBuffer;
	};

	struct PixelShaderWrapper
	{
		ID3D11PixelShader* PixelShader;
		ID3D11Buffer* PerObjBuffer;
		ID3D11Buffer* PerFrameBuffer;
	};

	struct GeometryShaderWrapper
	{
		ID3D11GeometryShader* GeometryShader;
		ID3D11Buffer* PerObjBuffer;
		ID3D11Buffer* PerFrameBuffer;
	};

	struct ComputeShaderWrapper
	{
		ID3D11ComputeShader* ComputeShader;
		ID3D11Buffer* PerObjBuffer;
		ID3D11Buffer* PerFrameBuffer;
	};

	VertexShaderWrapper mVertexShaderWrapper;
	PixelShaderWrapper mPixelShaderWrapper;
	GeometryShaderWrapper mGeometryShaderWrapper;
	ComputeShaderWrapper mComputeShaderWrapper;
};

template<typename bufferVariables>
HRESULT IShader::CreateConstantBuffer(ID3D11Device* device, ID3D11Buffer** buffer, bufferVariables* bufferVars)
{
	HRESULT hr;

	ZeroMemory(bufferVars, sizeof(bufferVariables));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(bufferVariables);
	cbDesc.Usage = D3D11_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &bufferVars;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	hr = device->CreateBuffer(&cbDesc, &InitData, buffer);

	return hr;
}

template<typename bufferVariables>
HRESULT IShader::CreateBuffer(ID3D11Device* device, ID3D11Buffer** buffer, bufferVariables* bufferVars)
{
	HRESULT hr;

	ZeroMemory(bufferVars, sizeof(bufferVariables));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(bufferVariables);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &bufferVars;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	hr = device->CreateBuffer(&cbDesc, &InitData, buffer);

	return hr;
}

//--------------------------------------------------------------------------------------
// Helper for creating structured buffers with an SRV and UAV
//--------------------------------------------------------------------------------------
template <class T>
HRESULT CreateStructuredBuffer(ID3D11Device* pd3dDevice, UINT iNumElements, ID3D11Buffer** ppBuffer, ID3D11ShaderResourceView** ppSRV, ID3D11UnorderedAccessView** ppUAV, const T* pInitialData = nullptr)
{
	HRESULT hr = S_OK;

	// Create SB
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.ByteWidth = iNumElements * sizeof(T);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(T);

	D3D11_SUBRESOURCE_DATA bufferInitData;
	ZeroMemory(&bufferInitData, sizeof(bufferInitData));
	bufferInitData.pSysMem = pInitialData;
	hr = pd3dDevice->CreateBuffer(&bufferDesc, (pInitialData) ? &bufferInitData : nullptr, ppBuffer);

	if (FAILED(hr))
		return hr;

	// Create SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementWidth = iNumElements;
	hr = pd3dDevice->CreateShaderResourceView(*ppBuffer, &srvDesc, ppSRV);

	if (FAILED(hr))
		return hr;

	// Create UAV
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = iNumElements;
	hr = pd3dDevice->CreateUnorderedAccessView(*ppBuffer, &uavDesc, ppUAV);

	return hr;
}

template<typename bufferVariables>
HRESULT IShader::UpdateBuffer(ID3D11DeviceContext* dc, ID3D11Buffer* buffer, bufferVariables* bufferVars)
{
	HRESULT hr;

	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Vertex shader per obj buffer
	hr = dc->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	bufferVariables* dataPtr = (bufferVariables*)mappedResource.pData;
	*dataPtr = *bufferVars;
	dc->Unmap(buffer, 0);

	return hr;
}

class SkyDeferredShader : public IShader
{
public:
	SkyDeferredShader();
	~SkyDeferredShader();

	void SetActive(ID3D11DeviceContext* dc);

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout,
		ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader);

	void SetWorldViewProj(const XMMATRIX& worldViewProj);
	void SetCubeMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* cubeMap);

	void UpdatePerFrame(ID3D11DeviceContext* dc);

private:
	struct BUFFER_PERFRAME_VS
	{
		XMMATRIX worldViewProj;
	};

	BUFFER_PERFRAME_VS mBuffer_PerFrame_VS;
};

class BasicDeferredShader : public IShader
{
public:
	BasicDeferredShader();
	~BasicDeferredShader();

	void SetActive(ID3D11DeviceContext* dc);

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout,
		ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader);

	void SetWorldViewProjTex(XMMATRIX& world,
		XMMATRIX& viewProj,
		XMMATRIX& tex);

	void SetMaterial(const Material& mat, UINT globalMaterialIndex);
	void SetDiffuseMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

	void UpdatePerObj(ID3D11DeviceContext* dc);

private:
	struct BUFFER_PEROBJ_VS
	{
		XMMATRIX world;
		XMMATRIX worldInvTranspose;
		XMMATRIX worldViewProj;
		XMMATRIX texTransform;
	};
	
	struct BUFFER_PEROBJ_PS
	{
		Material mat;

		unsigned int globalMaterialIndex;
		XMFLOAT3 padding;
	};

	BUFFER_PEROBJ_VS mBuffer_PerObj_VS;
	BUFFER_PEROBJ_PS mBuffer_PerObj_PS;
};

class LightDeferredShader : public IShader
{
public:
	LightDeferredShader();
	~LightDeferredShader();

	void SetActive(ID3D11DeviceContext* dc);

	void Init(ID3D11Device* device, ID3D11InputLayout* inputLayout,
		ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader);

	void SetWorldViewProj(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj);

	void SetEyePosW(XMFLOAT3 eyePosW);
	void SetCameraViewProjMatrix(XMMATRIX& camViewMatrix, XMMATRIX& proj);

	void SetPointLights(ID3D11DeviceContext* dc, UINT numPointLights, PointLight* pointLights[]);
	void SetDirLights(ID3D11DeviceContext* dc, UINT numDirLights, DirLight* dirLights[]);
	void SetSpotLights(ID3D11DeviceContext* dc, UINT numSpotLights, SpotLight* spotLights[]);
	void SetMaterials(ID3D11DeviceContext* dc, UINT numMaterials, Material* mats[]);

	void SetDiffuseTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetNormalTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetDepthTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

	void SetSkipLighting(bool skipLighting);
	void SetSkipProcessing(bool skipProcessing);

	void UpdatePerObj(ID3D11DeviceContext* dc);
	void UpdatePerFrame(ID3D11DeviceContext* dc);

private:
	struct BUFFER_PEROBJ_VS
	{
		XMMATRIX worldViewProj;
	};

	struct BUFFER_PERFRAME_PS
	{
		PointLight pointLights[MAX_POINT_LIGHTS];
		DirLight dirLights[MAX_DIR_LIGHTS];
		SpotLight spotLights[MAX_SPOT_LIGHTS];

		UINT numPLights;
		UINT numDirLights;
		UINT numSLights;
		UINT skipProcessing;

		// Forms into a 4D vector
		XMFLOAT3 gEyePosW;
		int skipLighting;

		XMMATRIX camViewProjInv;

		Material materials[MAX_MATERIALS];
	};

	BUFFER_PEROBJ_VS mBuffer_PerObj_VS;
	BUFFER_PERFRAME_PS mBuffer_PerFrame_PS;
};

class ParticleDrawShader : public IShader
{
public:
	ParticleDrawShader();
	~ParticleDrawShader();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout,
		ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader);

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout,
		ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader,
		ID3D11GeometryShader* geometryShader);

	void SetViewProj(XMMATRIX& viewProj, XMMATRIX& view, XMMATRIX& proj);
	void SetEyePos(XMFLOAT3 eyePos);
	void SetDiffuseTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetActive(ID3D11DeviceContext* dc);

	void SetParticleRadius(float particleRadius);

	void UpdatePerFrame(ID3D11DeviceContext* dc);

private:
	struct BUFFER_PERFRAME_VS
	{
		XMMATRIX viewProj;
		float particleRadius;
		XMFLOAT3 padding;
	};

	struct BUFFER_PERFRAME_GS
	{
		XMMATRIX viewProj;
		XMMATRIX view;
		XMMATRIX proj;
		XMFLOAT3 eyePos;
	};

	BUFFER_PERFRAME_VS mBuffer_PerFrame_VS;
	BUFFER_PERFRAME_GS mBuffer_PerFrame_GS;
};

class BitonicSortShader : public IShader
{
public:
	BitonicSortShader();
	~BitonicSortShader();

	bool Init(ID3D11Device* device, ID3D11ComputeShader* computeShader);
	void SetActive(ID3D11DeviceContext* dc);

	void UpdatePerFrame(ID3D11DeviceContext* dc);
	void SetBuffers(ID3D11DeviceContext* dc);

	void SetLevelProperties(UINT iLevel, UINT iLevelMask);

private:
	struct BUFFER_PERFRAME_CS
	{
		unsigned int g_iLevel;
		unsigned int g_iLevelMask;
		XMFLOAT2 padding;
	};

	BUFFER_PERFRAME_CS mBuffer_PerFrame_CS;
};

class MatrixTransposeShader : public IShader
{
public:
	MatrixTransposeShader();
	~MatrixTransposeShader();

	bool Init(ID3D11Device* device, ID3D11ComputeShader* computeShader);
	void SetActive(ID3D11DeviceContext* dc);

	void UpdatePerFrame(ID3D11DeviceContext* dc);
	void SetBuffers(ID3D11DeviceContext* dc);

	void SetMatrixProperties(UINT matrixWidth, UINT matrixHeight);

private:
	struct BUFFER_PERFRAME_CS
	{
		unsigned int g_iWidth;
		unsigned int g_iHeight;
		XMFLOAT2 padding;
	};

	BUFFER_PERFRAME_CS mBuffer_PerFrame_CS;
};

class GridIndicesShader : public IShader
{
public:
	GridIndicesShader();
	~GridIndicesShader();

	bool Init(ID3D11Device* device, ID3D11ComputeShader* gridBuildShader, ID3D11ComputeShader* gridClearShader);
	
	void BuildGridIndices(ID3D11DeviceContext* dc, ID3D11UnorderedAccessView* gridIndicesUAV, ID3D11ShaderResourceView* gridSRV,
		UINT threadGroupCountX, UINT threadGroupCountY, UINT threadGroupCountZ);
	void ClearGridIndices(ID3D11DeviceContext* dc, ID3D11UnorderedAccessView* gridIndicesUAV,
		UINT threadGroupCountX, UINT threadGroupCountY, UINT threadGroupCountZ);

	void SetNumElements(ID3D11DeviceContext* dc, UINT numElements);

private:
	ID3D11ComputeShader* mBuildShader;
	ID3D11ComputeShader* mClearShader;

	struct BUFFER_PERFRAME_BUILD_CS
	{
		UINT g_iNumElements;
		XMFLOAT3 padding;
	};

	BUFFER_PERFRAME_BUILD_CS mBufferVars_PerFrame_Build_CS;
	ID3D11Buffer* mBuffer_PerFrame_Build_CS;
};

#pragma region ShaderHandler
enum ShaderType
{
	VERTEXSHADER = 0,
	PIXELSHADER,
	GEOMETRYSHADER,
	GEOMETRYSTREAMOUTSHADER,
	COMPUTESHADER
};

struct Shader
{
	std::string Name;
	ID3DBlob* Buffer;
	UINT Type;
};

// Loads compiled shader objects
class ShaderHandler
{
public:
	ShaderHandler();
	~ShaderHandler();

	bool Init();

	void LoadCompiledVertexShader(LPCWSTR fileName, char* name, ID3D11Device* device);
	void LoadCompiledPixelShader(LPCWSTR fileName, char* name, ID3D11Device* device);
	void LoadCompiledGeometryShader(LPCWSTR fileName, char* name, ID3D11Device* device);
	void LoadCompiledGeometryStreamOutShader(LPCWSTR fileName, char* name, ID3D11Device* device, D3D11_SO_DECLARATION_ENTRY pDecl[], UINT pDeclSize, UINT rasterizedStream = 0, UINT* bufferStrides = NULL, UINT numStrides = 0);
	void LoadCompiledComputeShader(LPCWSTR fileName, char* name, ID3D11Device* device);

	ID3D11VertexShader* GetVertexShader(std::string name);
	ID3D11PixelShader* GetPixelShader(std::string name);
	ID3D11GeometryShader* GetGeometryShader(std::string name);
	ID3D11ComputeShader* GetComputeShader(std::string name);

	Shader* GetShader(std::string name);

	BasicDeferredShader* mBasicDeferredShader;
	LightDeferredShader* mLightDeferredShader;
	SkyDeferredShader* mSkyDeferredShader;
	LightDeferredShader* mLightDeferredToTextureShader;
	ParticleDrawShader* mParticleDrawShader;

	BitonicSortShader* mBitonicSortShader;
	MatrixTransposeShader* mMatrixTransposeShader;
	GridIndicesShader* mGridIndicesShader;

private:
	BYTE* LoadByteCode(char* fileName, UINT& size);

	std::map<std::string, ID3D11VertexShader*> mVertexShaders;
	std::map<std::string, ID3D11PixelShader*> mPixelShaders;
	std::map<std::string, ID3D11GeometryShader*> mGeometryShaders;
	std::map<std::string, ID3D11ComputeShader*> mComputeShaders;

	std::vector<Shader*> mShaders;
};
#pragma endregion ShaderHandler

#endif