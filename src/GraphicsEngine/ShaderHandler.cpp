#include "ShaderHandler.h"

// Must be included last!
//#include "shared/debug.h"

#pragma region ShaderHandler
ShaderHandler::ShaderHandler()
{
	mBasicDeferredShader = new BasicDeferredShader();
	mLightDeferredShader = new LightDeferredShader();
	mSkyDeferredShader = new SkyDeferredShader();
	mLightDeferredToTextureShader = new LightDeferredShader();
}

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

	delete mBasicDeferredShader;
	delete mLightDeferredShader;
	delete mSkyDeferredShader;
	delete mLightDeferredToTextureShader;
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
// 	hr = device->CreateGeometryShader(mShaders.back()->Buffer->GetBufferPointer(),
// 		mShaders.back()->Buffer->GetBufferSize(), nullptr, &gShader);

// 	mGeometryShaders[name] = gShader;

	hr = device->CreateGeometryShaderWithStreamOutput(
		mShaders.back()->Buffer->GetBufferPointer(),
		mShaders.back()->Buffer->GetBufferSize(),
		/*Stream out declaration*/pDecl,
		pDeclSize,
		bufferStrides, // sizeof(Vertex::Particle)
		numStrides, // One stride
		rasterizedStream,
		NULL,
		&gShader);

	mGeometryShaders[name] = gShader;
}

