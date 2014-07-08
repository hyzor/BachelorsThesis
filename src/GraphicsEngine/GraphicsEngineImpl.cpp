#include "GraphicsEngineImpl.h"

#include <algorithm>

// Must be included last!
#include "common/debug.h"

GraphicsEngineImpl::GraphicsEngineImpl()
{
	//mPointLights = new PointLight[MAX_POINT_LIGHTS];
	//mDirLights = new DirLight[MAX_DIR_LIGHTS];
	//mSpotLights = new SpotLight[MAX_SPOT_LIGHTS];

	mPointLightsCount = 0;
	mDirLightsCount = 0;
	mSpotLightsCount = 0;
}

GraphicsEngineImpl::~GraphicsEngineImpl()
{
	mMaterials.clear();
	mPointLights.clear();
	mDirLights.clear();
	mSpotLights.clear();

	//delete[] mPointLights;
	//delete[] mDirLights;
	//delete[] mSpotLights;

	delete mSky;
	delete mCamera;

	mDeferredBuffers->Shutdown();
	delete mDeferredBuffers;

	mOrthoWindow->Shutdown();
	delete mOrthoWindow;

	delete mShaderHandler;

	mInputLayouts->DestroyAll();
	delete mInputLayouts;
	RenderStates::DestroyAll();

	delete mSpriteFontMonospace;
	delete mSpriteFont;
	delete mSpriteBatch;

	delete mTextureMgr;

	mD3D->Shutdown();
	delete mD3D;
}

