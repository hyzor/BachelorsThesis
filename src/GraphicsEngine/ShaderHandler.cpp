#include "ShaderHandler.h"

// Must be included last!
//#include "common/debug.h"

#pragma region ShaderHandler
ShaderHandler::ShaderHandler(){}

ShaderHandler::~ShaderHandler()
{
	for (UINT i = 0; i < mShaders.size(); ++i)
	{
		ReleaseCOM(mShaders[i]->Buffer);
		delete mShaders[i];
	}
	mShaders.clear();

	for (auto& it(mVertexShaders.begin()); it != mVertexShaders.end(); ++it)
	{
		if (it->second)
			ReleaseCOM(it->second);
	}
	mVertexShaders.clear();

	for (auto& it(mPixelShaders.begin()); it != mPixelShaders.end(); ++it)
	{
		if (it->second)
			ReleaseCOM(it->second);
	}
	mPixelShaders.clear();

	for (auto& it(mGeometryShaders.begin()); it != mGeometryShaders.end(); ++it)
	{
		if (it->second)
			ReleaseCOM(it->second);
	}
	mGeometryShaders.clear();

	for (auto& it(mComputeShaders.begin()); it != mComputeShaders.end(); ++it)
	{
		if (it->second)
			ReleaseCOM(it->second);
	}
	mComputeShaders.clear();

	delete mBasicDeferredShader;
	delete mLightDeferredShader;
	delete mSkyDeferredShader;
	delete mLightDeferredToTextureShader;
	delete mParticleDrawShader;

	delete mBitonicSortShader;
	delete mMatrixTransposeShader;
	delete mGridIndicesShader;
}

void ShaderHandler::LoadCompiledVertexShader(LPCWSTR fileName, char* name, ID3D11Device* device)
{
	HRESULT hr;

	Shader* shader = new Shader();
	shader->Name = name;
	shader->Type = VERTEXSHADER;
	hr = D3DReadFileToBlob(fileName, &shader->Buffer);

	if (FAILED(hr))
	{
		std::wstringstream ErrorMsg;
		ErrorMsg << "Failed to load vertex shader " << fileName;
		MessageBox(0, ErrorMsg.str().c_str(), 0, 0);
	}

	mShaders.push_back(shader);

	// Create the actual shader
	ID3D11VertexShader* vShader;
	hr = device->CreateVertexShader(mShaders.back()->Buffer->GetBufferPointer(),
		mShaders.back()->Buffer->GetBufferSize(), nullptr, &vShader);
	mVertexShaders[name] = vShader;
}

void ShaderHandler::LoadCompiledPixelShader(LPCWSTR fileName, char* name, ID3D11Device* device)
{
	HRESULT hr;

	Shader* shader = new Shader();
	shader->Name = name;
	shader->Type = PIXELSHADER;
	hr = D3DReadFileToBlob(fileName, &shader->Buffer);

	if (FAILED(hr))
	{
		std::wstringstream ErrorMsg;
		ErrorMsg << "Failed to load pixel shader " << fileName;
		MessageBox(0, ErrorMsg.str().c_str(), 0, 0);
	}

	mShaders.push_back(shader);

	// Create the actual shader
	ID3D11PixelShader* pShader;
	hr = device->CreatePixelShader(mShaders.back()->Buffer->GetBufferPointer(),
		mShaders.back()->Buffer->GetBufferSize(), nullptr, &pShader);
	mPixelShaders[name] = pShader;
}

void ShaderHandler::LoadCompiledGeometryShader(LPCWSTR fileName, char* name, ID3D11Device* device)
{
	HRESULT hr;

	Shader* shader = new Shader();
	shader->Name = name;
	shader->Type = GEOMETRYSHADER;
	hr = D3DReadFileToBlob(fileName, &shader->Buffer);

	if (FAILED(hr))
	{
		std::wstringstream ErrorMsg;
		ErrorMsg << "Failed to load geometry shader " << fileName;
		MessageBox(0, ErrorMsg.str().c_str(), 0, 0);
	}

	mShaders.push_back(shader);

	// Create the actual shader
	ID3D11GeometryShader* gShader;
	hr = device->CreateGeometryShader(mShaders.back()->Buffer->GetBufferPointer(),
		mShaders.back()->Buffer->GetBufferSize(), nullptr, &gShader);
	mGeometryShaders[name] = gShader;
}