BYTE* ShaderHandler::LoadByteCode(char* fileName, UINT& size)
{
	FILE* file = 0;
	unsigned char* shaderCode = 0;
	UINT fileSize = 0;
	UINT numRead = 0;

	//file = fopen(fileName, "rb");

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
#pragma endregion ShaderHandler

BasicDeferredShader::BasicDeferredShader()
{

}

BasicDeferredShader::~BasicDeferredShader()
{
// 	if (vs_cPerObjBuffer)
// 		vs_cPerObjBuffer->Release();
// 	if (ps_cPerObjBuffer)
// 		ps_cPerObjBuffer->Release();

	ReleaseCOM(vs_cPerObjBuffer);
	ReleaseCOM(ps_cPerObjBuffer);
}

bool BasicDeferredShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	//------------------------
	// Vertex shader buffers
	//------------------------
	// PER OBJECT BUFFER
	ZeroMemory(&vs_cPerObjBufferVariables, sizeof(VS_CPEROBJBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CPEROBJBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &vs_cPerObjBufferVariables;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	HRESULT res = device->CreateBuffer(&cbDesc, &InitData, &vs_cPerObjBuffer);

	//------------------------
	// Pixel shader buffers
	//------------------------
	// PER OBJECT BUFFER
	ZeroMemory(&ps_cPerObjBufferVariables, sizeof(PS_CPEROBJBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc2;
	cbDesc2.ByteWidth = sizeof(PS_CPEROBJBUFFER);
	cbDesc2.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc2.MiscFlags = 0;
	cbDesc2.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData2;
	InitData2.pSysMem = &ps_cPerObjBufferVariables;
	InitData2.SysMemPitch = 0;
	InitData2.SysMemSlicePitch = 0;

	// Now create the buffer
	res = device->CreateBuffer(&cbDesc2, &InitData2, &ps_cPerObjBuffer);

	mInputLayout = inputLayout;

	return true;
}

bool BasicDeferredShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

bool BasicDeferredShader::SetActive(ID3D11DeviceContext* dc)
{
	// Set vertex layout and primitive topology
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearSS);
	dc->PSSetSamplers(1, 1, &RenderStates::mAnisotropicSS);
	dc->PSSetSamplers(2, 1, &RenderStates::mComparisonSS);

	return true;
}

void BasicDeferredShader::SetType(int type)
{
	mBufferCache.psPerObjBuffer.type = type;
}

void BasicDeferredShader::SetWorldViewProjTex(XMMATRIX& world, XMMATRIX& viewProj, XMMATRIX& tex)
{
	mBufferCache.vsPerObjBuffer.world = XMMatrixTranspose(world);
	mBufferCache.vsPerObjBuffer.worldViewProj = XMMatrixTranspose(XMMatrixMultiply(world, viewProj));
	mBufferCache.vsPerObjBuffer.worldInvTranspose = XMMatrixTranspose(DirectXUtil::InverseTranspose(world));
	mBufferCache.vsPerObjBuffer.texTransform = XMMatrixScaling(1.0f, 1.0f, 1.0f);
}

void BasicDeferredShader::SetMaterial(const Material& mat, UINT globalMaterialIndex)
{
	mBufferCache.psPerObjBuffer.mat = mat;
	mBufferCache.psPerObjBuffer.globalMaterialIndex = globalMaterialIndex;
}

void BasicDeferredShader::SetDiffuseMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(0, 1, &tex);
}

void BasicDeferredShader::UpdatePerObj(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Vertex shader per obj buffer
	dc->Map(vs_cPerObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_CPEROBJBUFFER* dataPtr = (VS_CPEROBJBUFFER*)mappedResource.pData;

// 	dataPtr->world = mBufferCache.vsPerObjBuffer.world;
// 	dataPtr->worldViewProj = mBufferCache.vsPerObjBuffer.worldViewProj;
// 	//dataPtr->worldViewProjTex = mBufferCache.vsBuffer.worldViewProjTex;
// 	dataPtr->worldInvTranspose = mBufferCache.vsPerObjBuffer.worldInvTranspose;
// 	dataPtr->texTransform = mBufferCache.vsPerObjBuffer.texTransform;
	*dataPtr = mBufferCache.vsPerObjBuffer;

	dc->Unmap(vs_cPerObjBuffer, 0);

	dc->VSSetConstantBuffers(0, 1, &vs_cPerObjBuffer);

	// Pixel shader per obj buffer
	dc->Map(ps_cPerObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	PS_CPEROBJBUFFER* dataPtr2 = (PS_CPEROBJBUFFER*)mappedResource.pData;

	//dataPtr2->mat = mBufferCache.psPerObjBuffer.mat;
	*dataPtr2 = mBufferCache.psPerObjBuffer;

	dc->Unmap(ps_cPerObjBuffer, 0);

	dc->PSSetConstantBuffers(0, 1, &ps_cPerObjBuffer);
}

void BasicDeferredShader::SetShadowTransformLightViewProj(XMMATRIX& shadowTransform, XMMATRIX& lightView, XMMATRIX& lightProj)
{
	//mBufferCache.vsPerObjBuffer.shadowTransform = XMMatrixTranspose(shadowTransform);
}

void BasicDeferredShader::SetShadowMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(1, 1, &tex);
}

void BasicDeferredShader::SetPrevWorldViewProj(XMMATRIX& prevWorld, XMMATRIX& prevViewProj)
{
	mBufferCache.vsPerObjBuffer.prevWorldViewProj = XMMatrixTranspose(XMMatrixMultiply(prevWorld, prevViewProj));
}

void LightDeferredShader::SetEyePosW(XMFLOAT3 eyePosW)
{
	mBufferCache.psPerFrameBuffer.gEyePosW = eyePosW;
}

bool LightDeferredShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

void LightDeferredShader::SetPLights(ID3D11DeviceContext* dc, UINT numPLights, PointLight PLights[])
{
	mBufferCache.psPerFrameBuffer.numPLights = numPLights;

	for (UINT i = 0; i < numPLights; ++i)
		mBufferCache.psPerFrameBuffer.PLights[i] = PLights[i];
}

void LightDeferredShader::SetDirLights(ID3D11DeviceContext* dc, UINT numDirLights, DirLight dirLights[])
{
	mBufferCache.psPerFrameBuffer.numDirLights = numDirLights;

	for (UINT i = 0; i < numDirLights; ++i)
		mBufferCache.psPerFrameBuffer.dirLights[i] = dirLights[i];
}

void LightDeferredShader::SetSLights(ID3D11DeviceContext* dc, UINT numSLights, SpotLight SLights[])
{
	mBufferCache.psPerFrameBuffer.numSLights = numSLights;

	for (UINT i = 0; i < numSLights; ++i)
		mBufferCache.psPerFrameBuffer.SLights[i] = SLights[i];
}

LightDeferredShader::LightDeferredShader()
{

}

LightDeferredShader::~LightDeferredShader()
{
// 	if (vs_cPerObjBuffer)
// 		vs_cPerObjBuffer->Release();
// 	if (ps_cPerFrameBuffer)
// 		ps_cPerFrameBuffer->Release();

	ReleaseCOM(vs_cPerObjBuffer);
	ReleaseCOM(ps_cPerFrameBuffer);
}

bool LightDeferredShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	//------------------------
	// Vertex shader buffer
	//------------------------
	ZeroMemory(&mBufferCache.vsPerObjBuffer, sizeof(VS_CPEROBJBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CPEROBJBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &mBufferCache.vsPerObjBuffer;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbDesc, &InitData, &vs_cPerObjBuffer);

	//------------------------
	// Pixel shader buffers
	//------------------------
	// PER FRAME BUFFER
	ZeroMemory(&mBufferCache.psPerFrameBuffer, sizeof(PS_CPERFRAMEBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC ps_cPerFramebDesc;
	ps_cPerFramebDesc.ByteWidth = sizeof(PS_CPERFRAMEBUFFER);
	ps_cPerFramebDesc.Usage = D3D11_USAGE_DYNAMIC;
	ps_cPerFramebDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	ps_cPerFramebDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ps_cPerFramebDesc.MiscFlags = 0;
	ps_cPerFramebDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA PS_InitData2;
	PS_InitData2.pSysMem = &mBufferCache.psPerFrameBuffer;
	PS_InitData2.SysMemPitch = 0;
	PS_InitData2.SysMemSlicePitch = 0;

	device->CreateBuffer(&ps_cPerFramebDesc, &PS_InitData2, &ps_cPerFrameBuffer);

	mInputLayout = inputLayout;

	return true;
}

bool LightDeferredShader::SetActive(ID3D11DeviceContext* dc)
{
	// Set vertex layout and primitive topology
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearSS);
	dc->PSSetSamplers(1, 1, &RenderStates::mAnisotropicSS);
	dc->PSSetSamplers(2, 1, &RenderStates::mComparisonSS);
	dc->PSSetSamplers(3, 1, &RenderStates::mPointClampedSS);

	return true;
}

void LightDeferredShader::SetDiffuseTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(0, 1, &tex);
}

void LightDeferredShader::SetNormalTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(1, 1, &tex);
}

void LightDeferredShader::SetSpecularTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(2, 1, &tex);
}

// void LightDeferredShader::SetPositionTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
// {
// 	dc->PSSetShaderResources(3, 1, &tex);
// }

void LightDeferredShader::SetSSAOTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(4, 1, &tex);
}

