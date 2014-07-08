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

	virtual bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout) = 0;
	virtual bool SetActive(ID3D11DeviceContext* dc) = 0;

	const ID3D11VertexShader* GetVertexShader() { return mVertexShader; }
	const ID3D11PixelShader* GetPixelShader() { return mPixelShader; }

	virtual void Update(ID3D11DeviceContext* dc) = 0;

protected:
	ID3D11VertexShader* mVertexShader;
	ID3D11PixelShader* mPixelShader;
	ID3D11InputLayout* mInputLayout;
};

#pragma region SkyDeferredShader
class SkyDeferredShader : public IShader
{
public:
	SkyDeferredShader();
	~SkyDeferredShader();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool SetActive(ID3D11DeviceContext* dc);

	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);

	void SetWorldViewProj(const XMMATRIX& worldViewProj);
	void SetPrevWorldViewProj(XMMATRIX& prevWorld, XMMATRIX& prevViewProj);
	void SetCubeMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* cubeMap);

	void Update(ID3D11DeviceContext* dc);

private:
	struct VS_CPERFRAMEBUFFER
	{
		XMMATRIX WorldViewProj;
		XMMATRIX prevWorldViewProj;
	};

	struct BUFFERCACHE
	{
		VS_CPERFRAMEBUFFER vsBuffer;
	};

	// VS - per frame
	ID3D11Buffer* vs_cPerFrameBuffer;
	VS_CPERFRAMEBUFFER vs_cPerFrameBufferVariables;

	struct BUFFERCACHE mBufferCache;
};
#pragma endregion SkyDeferredShader

#pragma region BasicDeferredShader
class BasicDeferredShader : public IShader
{
public:
	BasicDeferredShader();
	~BasicDeferredShader();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);
	bool SetActive(ID3D11DeviceContext* dc);

	void SetWorldViewProjTex(XMMATRIX& world,
		XMMATRIX& viewProj,
		XMMATRIX& tex);

	void SetPrevWorldViewProj(XMMATRIX& prevWorld, XMMATRIX& prevViewProj);

	void SetShadowTransformLightViewProj(XMMATRIX& shadowTransform, XMMATRIX& lightView, XMMATRIX& lightProj);

	void SetMaterial(const Material& mat, UINT globalMaterialIndex);
	void SetDiffuseMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetShadowMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetType(int type);

	void UpdatePerObj(ID3D11DeviceContext* dc);

private:
	void Update(ID3D11DeviceContext* dc) { ; }

	struct VS_CPEROBJBUFFER
	{
		XMMATRIX world;
		XMMATRIX worldInvTranspose;
		XMMATRIX worldViewProj;
		XMMATRIX texTransform;
		XMMATRIX prevWorldViewProj;
		XMMATRIX toEyeSpace;
	};
	
	struct PS_CPEROBJBUFFER
	{
		Material mat;

		unsigned int globalMaterialIndex;
		int type;
		XMFLOAT2 padding;

		XMFLOAT4 nearDepths;
		XMFLOAT4 farDepths;
		int nrOfCascades;
		XMFLOAT3 padding1;
	};

	struct BUFFERCACHE
	{
		VS_CPEROBJBUFFER vsPerObjBuffer;
		PS_CPEROBJBUFFER psPerObjBuffer;
	};

	struct BUFFERCACHE mBufferCache;

	// VS - per object
	ID3D11Buffer* vs_cPerObjBuffer;
	VS_CPEROBJBUFFER vs_cPerObjBufferVariables;

	// PS - per object
	ID3D11Buffer* ps_cPerObjBuffer;
	PS_CPEROBJBUFFER ps_cPerObjBufferVariables;
};
#pragma endregion BasicDeferredShader

#pragma region LightDeferredShader
class LightDeferredShader : public IShader
{
public:
	LightDeferredShader();
	~LightDeferredShader();