void ShaderHandler::LoadCompiledGeometryStreamOutShader(LPCWSTR fileName, char* name, ID3D11Device* device, D3D11_SO_DECLARATION_ENTRY pDecl[], UINT pDeclSize, UINT rasterizedStream, UINT* bufferStrides, UINT numStrides)
{
	HRESULT hr;

	Shader* shader = new Shader();
	shader->Name = name;
	shader->Type = GEOMETRYSTREAMOUTSHADER;
	hr = D3DReadFileToBlob(fileName, &shader->Buffer);

	if (FAILED(hr))
	{
		std::wstringstream ErrorMsg;
		ErrorMsg << "Failed to load geometry stream out shader " << fileName;
		MessageBox(0, ErrorMsg.str().c_str(), 0, 0);
	}

	mShaders.push_back(shader);

	// Create the actual shader
	ID3D11GeometryShader* gShader;

	hr = device->CreateGeometryShaderWithStreamOutput(
		mShaders.back()->Buffer->GetBufferPointer(),
		mShaders.back()->Buffer->GetBufferSize(),
		pDecl,
		pDeclSize,
		bufferStrides, // sizeof(Vertex::Particle)
		numStrides, // One stride
		rasterizedStream,
		NULL,
		&gShader);

	mGeometryShaders[name] = gShader;
}

void ShaderHandler::LoadCompiledComputeShader(LPCWSTR fileName, char* name, ID3D11Device* device)
{
	HRESULT hr;

	Shader* shader = new Shader();
	shader->Name = name;
	shader->Type = COMPUTESHADER;
	hr = D3DReadFileToBlob(fileName, &shader->Buffer);

	if (FAILED(hr))
	{
		std::wstringstream ErrorMsg;
		ErrorMsg << "Failed to load compute shader " << fileName;
		MessageBox(0, ErrorMsg.str().c_str(), 0, 0);
	}

	mShaders.push_back(shader);

	// Create the actual shader
	ID3D11ComputeShader* cShader;
	hr = device->CreateComputeShader(mShaders.back()->Buffer->GetBufferPointer(),
		mShaders.back()->Buffer->GetBufferSize(), nullptr, &cShader);
	mComputeShaders[name] = cShader;
}

BYTE* ShaderHandler::LoadByteCode(char* fileName, UINT& size)
{
	FILE* file = 0;
	unsigned char* shaderCode = 0;
	UINT fileSize = 0;
	UINT numRead = 0;

	fopen_s(&file, fileName, "rb");

	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	shaderCode = (unsigned char*)malloc(fileSize);

	while (numRead != fileSize)
		numRead = (UINT)fread(&shaderCode[numRead], 1, fileSize, file);

	fclose(file);

	size = fileSize;

	return shaderCode;
}

ID3D11VertexShader* ShaderHandler::GetVertexShader(std::string name)
{
	if (mVertexShaders[name])
		return mVertexShaders[name];
	else
	{
		std::wstringstream ErrorMsg;
		ErrorMsg << "Failed to get vertex shader " << name.c_str();
		MessageBox(0, ErrorMsg.str().c_str(), 0, 0);
		return NULL;
	}
}

ID3D11PixelShader* ShaderHandler::GetPixelShader(std::string name)
{
	if (mPixelShaders[name])
		return mPixelShaders[name];
	else
	{
		std::wstringstream ErrorMsg;
		ErrorMsg << "Failed to get pixel shader " << name.c_str();
		MessageBox(0, ErrorMsg.str().c_str(), 0, 0);
		return NULL;
	}
}

ID3D11GeometryShader* ShaderHandler::GetGeometryShader(std::string name)
{
	if (mGeometryShaders[name])
		return mGeometryShaders[name];
	else
	{
		std::wstringstream ErrorMsg;
		ErrorMsg << "Failed to get geometry shader " << name.c_str();
		MessageBox(0, ErrorMsg.str().c_str(), 0, 0);
		return NULL;
	}
}

ID3D11ComputeShader* ShaderHandler::GetComputeShader(std::string name)
{
	if (mComputeShaders[name])
		return mComputeShaders[name];
	else
	{
		std::wstringstream ErrorMsg;
		ErrorMsg << "Failed to get compute shader " << name.c_str();
		MessageBox(0, ErrorMsg.str().c_str(), 0, 0);
		return NULL;
	}
}