void LightDeferredShader::UpdatePerObj(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Vertex shader per obj buffer
	dc->Map(vs_cPerObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_CPEROBJBUFFER* dataPtr = (VS_CPEROBJBUFFER*)mappedResource.pData;

	*dataPtr = mBufferCache.vsPerObjBuffer;

	dc->Unmap(vs_cPerObjBuffer, 0);
	
	dc->VSSetConstantBuffers(0, 1, &vs_cPerObjBuffer);
}

void LightDeferredShader::SetWorldViewProj(XMMATRIX& world, XMMATRIX& view, XMMATRIX& proj)
{
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	mBufferCache.vsPerObjBuffer.worldViewProj = XMMatrixTranspose(XMMatrixMultiply(world, viewProj));
}

void LightDeferredShader::UpdatePerFrame(ID3D11DeviceContext* dc)
{
	// Update constant shader buffers using our cache
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Pixel shader per frame buffer
	dc->Map(ps_cPerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	PS_CPERFRAMEBUFFER* dataPtr3 = (PS_CPERFRAMEBUFFER*)mappedResource.pData;

	*dataPtr3 = mBufferCache.psPerFrameBuffer;

	dc->Unmap(ps_cPerFrameBuffer, 0);

	dc->PSSetConstantBuffers(0, 1, &ps_cPerFrameBuffer);
}

void LightDeferredShader::SetShadowMapTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(6, 1, &tex);
}

void LightDeferredShader::SetShadowTransform(XMMATRIX& shadowTransform)
{
	mBufferCache.vsPerObjBuffer.shadowTransform = XMMatrixTranspose(shadowTransform);
}

void LightDeferredShader::SetCameraViewProjMatrix(XMMATRIX& camViewMatrix, XMMATRIX& proj)
{
	XMMATRIX viewProj = XMMatrixMultiply(camViewMatrix, proj);
	XMMATRIX viewProjInv = XMMatrixInverse(nullptr, viewProj);

	mBufferCache.psPerFrameBuffer.camViewProjInv = XMMatrixTranspose(viewProjInv);

	mBufferCache.vsPerObjBuffer.viewProjInv = XMMatrixTranspose(XMMatrixInverse(nullptr, XMMatrixMultiply(camViewMatrix, proj)));
}

void LightDeferredShader::SetCameraWorldMatrix(XMMATRIX& camWorldMatrix)
{
}

void LightDeferredShader::SetLightWorldViewProj(XMMATRIX& lightWorld, XMMATRIX& lightView, XMMATRIX& lightProj)
{
	mBufferCache.vsPerObjBuffer.lightViewProj = XMMatrixTranspose(XMMatrixMultiply(lightView, lightProj));
}

void LightDeferredShader::SetDepthTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(5, 1, &tex);
}