bool GraphicsEngineImpl::Init(HWND hWindow, UINT width, UINT height, const std::string &resourceDir)
{
	mScreenWidth = width;
	mScreenHeight = height;

	mResourceDir = resourceDir;

	mD3D = new Direct3D();
	mD3D->Init(&hWindow, width, height);

	mInputLayouts = new InputLayouts();

	RenderStates::InitAll(mD3D->GetDevice());

	// Texture manager
	mTextureMgr = new TextureManager();
	mTextureMgr->Init(mD3D->GetDevice(), mD3D->GetImmediateContext());

	// Camera
	mCamera = new Camera();
	mCamera->SetLens(fovY, static_cast<float>(width) / height, zNear, zFar);
	mCamera->UpdateOrthoMatrix(static_cast<float>(width), static_cast<float>(height), zNear, zFar);
	mCamera->SetPosition(XMFLOAT3(0.0f, 0.0f, -400.0f));
	mCamera->UpdateBaseViewMatrix();
	mCamera->Update();

	mDeferredBuffers = new DeferredBuffers();
	mDeferredBuffers->Init(mD3D->GetDevice(), width, height);

	//mSky = new Sky(mD3D->GetDevice(), mTextureMgr, mResourceDir + "Textures\\SkyBox.dds", 2000.0f);
	mSky = new Sky(mD3D->GetDevice(), mTextureMgr, 2000.0f);

	mGameTime = 0.0f;

	//-------------------------------------------------------------------------------------------------------
	// Shaders
	//-------------------------------------------------------------------------------------------------------
	mShaderHandler = new ShaderHandler();

	// Load all the pre-compiled shaders
	// Deferred shaders
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\BasicDeferredVS.cso", "BasicDeferredVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\BasicDeferredPS.cso", "BasicDeferredPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\LightDeferredVS.cso", "LightDeferredVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\LightDeferredPS.cso", "LightDeferredPS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledVertexShader(L"..\\shaders\\SkyDeferredVS.cso", "SkyDeferredVS", mD3D->GetDevice());
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\SkyDeferredPS.cso", "SkyDeferredPS", mD3D->GetDevice());

	// Lit scene to texture
	mShaderHandler->LoadCompiledPixelShader(L"..\\shaders\\LightDeferredPS_ToTexture.cso", "LightDeferredPS_ToTexture", mD3D->GetDevice());

	// Bind loaded shaders to shader objects
	mShaderHandler->mBasicDeferredShader->BindShaders(
		mShaderHandler->GetVertexShader("BasicDeferredVS"),
		mShaderHandler->GetPixelShader("BasicDeferredPS"));
	mShaderHandler->mLightDeferredShader->BindShaders(
		mShaderHandler->GetVertexShader("LightDeferredVS"),
		mShaderHandler->GetPixelShader("LightDeferredPS"));
	mShaderHandler->mSkyDeferredShader->BindShaders(
		mShaderHandler->GetVertexShader("SkyDeferredVS"),
		mShaderHandler->GetPixelShader("SkyDeferredPS"));

	mShaderHandler->mLightDeferredToTextureShader->BindShaders(
		mShaderHandler->GetVertexShader("LightDeferredVS"),
		mShaderHandler->GetPixelShader("LightDeferredPS_ToTexture"));

	// Now create all the input layouts
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("BasicVS"), InputLayoutDesc::PosNormalTex, COUNT_OF(InputLayoutDesc::PosNormalTex), &mInputLayouts->PosNormalTex);
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("LineVS"), InputLayoutDesc::Position2D, COUNT_OF(InputLayoutDesc::Position2D), &mInputLayouts->Position2D);
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("SkyVS"), InputLayoutDesc::Position, COUNT_OF(InputLayoutDesc::Position), &mInputLayouts->Position);
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("NormalMapSkinnedVS"),
		InputLayoutDesc::PosNormalTexTanSkinned,
		COUNT_OF(InputLayoutDesc::PosNormalTexTanSkinned),
		&mInputLayouts->PosNormalTexTanSkinned);
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("LightDeferredVS"), InputLayoutDesc::PosTex, COUNT_OF(InputLayoutDesc::PosTex), &mInputLayouts->PosTex);
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("BasicDeferredMorphVS"), InputLayoutDesc::PosNormalTexTargets4, COUNT_OF(InputLayoutDesc::PosNormalTexTargets4), &mInputLayouts->PosNormalTexTargets4);
	mInputLayouts->CreateInputLayout(mD3D->GetDevice(), mShaderHandler->GetShader("StreamOutParticleVS"), InputLayoutDesc::Particle, COUNT_OF(InputLayoutDesc::Particle), &mInputLayouts->Particle);

	// Init all the shader objects
	mShaderHandler->mBasicDeferredShader->Init(mD3D->GetDevice(), mInputLayouts->PosNormalTex);
	mShaderHandler->mLightDeferredShader->Init(mD3D->GetDevice(), mInputLayouts->PosTex);
	mShaderHandler->mSkyDeferredShader->Init(mD3D->GetDevice(), mInputLayouts->Position);
	mShaderHandler->mLightDeferredToTextureShader->Init(mD3D->GetDevice(), mInputLayouts->PosTex);

	std::string fontPath = mResourceDir + "buxton.spritefont";
	std::string fontPathMonospace = mResourceDir + "monospace_8.spritefont";
	std::wstring fontPathW(fontPath.begin(), fontPath.end());
	std::wstring fontPatMonospacehW(fontPathMonospace.begin(), fontPathMonospace.end());

	mSpriteBatch = new SpriteBatch(mD3D->GetImmediateContext());
	mSpriteFont = new SpriteFont(mD3D->GetDevice(), fontPathW.c_str());
	mSpriteFontMonospace = new SpriteFont(mD3D->GetDevice(), fontPatMonospacehW.c_str());

	// Create orthogonal window
	mOrthoWindow = new OrthoWindow();
	mOrthoWindow->Initialize(mD3D->GetDevice(), width, height);

	return true;
}

void GraphicsEngineImpl::Run(float dt)
{
	UpdateScene(dt, mGameTime);
	DrawScene();
}