Shader* ShaderHandler::GetShader(std::string name)
{
	for (UINT i = 0; i < mShaders.size(); ++i)
	{
		if (mShaders[i]->Name == name)
		{
			return mShaders[i];
		}
	}

	std::wstringstream ErrorMsg;
	ErrorMsg << "Failed to get shader " << name.c_str();
	MessageBox(0, ErrorMsg.str().c_str(), 0, 0);
	return NULL;
}

bool ShaderHandler::Init()
{
	mBasicDeferredShader = new BasicDeferredShader();
	mLightDeferredShader = new LightDeferredShader();
	mSkyDeferredShader = new SkyDeferredShader();
	mLightDeferredToTextureShader = new LightDeferredShader();
	mParticleDrawShader = new ParticleDrawShader();

	mBitonicSortShader = new BitonicSortShader();
	mMatrixTransposeShader = new MatrixTransposeShader();
	mGridIndicesShader = new GridIndicesShader();

	return true;
}
#pragma endregion ShaderHandler

IShader::IShader(){}

IShader::~IShader()
{
	ReleaseCOM(mVertexShaderWrapper.PerFrameBuffer);
	ReleaseCOM(mVertexShaderWrapper.PerObjBuffer);
	ReleaseCOM(mPixelShaderWrapper.PerFrameBuffer);
	ReleaseCOM(mPixelShaderWrapper.PerObjBuffer);
	ReleaseCOM(mGeometryShaderWrapper.PerFrameBuffer);
	ReleaseCOM(mGeometryShaderWrapper.PerObjBuffer);
	ReleaseCOM(mComputeShaderWrapper.PerFrameBuffer);
	ReleaseCOM(mComputeShaderWrapper.PerObjBuffer);
}

bool IShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	ZeroMemory(&mVertexShaderWrapper, sizeof(mVertexShaderWrapper));
	ZeroMemory(&mPixelShaderWrapper, sizeof(mPixelShaderWrapper));
	ZeroMemory(&mGeometryShaderWrapper, sizeof(mGeometryShaderWrapper));
	ZeroMemory(&mComputeShaderWrapper, sizeof(mComputeShaderWrapper));

	mInputLayout = inputLayout;

	return true;
}

bool IShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout,
	ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader)
{
	Init(device, inputLayout);

	mVertexShaderWrapper.VertexShader = vertexShader;
	mPixelShaderWrapper.PixelShader = pixelShader;

	return true;
}

bool IShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout,
	ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11GeometryShader* geometryShader)
{
	Init(device, inputLayout);

	mVertexShaderWrapper.VertexShader = vertexShader;
	mPixelShaderWrapper.PixelShader = pixelShader;
	mGeometryShaderWrapper.GeometryShader = geometryShader;

	return true;
}

bool IShader::Init(ID3D11Device* device, ID3D11ComputeShader* computeShader)
{
	ID3D11InputLayout* layout = nullptr;
	Init(device, layout);

	mComputeShaderWrapper.ComputeShader = computeShader;

	return true;
}

void IShader::SetActive(ID3D11DeviceContext* dc)
{
	if (mInputLayout)
		dc->IASetInputLayout(mInputLayout);
	else
		dc->IASetInputLayout(nullptr);

	ID3D11ShaderResourceView* nullSRV[16] = { NULL };

	if (mVertexShaderWrapper.VertexShader)
	{
		dc->VSSetShader(mVertexShaderWrapper.VertexShader, nullptr, 0);
		dc->VSSetShaderResources(0, 16, nullSRV);
	}
	else
		dc->VSSetShader(nullptr, nullptr, 0);

	if (mPixelShaderWrapper.PixelShader)
	{
		dc->PSSetShader(mPixelShaderWrapper.PixelShader, nullptr, 0);
		dc->PSSetShaderResources(0, 16, nullSRV);
	}
	else
		dc->PSSetShader(nullptr, nullptr, 0);

	if (mGeometryShaderWrapper.GeometryShader)
	{
		dc->GSSetShader(mGeometryShaderWrapper.GeometryShader, nullptr, 0);
		dc->GSSetShaderResources(0, 16, nullSRV);
	}
	else
		dc->GSSetShader(nullptr, nullptr, 0);

	if (mComputeShaderWrapper.ComputeShader)
	{
		dc->CSSetShader(mComputeShaderWrapper.ComputeShader, nullptr, 0);
		dc->CSSetShaderResources(0, 16, nullSRV);
	}
	else
		dc->CSSetShader(nullptr, nullptr, 0);
}