void LightDeferredShader::SetFogProperties(int enableFogging, float heightFalloff, float heightOffset, float globalDensity, XMFLOAT4 fogColor)
{
	mBufferCache.psPerFrameBuffer.enableFogging = enableFogging;
	mBufferCache.psPerFrameBuffer.fogHeightFalloff = heightFalloff;
	mBufferCache.psPerFrameBuffer.fogHeightOffset = heightOffset;
	mBufferCache.psPerFrameBuffer.fogGlobalDensity = globalDensity;
	mBufferCache.psPerFrameBuffer.fogColor = fogColor;
}

void LightDeferredShader::SetVelocityTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(3, 1, &tex);
}

void LightDeferredShader::SetMotionBlurProperties(int enableMotionBlur)
{
	mBufferCache.psPerFrameBuffer.enableMotionBlur = enableMotionBlur;
}

void LightDeferredShader::SetFpsValues(float curFps, float targetFps)
{
	mBufferCache.psPerFrameBuffer.curFPS = curFps;
	mBufferCache.psPerFrameBuffer.targetFPS = targetFps;
}

void LightDeferredShader::SetBackgroundTexture(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* tex)
{
	dc->PSSetShaderResources(6, 1, &tex);
}

void LightDeferredShader::SetSkipLighting(bool skipLighting)
{
	mBufferCache.psPerFrameBuffer.skipLighting = skipLighting;
}

void LightDeferredShader::SetIsTransparencyPass(bool isTransparencyPass)
{
	mBufferCache.psPerFrameBuffer.isTransparencyPass = isTransparencyPass;
}

void LightDeferredShader::SetMaterials(ID3D11DeviceContext* dc, UINT numMaterials, Material* mats[])
{
	for (UINT i = 0; i < numMaterials; ++i)
	{
		mBufferCache.psPerFrameBuffer.materials[i] = *mats[i];
	}
}

SkyDeferredShader::SkyDeferredShader()
{

}

SkyDeferredShader::~SkyDeferredShader()
{
// 	if (vs_cPerFrameBuffer)
// 		vs_cPerFrameBuffer->Release();

	ReleaseCOM(vs_cPerFrameBuffer);
}

bool SkyDeferredShader::Init(ID3D11Device* device, ID3D11InputLayout* inputLayout)
{
	ZeroMemory(&vs_cPerFrameBufferVariables, sizeof(VS_CPERFRAMEBUFFER));

	// Fill in a buffer description.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_CPERFRAMEBUFFER);
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	cbDesc.StructureByteStride = 0;

	// Fill in the subresource data.
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &vs_cPerFrameBufferVariables;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// Now create the buffer
	device->CreateBuffer(&cbDesc, &InitData, &vs_cPerFrameBuffer);

	mInputLayout = inputLayout;

	return true;
}

bool SkyDeferredShader::SetActive(ID3D11DeviceContext* dc)
{
	// Set vertex layout and primitive topology
	dc->IASetInputLayout(mInputLayout);

	// Set active shaders
	dc->VSSetShader(mVertexShader, nullptr, 0);
	dc->PSSetShader(mPixelShader, nullptr, 0);

	dc->PSSetSamplers(0, 1, &RenderStates::mLinearSS);

	return true;
}

bool SkyDeferredShader::BindShaders(ID3D11VertexShader* vShader, ID3D11PixelShader* pShader)
{
	mVertexShader = vShader;
	mPixelShader = pShader;

	return true;
}

void SkyDeferredShader::SetWorldViewProj(const XMMATRIX& worldViewProj)
{
	mBufferCache.vsBuffer.WorldViewProj = XMMatrixTranspose(worldViewProj);
}

void SkyDeferredShader::SetCubeMap(ID3D11DeviceContext* dc, ID3D11ShaderResourceView* cubeMap)
{
	dc->PSSetShaderResources(0, 1, &cubeMap);
}

void SkyDeferredShader::Update(ID3D11DeviceContext* dc)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	dc->Map(vs_cPerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	VS_CPERFRAMEBUFFER* dataPtr = (VS_CPERFRAMEBUFFER*)mappedResource.pData;

	//dataPtr->WorldViewProj = mBufferCache.vsBuffer.WorldViewProj;
	*dataPtr = mBufferCache.vsBuffer;

	dc->Unmap(vs_cPerFrameBuffer, 0);
	dc->VSSetConstantBuffers(0, 1, &vs_cPerFrameBuffer);
}

void SkyDeferredShader::SetPrevWorldViewProj(XMMATRIX& prevWorld, XMMATRIX& prevViewProj)
{
	mBufferCache.vsBuffer.prevWorldViewProj = XMMatrixTranspose(XMMatrixMultiply(prevWorld, prevViewProj));
}