	bool Init(ID3D11Device* device, ID3D11InputLayout* inputLayout);
	bool BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader);
	bool SetActive(ID3D11DeviceContext* dc);

	void SetWorldViewProj(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj);

	void SetEyePosW(XMFLOAT3 eyePosW);
	void SetCameraViewProjMatrix(XMMATRIX& camViewMatrix, XMMATRIX& proj);
	void SetCameraWorldMatrix(XMMATRIX& camWorldMatrix);
	void SetLightWorldViewProj(XMMATRIX& lightWorld, XMMATRIX& lightView, XMMATRIX& lightProj);

	void SetPLights(ID3D11DeviceContext* dc, UINT numPLights, PointLight PLights[]);
	void SetDirLights(ID3D11DeviceContext* dc, UINT numDirLights, DirLight dirLights[]);
	void SetSLights(ID3D11DeviceContext* dc, UINT numSLights, SpotLight SLights[]);

	void SetMaterials(ID3D11DeviceContext* dc, UINT numMaterials, Material* mats[]);

	void SetDiffuseTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetNormalTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetSpecularTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetSSAOTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetVelocityTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

	void SetBackgroundTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

	void SetDepthTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);

	void SetShadowMapTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex);
	void SetShadowTransform(XMMATRIX& shadowTransform);

	void SetFogProperties(int enableFogging, float heightFalloff, float heightOffset, float globalDensity, XMFLOAT4 fogColor);
	void SetMotionBlurProperties(int enableMotionBlur);
	void SetFpsValues(float curFps, float targetFps);

	void SetSkipLighting(bool skipLighting);
	void SetIsTransparencyPass(bool isTransparencyPass);

	void UpdatePerObj(ID3D11DeviceContext* dc);
	void UpdatePerFrame(ID3D11DeviceContext* dc);

private:
	void Update(ID3D11DeviceContext* dc) { ; }

	struct VS_CPEROBJBUFFER
	{
		XMMATRIX worldViewProj;
		XMMATRIX shadowTransform;
		XMMATRIX lightViewProj;
		XMMATRIX viewProjInv;
	};

	struct PS_CPERFRAMEBUFFER
	{
		PointLight PLights[MAX_POINT_LIGHTS];
		DirLight dirLights[MAX_DIR_LIGHTS];
		SpotLight SLights[MAX_SPOT_LIGHTS];

		UINT numPLights;
		UINT numDirLights;
		UINT numSLights;
		UINT padding;

		// Forms into a 4D vector
		XMFLOAT3 gEyePosW;
		int isTransparencyPass;

		int enableFogging;
		float fogHeightFalloff, fogHeightOffset, fogGlobalDensity;
		XMFLOAT4 fogColor;

		int enableMotionBlur;
		float curFPS;
		float targetFPS;
		int skipLighting;

		XMMATRIX camViewProjInv;

		Material materials[MAX_MATERIALS];
	};

	struct BUFFERCACHE
	{
		VS_CPEROBJBUFFER vsPerObjBuffer;
		PS_CPERFRAMEBUFFER psPerFrameBuffer;
	};

	struct BUFFERCACHE mBufferCache;

	// VS - per object
	ID3D11Buffer* vs_cPerObjBuffer;
	VS_CPEROBJBUFFER vs_cPerObjBufferVariables;

	// PS - per object
	ID3D11Buffer* ps_cPerFrameBuffer;
	PS_CPERFRAMEBUFFER ps_cPerFrameBufferVariables;
};
#pragma endregion LightDeferredShader

#pragma region ShaderHandler
enum ShaderType
{
	VERTEXSHADER = 0,
	PIXELSHADER,
	GEOMETRYSHADER,
	GEOMETRYSTREAMOUTSHADER
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

	void LoadCompiledVertexShader(LPCWSTR fileName, char* name, ID3D11Device* device);
	void LoadCompiledPixelShader(LPCWSTR fileName, char* name, ID3D11Device* device);
	void LoadCompiledGeometryShader(LPCWSTR fileName, char* name, ID3D11Device* device);
	void LoadCompiledGeometryStreamOutShader(LPCWSTR fileName, char* name, ID3D11Device* device, D3D11_SO_DECLARATION_ENTRY pDecl[], UINT pDeclSize, UINT rasterizedStream = 0, UINT* bufferStrides = NULL, UINT numStrides = 0);

	ID3D11VertexShader* GetVertexShader(std::string name);
	ID3D11PixelShader* GetPixelShader(std::string name);
	ID3D11GeometryShader* GetGeometryShader(std::string name);

	Shader* GetShader(std::string name);

	BasicDeferredShader* mBasicDeferredShader;
	LightDeferredShader* mLightDeferredShader;
	SkyDeferredShader* mSkyDeferredShader;
	LightDeferredShader* mLightDeferredToTextureShader;

private:

	BYTE* LoadByteCode(char* fileName, UINT& size);

	std::map<std::string, ID3D11VertexShader*> mVertexShaders;
	std::map<std::string, ID3D11PixelShader*> mPixelShaders;
	std::map<std::string, ID3D11GeometryShader*> mGeometryShaders;

	std::vector<Shader*> mShaders;
};
#pragma endregion ShaderHandler

#endif