void IShader::UpdatePerFrame(ID3D11DeviceContext* dc){}

void IShader::UpdatePerObj(ID3D11DeviceContext* dc){}

const ID3D11VertexShader* IShader::GetVertexShader()
{
	if (mVertexShaderWrapper.VertexShader)
		return mVertexShaderWrapper.VertexShader;

	return nullptr;
}

const ID3D11PixelShader* IShader::GetPixelShader()
{
	if (mPixelShaderWrapper.PixelShader)
		return mPixelShaderWrapper.PixelShader;

	return nullptr;
}

const ID3D11GeometryShader* IShader::GetGeometryShader()
{
	if (mGeometryShaderWrapper.GeometryShader)
		return mGeometryShaderWrapper.GeometryShader;

	return nullptr;
}

const ID3D11ComputeShader* IShader::GetComputeShader()
{
	if (mComputeShaderWrapper.ComputeShader)
		return mComputeShaderWrapper.ComputeShader;

	return nullptr;
}

BasicDeferredShader::BasicDeferredShader()
	: IShader(){}

BasicDeferredShader::~BasicDeferredShader(){}

void BasicDeferredShader::SetActive(ID3D11DeviceContext* dc)
{
	IShader::SetActive(dc);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearSS);
	dc->PSSetSamplers(1, 1, &RenderStates::mAnisotropicSS);
	dc->PSSetSamplers(2, 1, &RenderStates::mComparisonSS);
}

void BasicDeferredShader::SetWorldViewProjTex(XMMATRIX& world, XMMATRIX& viewProj, XMMATRIX& tex)
{
	mBuffer_PerObj_VS.world = XMMatrixTranspose(world);
	mBuffer_PerObj_VS.worldViewProj = XMMatrixTranspose(XMMatrixMultiply(world, viewProj));
	mBuffer_PerObj_VS.worldInvTranspose = XMMatrixTranspose(DirectXUtil::InverseTranspose(world));
	mBuffer_PerObj_VS.texTransform = XMMatrixScaling(1.0f, 1.0f, 1.0f);
}

void BasicDeferredShader::SetMaterial(const Material& mat, UINT globalMaterialIndex)
{
	mBuffer_PerObj_PS.mat = mat;
	mBuffer_PerObj_PS.globalMaterialIndex = globalMaterialIndex;
}

void BasicDeferredShader::SetDiffuseMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(0, 1, &tex);
}

void BasicDeferredShader::UpdatePerObj(ID3D11DeviceContext* dc)
{
	IShader::UpdateBuffer(dc, mVertexShaderWrapper.PerObjBuffer, &mBuffer_PerObj_VS);
	dc->PSSetConstantBuffers(0, 1, &mVertexShaderWrapper.PerObjBuffer);
}

bool BasicDeferredShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout,
	ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader)
{
	IShader::Init(device, inputLayout, vertexShader, pixelShader);
	IShader::CreateBuffer(device, &mVertexShaderWrapper.PerObjBuffer, &mBuffer_PerObj_VS);
	IShader::CreateBuffer(device, &mPixelShaderWrapper.PerObjBuffer, &mBuffer_PerObj_PS);

	return true;
}

void LightDeferredShader::SetPointLights(ID3D11DeviceContext* dc, UINT numPointLights, PointLight* PLights[])
{
	mBuffer_PerFrame_PS.numPLights = numPointLights;

	for (UINT i = 0; i < numPointLights; ++i)
		mBuffer_PerFrame_PS.pointLights[i] = *PLights[i];
}

void LightDeferredShader::SetDirLights(ID3D11DeviceContext* dc, UINT numDirLights, DirLight* dirLights[])
{
	mBuffer_PerFrame_PS.numDirLights = numDirLights;

	for (UINT i = 0; i < numDirLights; ++i)
		mBuffer_PerFrame_PS.dirLights[i] = *dirLights[i];
}

void LightDeferredShader::SetSpotLights(ID3D11DeviceContext* dc, UINT numSpotLights, SpotLight* SLights[])
{
	mBuffer_PerFrame_PS.numSLights = numSpotLights;

	for (UINT i = 0; i < numSpotLights; ++i)
		mBuffer_PerFrame_PS.spotLights[i] = *SLights[i];
}