void GraphicsEngineImpl::DrawScene()
{
	// Restore default states
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	mD3D->GetImmediateContext()->RSSetState(0);
	mD3D->GetImmediateContext()->OMSetDepthStencilState(0, 0);
	mD3D->GetImmediateContext()->OMSetBlendState(0, blendFactor, 0xffffffff);

	mD3D->GetImmediateContext()->ClearDepthStencilView(mD3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ID3D11RenderTargetView* renderTarget;

	mD3D->GetImmediateContext()->RSSetState(0);
	// Restore back and depth buffer and viewport to the OM stage
	ID3D11RenderTargetView* renderTargets[1] = { mD3D->GetRenderTargetView() };
	mD3D->GetImmediateContext()->OMSetRenderTargets(1, renderTargets, mD3D->GetDepthStencilView());
	mD3D->GetImmediateContext()->ClearRenderTargetView(mD3D->GetRenderTargetView(), reinterpret_cast<const float*>(&Colors::LightSteelBlue));

	mD3D->GetImmediateContext()->ClearDepthStencilView(mD3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	mD3D->GetImmediateContext()->RSSetViewports(1, &mD3D->GetScreenViewport());

	// Render the scene to the render buffers and light it up
	RenderSceneToTexture();

	// Turn off Z-buffer to begin 2D-drawing
	//mD3D->GetImmediateContext()->OMSetDepthStencilState(RenderStates::mDisabledDSS, 0);
	mD3D->GetImmediateContext()->OMGetDepthStencilState(&RenderStates::mDepthStencilDisabledDSS, 0);

	ID3D11ShaderResourceView* finalSRV = mDeferredBuffers->GetLitSceneSRV();

	renderTarget = mD3D->GetRenderTargetView();
	mD3D->GetImmediateContext()->OMSetRenderTargets(1, &renderTarget, NULL);
	mD3D->GetImmediateContext()->OMSetBlendState(0, blendFactor, 0xffffffff);

	//-------------------------------------------------------------------------------------
	// Restore defaults
	//-------------------------------------------------------------------------------------
	// Reset the render target to the back buffer.
	renderTarget = mD3D->GetRenderTargetView();
	mD3D->GetImmediateContext()->OMSetRenderTargets(1, &renderTarget, NULL);
	// Reset viewport
	mD3D->GetImmediateContext()->RSSetViewports(1, &mD3D->GetScreenViewport());

	// Turn z-buffer back on
	mD3D->GetImmediateContext()->OMSetDepthStencilState(RenderStates::mDefaultDSS, 1);

	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	mD3D->GetImmediateContext()->PSSetShaderResources(0, 16, nullSRV);
	mD3D->GetImmediateContext()->VSSetShaderResources(0, 16, nullSRV);

	mD3D->GetImmediateContext()->RSSetState(0);
	mD3D->GetImmediateContext()->OMSetDepthStencilState(0, 0);
	mD3D->GetImmediateContext()->OMSetBlendState(0, blendFactor, 0xffffffff);

	mD3D->GetImmediateContext()->OMSetRenderTargets(0, nullptr, nullptr);
}

void GraphicsEngineImpl::UpdateScene(float dt, float gameTime)
{
	mGameTime = gameTime;
	mCurFPS = 1000.0f / dt;
	mCurFPS = mCurFPS / 1000.0f;

	mCamera->Update();
}

void GraphicsEngineImpl::Present()
{
	// Present the back buffer to front buffer
	// Set SyncInterval to 1 if you want to limit the FPS to the monitors refresh rate
	mD3D->GetSwapChain()->Present(0, 0);
}

void GraphicsEngineImpl::OnResize(UINT width, UINT height)
{
	mScreenWidth = width;
	mScreenHeight = height;

	mD3D->OnResize(width, height);
	mDeferredBuffers->OnResize(mD3D->GetDevice(), width, height);
	mCamera->SetLens(fovY, (float)width / height, zNear, zFar);
	mCamera->UpdateOrthoMatrix(static_cast<float>(width), static_cast<float>(height), zNear, zFar);
	mOrthoWindow->OnResize(mD3D->GetDevice(), width, height);
}

void GraphicsEngineImpl::RenderSceneToTexture()
{
	mDeferredBuffers->SetRenderTargets(mD3D->GetImmediateContext(), mD3D->GetDepthStencilView());
	mD3D->GetImmediateContext()->RSSetViewports(1, &mD3D->GetScreenViewport());

	mDeferredBuffers->ClearRenderTargets(mD3D->GetImmediateContext(), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), mD3D->GetDepthStencilView());
	mD3D->GetImmediateContext()->ClearRenderTargetView(mDeferredBuffers->GetLitSceneRTV(), reinterpret_cast<const float*>(&Colors::White));

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	mD3D->GetImmediateContext()->RSSetState(0);
	mD3D->GetImmediateContext()->OMSetDepthStencilState(0, 0);
	mD3D->GetImmediateContext()->OMSetBlendState(0, blendFactor, 0xffffffff);

	ID3D11RenderTargetView* renderTargetsLitScene[1] = { mDeferredBuffers->GetLitSceneRTV() };

	mD3D->GetImmediateContext()->OMSetBlendState(RenderStates::mDefaultBS, blendFactor, 0xffffffff);

	mD3D->GetImmediateContext()->RSSetState(RenderStates::mDefaultRS);

	//---------------------------------------------------------------------------------------
	// Sky
	//---------------------------------------------------------------------------------------
	mSky->Draw(mD3D->GetImmediateContext(), *mCamera, mShaderHandler->mSkyDeferredShader);

	// Enable stencil testing (subsequent draw calls will set stencil bits to 1)
	// Because the sky was drawn before setting this, the stencil bits that aren't set to 1 
	// (remained 0) will therefore mean that this is the sky.
	mD3D->GetImmediateContext()->OMSetDepthStencilState(RenderStates::mDepthStencilEnabledDSS, 1);

	// Sky shader uses no culling, so switch back to back face culling
	mD3D->GetImmediateContext()->RSSetState(RenderStates::mDefaultRS);

	//---------------------------------------------------------------------------------------
	// Static opaque objects
	//---------------------------------------------------------------------------------------
	mShaderHandler->mBasicDeferredShader->SetActive(mD3D->GetImmediateContext());

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	//---------------------------------------------------------------------------------------
	// Opaque objects lighting
	//---------------------------------------------------------------------------------------
	mShaderHandler->mLightDeferredToTextureShader->SetActive(mD3D->GetImmediateContext());
	mShaderHandler->mLightDeferredToTextureShader->SetEyePosW(mCamera->GetPosition());
	mShaderHandler->mLightDeferredToTextureShader->SetPointLights(mD3D->GetImmediateContext(), (UINT)mPointLights.size(), mPointLights.data());
	mShaderHandler->mLightDeferredToTextureShader->SetDirLights(mD3D->GetImmediateContext(), (UINT)mDirLights.size(), mDirLights.data());
	mShaderHandler->mLightDeferredToTextureShader->SetSpotLights(mD3D->GetImmediateContext(), (UINT)mSpotLights.size(), mSpotLights.data());
	//mShaderHandler->mLightDeferredToTextureShader->SetShadowTransform(mShadowMap->GetShadowTransform());
	mShaderHandler->mLightDeferredToTextureShader->SetCameraViewProjMatrix(mCamera->GetViewMatrix(), mCamera->GetProjMatrix());
	//mShaderHandler->mLightDeferredToTextureShader->SetLightWorldViewProj(mShadowMap->GetLightWorld(), mShadowMap->GetLightView(), mShadowMap->GetLightProj());

	// TODO: Instead of hard coding these properties, get them from some modifiable settings collection
	//mShaderHandler->mLightDeferredToTextureShader->SetFogProperties(1, 0.0195f, -125.0f, 0.105f, XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	mShaderHandler->mLightDeferredToTextureShader->SetMotionBlurProperties(1);
	mShaderHandler->mLightDeferredToTextureShader->SetFpsValues(mCurFPS, mTargetFPS);
	mShaderHandler->mLightDeferredToTextureShader->SetSkipLighting(false);
	mShaderHandler->mLightDeferredToTextureShader->SetIsTransparencyPass(false);

	mShaderHandler->mLightDeferredToTextureShader->UpdatePerFrame(mD3D->GetImmediateContext());

	mShaderHandler->mLightDeferredToTextureShader->SetDiffuseTexture(mD3D->GetImmediateContext(), mDeferredBuffers->GetSRV(DeferredBuffersIndex::Diffuse));
	mShaderHandler->mLightDeferredToTextureShader->SetNormalTexture(mD3D->GetImmediateContext(), mDeferredBuffers->GetSRV(DeferredBuffersIndex::Normal));

	mShaderHandler->mLightDeferredToTextureShader->SetWorldViewProj(XMMatrixIdentity(), mCamera->GetBaseViewMatrix(), mCamera->GetOrthoMatrix());
	mShaderHandler->mLightDeferredToTextureShader->UpdatePerObj(mD3D->GetImmediateContext());

	// Now render the window
	mOrthoWindow->Render(mD3D->GetImmediateContext());

	//---------------------------------------------------------------------------------------
	// Sky lighting
	//---------------------------------------------------------------------------------------
	mD3D->GetImmediateContext()->OMSetDepthStencilState(RenderStates::mDepthDisabledStencilUseDSS, 0); // Draw using stencil values of 0 (this is the sky)

	// We don't want any light to affect the sky, set these "num values" to 0
	mShaderHandler->mLightDeferredToTextureShader->SetPointLights(mD3D->GetImmediateContext(), 0, mPointLights.data());
	mShaderHandler->mLightDeferredToTextureShader->SetDirLights(mD3D->GetImmediateContext(), 0, mDirLights.data());
	mShaderHandler->mLightDeferredToTextureShader->SetSpotLights(mD3D->GetImmediateContext(), 0, mSpotLights.data());
	mShaderHandler->mLightDeferredToTextureShader->SetSkipLighting(true);

	mShaderHandler->mLightDeferredToTextureShader->UpdatePerFrame(mD3D->GetImmediateContext());

	mOrthoWindow->Render(mD3D->GetImmediateContext());

	// Lastly, clear (unbind) the textures (otherwise D3D11 WARNING)
	mShaderHandler->mLightDeferredToTextureShader->SetDiffuseTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredToTextureShader->SetNormalTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredToTextureShader->SetSpecularTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredToTextureShader->SetSSAOTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredToTextureShader->SetDepthTexture(mD3D->GetImmediateContext(), NULL);
	mShaderHandler->mLightDeferredToTextureShader->SetVelocityTexture(mD3D->GetImmediateContext(), NULL);

	// Clear stencil buffer
	mD3D->GetImmediateContext()->ClearDepthStencilView(mD3D->GetDepthStencilView(), /*D3D11_CLEAR_DEPTH | */D3D11_CLEAR_STENCIL, 1.0f, 0);

	mD3D->GetImmediateContext()->RSSetState(0);
	mD3D->GetImmediateContext()->OMSetDepthStencilState(0, 0);
	mD3D->GetImmediateContext()->OMSetBlendState(0, blendFactor, 0xffffffff);

	// Reset the render target back to the original back buffer and not the render buffers
	ID3D11RenderTargetView* renderTargets[1] = { mD3D->GetRenderTargetView() };
	mD3D->GetImmediateContext()->OMSetRenderTargets(1, renderTargets, mD3D->GetDepthStencilView());

	// Reset viewport
	mD3D->GetImmediateContext()->RSSetViewports(1, &mD3D->GetScreenViewport());
}

void GraphicsEngineImpl::UpdateSceneData()
{
	//--------------------------------------------------------
	// Compute scene bounding box
	//--------------------------------------------------------
	XMFLOAT3 minPt(+MathHelper::GetInfinity(), +MathHelper::GetInfinity(), +MathHelper::GetInfinity());
	XMFLOAT3 maxPt(-MathHelper::GetInfinity(), -MathHelper::GetInfinity(), -MathHelper::GetInfinity());

	//Used when creating orthogonal projection matrix for cascades
	BoundingBox::CreateFromPoints(this->mSceneBB, XMLoadFloat3(&minPt), XMLoadFloat3(&maxPt));

	// Sphere center is at half of these new dimensions
	mSceneBounds.Center = XMFLOAT3(
		0.5f*(minPt.x + maxPt.x),
		0.5f*(minPt.y + maxPt.y),
		0.5f*(minPt.z + maxPt.z));

	// Calculate the sphere radius
	XMFLOAT3 extent(
		0.5f*(maxPt.x - minPt.x),
		0.5f*(maxPt.y - minPt.y),
		0.5f*(maxPt.z - minPt.z));

	mSceneBounds.Radius = sqrtf(extent.x*extent.x + extent.y*extent.y + extent.z*extent.z);

	mShaderHandler->mLightDeferredToTextureShader->SetMaterials(mD3D->GetImmediateContext(),
		mMaterials.size(), mMaterials.data());
}


void GraphicsEngineImpl::PrintText(std::string text, int x, int y, XMFLOAT3 RGB, float scale, float alpha)
{
	std::wstring t = std::wstring(text.begin(), text.end());
	XMVECTORF32 v_color = { RGB.x, RGB.y, RGB.z, alpha };
	mSpriteFont->DrawString(mSpriteBatch, t.c_str(), XMFLOAT2((float)x, (float)y), v_color, 0.0f, XMFLOAT2(0, 0), scale);
}

void GraphicsEngineImpl::GetWindowResolution(UINT& width, UINT& height)
{
	width = mScreenWidth;
	height = mScreenHeight;
}

void GraphicsEngineImpl::Clear()
{
	mD3D->GetImmediateContext()->RSSetState(0);
	// Restore back and depth buffer and viewport to the OM stage
	ID3D11RenderTargetView* renderTargets[1] = { mD3D->GetRenderTargetView() };
	mD3D->GetImmediateContext()->OMSetRenderTargets(1, renderTargets, mD3D->GetDepthStencilView());
	mD3D->GetImmediateContext()->ClearRenderTargetView(mD3D->GetRenderTargetView(), reinterpret_cast<const float*>(&Colors::LightSteelBlue));

	mD3D->GetImmediateContext()->ClearDepthStencilView(mD3D->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	mD3D->GetImmediateContext()->RSSetViewports(1, &mD3D->GetScreenViewport());
}

void GraphicsEngineImpl::SetFullscreen(bool fullscreen)
{
	mD3D->GetSwapChain()->SetFullscreenState(fullscreen, NULL);
}

bool GraphicsEngineImpl::IsFullscreen()
{
	BOOL fullscreen;
	mD3D->GetSwapChain()->GetFullscreenState(&fullscreen, NULL);
	return fullscreen == 1;
}

void GraphicsEngineImpl::ResetRenderTargetAndViewport()
{
	ID3D11RenderTargetView *renderTarget = mD3D->GetRenderTargetView();
	mD3D->GetImmediateContext()->OMSetRenderTargets(1, &renderTarget, mD3D->GetDepthStencilView());
	mD3D->GetImmediateContext()->RSSetViewports(1, &mD3D->GetScreenViewport());
}

void GraphicsEngineImpl::SetSkyTexture(const std::string& fileName)
{
	mSky->SetTexture(mResourceDir + fileName, mTextureMgr);
}