void LightDeferredShader::SetMaterials(ID3D11DeviceContext* dc, UINT numMaterials, Material* mats[])
{
	for (UINT i = 0; i < numMaterials; ++i)
	{
		mBuffer_PerFrame_PS.materials[i] = *mats[i];
	}
}

LightDeferredShader::LightDeferredShader()
	: IShader(){}

LightDeferredShader::~LightDeferredShader(){}

void LightDeferredShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout,
	ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader)
{
	IShader::Init(device, inputLayout, vertexShader, pixelShader);
	IShader::CreateBuffer(device, &mVertexShaderWrapper.PerObjBuffer, &mBuffer_PerObj_VS);
	IShader::CreateBuffer(device, &mPixelShaderWrapper.PerFrameBuffer, &mBuffer_PerFrame_PS);
}

void LightDeferredShader::SetActive(ID3D11DeviceContext* dc)
{
	IShader::SetActive(dc);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearSS);
	dc->PSSetSamplers(1, 1, &RenderStates::mAnisotropicSS);
	dc->PSSetSamplers(2, 1, &RenderStates::mComparisonSS);
	dc->PSSetSamplers(3, 1, &RenderStates::mPointClampedSS);
}

void LightDeferredShader::SetDiffuseTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(0, 1, &tex);
}

void LightDeferredShader::SetNormalTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(1, 1, &tex);
}

void LightDeferredShader::UpdatePerObj(ID3D11DeviceContext* dc)
{
	IShader::UpdateBuffer(dc, mVertexShaderWrapper.PerObjBuffer, &mBuffer_PerObj_VS);
	dc->VSSetConstantBuffers(0, 1, &mVertexShaderWrapper.PerObjBuffer);
}

void LightDeferredShader::SetEyePosW(XMFLOAT3 eyePosW)
{
	mBuffer_PerFrame_PS.gEyePosW = eyePosW;
}

void LightDeferredShader::SetWorldViewProj(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj)
{
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	mBuffer_PerObj_VS.worldViewProj = XMMatrixTranspose(XMMatrixMultiply(world, viewProj));
}

void LightDeferredShader::UpdatePerFrame(ID3D11DeviceContext* dc)
{
	IShader::UpdateBuffer(dc, mPixelShaderWrapper.PerFrameBuffer, &mBuffer_PerFrame_PS);

	dc->PSSetConstantBuffers(0, 1, &mPixelShaderWrapper.PerFrameBuffer);
}

void LightDeferredShader::SetCameraViewProjMatrix(XMMATRIX& camViewMatrix, XMMATRIX& proj)
{
	XMMATRIX viewProj = XMMatrixMultiply(camViewMatrix, proj);
	XMMATRIX viewProjInv = XMMatrixInverse(nullptr, viewProj);

	mBuffer_PerFrame_PS.camViewProjInv = XMMatrixTranspose(viewProjInv);
}

void LightDeferredShader::SetDepthTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(2, 1, &tex);
}

void LightDeferredShader::SetSkipLighting(bool skipLighting)
{
	mBuffer_PerFrame_PS.skipLighting = skipLighting;
}

void LightDeferredShader::SetSkipProcessing(bool skipProcessing)
{
	mBuffer_PerFrame_PS.skipProcessing = skipProcessing;
}

SkyDeferredShader::SkyDeferredShader()
	: IShader(){}

SkyDeferredShader::~SkyDeferredShader(){}

bool SkyDeferredShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader)
{
	IShader::Init(device, inputLayout, vertexShader, pixelShader);
	IShader::CreateBuffer(device, &mVertexShaderWrapper.PerFrameBuffer, &mBuffer_PerFrame_VS);

	return true;
}

void SkyDeferredShader::SetActive(ID3D11DeviceContext* dc)
{
	IShader::SetActive(dc);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearSS);
}

void SkyDeferredShader::SetWorldViewProj(const XMMATRIX& worldViewProj)
{
	mBuffer_PerFrame_VS.worldViewProj = XMMatrixTranspose(worldViewProj);
}

void SkyDeferredShader::SetCubeMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* cubeMap)
{
	dc->PSSetShaderResources(0, 1, &cubeMap);
}

void SkyDeferredShader::UpdatePerFrame(ID3D11DeviceContext* dc)
{
	IShader::UpdateBuffer(dc, mVertexShaderWrapper.PerFrameBuffer, &mBuffer_PerFrame_VS);
	dc->VSSetConstantBuffers(0, 1, &mVertexShaderWrapper.PerFrameBuffer);
}

ParticleDrawShader::ParticleDrawShader()
	: IShader()
{}

ParticleDrawShader::~ParticleDrawShader(){}

bool ParticleDrawShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout, ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader)
{
	IShader::Init(device, inputLayout, vertexShader, pixelShader);
	
	if (FAILED(IShader::CreateBuffer(device, &mVertexShaderWrapper.PerFrameBuffer, &mBuffer_PerFrame_VS)))
		return false;

	return true;
}

bool ParticleDrawShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout,
	ID3D11VertexShader* vertexShader, ID3D11PixelShader* pixelShader, ID3D11GeometryShader* geometryShader)
{
	IShader::Init(device, inputLayout, vertexShader, pixelShader, geometryShader);

	if (FAILED(IShader::CreateBuffer(device, &mVertexShaderWrapper.PerFrameBuffer, &mBuffer_PerFrame_VS)))
		return false;

	if (FAILED(IShader::CreateBuffer(device, &mGeometryShaderWrapper.PerFrameBuffer, &mBuffer_PerFrame_GS)))
		return false;

	return true;
}

void ParticleDrawShader::SetViewProj(XMMATRIX& viewProj, XMMATRIX& view, XMMATRIX& proj)
{
	mBuffer_PerFrame_VS.viewProj = XMMatrixTranspose(viewProj);
	mBuffer_PerFrame_GS.viewProj = XMMatrixTranspose(viewProj);
	mBuffer_PerFrame_GS.view = XMMatrixTranspose(view);
	mBuffer_PerFrame_GS.proj = XMMatrixTranspose(proj);
}

void ParticleDrawShader::UpdatePerFrame(ID3D11DeviceContext* dc)
{
	IShader::UpdateBuffer(dc, mVertexShaderWrapper.PerFrameBuffer, &mBuffer_PerFrame_VS);
	dc->VSSetConstantBuffers(0, 1, &mVertexShaderWrapper.PerFrameBuffer);

	IShader::UpdateBuffer(dc, mGeometryShaderWrapper.PerFrameBuffer, &mBuffer_PerFrame_GS);
	dc->GSSetConstantBuffers(0, 1, &mGeometryShaderWrapper.PerFrameBuffer);
}

void ParticleDrawShader::SetActive(ID3D11DeviceContext* dc)
{
	IShader::SetActive(dc);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearSS);
}

void ParticleDrawShader::SetEyePos(XMFLOAT3 eyePos)
{
	mBuffer_PerFrame_GS.eyePos = eyePos;
}

void ParticleDrawShader::SetDiffuseTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(0, 1, &tex);
}

void ParticleDrawShader::SetParticleRadius(float particleRadius)
{
	mBuffer_PerFrame_VS.particleRadius = particleRadius;
}

BitonicSortShader::BitonicSortShader()
	: IShader()
{}

BitonicSortShader::~BitonicSortShader(){}

bool BitonicSortShader::Init(ID3D11Device* device, ID3D11ComputeShader* computeShader)
{
	IShader::Init(device, computeShader);

	if (FAILED(IShader::CreateBuffer(device, &mComputeShaderWrapper.PerFrameBuffer, &mBuffer_PerFrame_CS)))
		return false;

	return true;
}

void BitonicSortShader::SetActive(ID3D11DeviceContext* dc)
{
	IShader::SetActive(dc);
}

void BitonicSortShader::UpdatePerFrame(ID3D11DeviceContext* dc)
{
	IShader::UpdateBuffer(dc, mComputeShaderWrapper.PerFrameBuffer, &mBuffer_PerFrame_CS);
	dc->CSSetConstantBuffers(0, 1, &mComputeShaderWrapper.PerFrameBuffer);
}

void BitonicSortShader::SetLevelProperties(UINT iLevel, UINT iLevelMask)
{
	mBuffer_PerFrame_CS.g_iLevel = iLevel;
	mBuffer_PerFrame_CS.g_iLevelMask = iLevelMask;
}

void BitonicSortShader::SetBuffers(ID3D11DeviceContext* dc)
{
	dc->CSSetConstantBuffers(0, 1, &mComputeShaderWrapper.PerFrameBuffer);
}

void MatrixTransposeShader::SetMatrixProperties(UINT matrixWidth, UINT matrixHeight)
{
	mBuffer_PerFrame_CS.g_iWidth = matrixWidth;
	mBuffer_PerFrame_CS.g_iHeight = matrixHeight;
}

MatrixTransposeShader::MatrixTransposeShader()
	: IShader()
{}

MatrixTransposeShader::~MatrixTransposeShader(){}

bool MatrixTransposeShader::Init(ID3D11Device* device, ID3D11ComputeShader* computeShader)
{
	IShader::Init(device, computeShader);

	if (FAILED(IShader::CreateBuffer(device, &mComputeShaderWrapper.PerFrameBuffer, &mBuffer_PerFrame_CS)))
		return false;

	return true;
}

void MatrixTransposeShader::SetActive(ID3D11DeviceContext* dc)
{
	IShader::SetActive(dc);
}

void MatrixTransposeShader::UpdatePerFrame(ID3D11DeviceContext* dc)
{
	IShader::UpdateBuffer(dc, mComputeShaderWrapper.PerFrameBuffer, &mBuffer_PerFrame_CS);
	dc->CSSetConstantBuffers(0, 1, &mComputeShaderWrapper.PerFrameBuffer);
}

void MatrixTransposeShader::SetBuffers(ID3D11DeviceContext* dc)
{
	dc->CSSetConstantBuffers(0, 1, &mComputeShaderWrapper.PerFrameBuffer);
}

GridIndicesShader::GridIndicesShader()
	: IShader()
{

}

GridIndicesShader::~GridIndicesShader()
{
	ReleaseCOM(mBuffer_PerFrame_Build_CS);
}

bool GridIndicesShader::Init(ID3D11Device* device, ID3D11ComputeShader* gridBuildShader, ID3D11ComputeShader* gridClearShader)
{
	ID3D11InputLayout* inputLayout = nullptr;

	IShader::Init(device, inputLayout);

	mBuildShader = gridBuildShader;
	mClearShader = gridClearShader;

	ZeroMemory(&mBufferVars_PerFrame_Build_CS, sizeof(mBufferVars_PerFrame_Build_CS));

	if (FAILED(IShader::CreateBuffer(device, &mBuffer_PerFrame_Build_CS, &mBufferVars_PerFrame_Build_CS)))
		return false;

	return true;
}

void GridIndicesShader::BuildGridIndices(ID3D11DeviceContext* dc,
	ID3D11UnorderedAccessView* gridIndicesUAV, ID3D11ShaderResourceView* gridSRV,
	UINT threadGroupCountX, UINT threadGroupCountY, UINT threadGroupCountZ)
{
	UINT UAVInitialCounts = 0;
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;

	dc->CSSetUnorderedAccessViews(0, 1, &gridIndicesUAV, &UAVInitialCounts);
	dc->CSSetShaderResources(0, 1, &gridSRV);
	dc->CSSetShader(mBuildShader, nullptr, 0);
	dc->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
	dc->CSSetUnorderedAccessViews(0, 1, &nullUAV, &UAVInitialCounts);
	dc->CSSetShaderResources(0, 1, &nullSRV);
}

void GridIndicesShader::ClearGridIndices(ID3D11DeviceContext* dc, ID3D11UnorderedAccessView* gridIndicesUAV,
	UINT threadGroupCountX, UINT threadGroupCountY, UINT threadGroupCountZ)
{
	UINT UAVInitialCounts = 0;
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11UnorderedAccessView* nullUAV = nullptr;

	dc->CSSetConstantBuffers(0, 1, &mBuffer_PerFrame_Build_CS);
	dc->CSSetUnorderedAccessViews(0, 1, &gridIndicesUAV, &UAVInitialCounts);
	dc->CSSetShader(mClearShader, nullptr, 0);
	dc->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
	dc->CSSetUnorderedAccessViews(0, 1, &nullUAV, &UAVInitialCounts);
}

void GridIndicesShader::SetNumElements(ID3D11DeviceContext* dc, UINT numElements)
{
	mBufferVars_PerFrame_Build_CS.g_iNumElements = numElements;
	IShader::UpdateBuffer(dc, mBuffer_PerFrame_Build_CS, &mBufferVars_PerFrame_Build_CS);
}
