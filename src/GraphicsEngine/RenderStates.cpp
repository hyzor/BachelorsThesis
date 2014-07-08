#include "RenderStates.h"

// Must be included last!
#include "common/debug.h"

ID3D11RasterizerState* RenderStates::mDefaultRS = 0;
ID3D11RasterizerState* RenderStates::mWireframeRS = 0;
ID3D11RasterizerState* RenderStates::mNoCullRS = 0;
ID3D11RasterizerState* RenderStates::mDepthBiasCloseToEyeRS = 0;
ID3D11RasterizerState* RenderStates::mDepthBiasFarFromEyeRS = 0;
ID3D11RasterizerState* RenderStates::mDepthBiasSuperFarFromEyeRS = 0;

ID3D11SamplerState* RenderStates::mLinearSS = 0;
ID3D11SamplerState* RenderStates::mLinearClampedSS = 0;
ID3D11SamplerState* RenderStates::mSSAODepthSS = 0;
ID3D11SamplerState* RenderStates::mAnisotropicSS = 0;
ID3D11SamplerState* RenderStates::mComparisonSS = 0;
ID3D11SamplerState* RenderStates::mPointClampedSS = 0;

ID3D11DepthStencilState* RenderStates::mLessEqualDSS = 0;
ID3D11DepthStencilState* RenderStates::mDefaultDSS = 0;
ID3D11DepthStencilState* RenderStates::mDisabledDSS = 0;
ID3D11DepthStencilState* RenderStates::mDepthStencilEnabledDSS = 0;
ID3D11DepthStencilState* RenderStates::mDepthDisabledStencilEnabledDSS = 0;
ID3D11DepthStencilState* RenderStates::mDepthDisabledStencilReplaceDSS = 0;
ID3D11DepthStencilState* RenderStates::mDepthDisabledStencilUseDSS = 0;
ID3D11DepthStencilState* RenderStates::mDepthStencilDisabledDSS = 0;
ID3D11DepthStencilState* RenderStates::mDepthEnabledStencilUseDSS = 0;

ID3D11BlendState* RenderStates::mDefaultBS = 0;
ID3D11BlendState* RenderStates::mAdditiveBS = 0;
ID3D11BlendState* RenderStates::mBlendBS = 0;
ID3D11BlendState* RenderStates::mParticleBlendBS = 0;

void RenderStates::InitAll(ID3D11Device* device)
{
	//-----------------------------------------------------------
	// Rasterizer states
	//-----------------------------------------------------------
	// Default rasterizer state
	D3D11_RASTERIZER_DESC defaultRSdesc;
	ZeroMemory(&defaultRSdesc, sizeof(D3D11_RASTERIZER_DESC));
	defaultRSdesc.AntialiasedLineEnable = false;
	defaultRSdesc.CullMode = D3D11_CULL_BACK;
	defaultRSdesc.DepthBias = 0;
	defaultRSdesc.DepthBiasClamp = 0.0f;
	defaultRSdesc.DepthClipEnable = true;
	defaultRSdesc.FillMode = D3D11_FILL_SOLID;
	defaultRSdesc.FrontCounterClockwise = false;
	defaultRSdesc.MultisampleEnable = false;
	defaultRSdesc.ScissorEnable = false;
	defaultRSdesc.SlopeScaledDepthBias = 0.0f;

	device->CreateRasterizerState(&defaultRSdesc, &mDefaultRS);

	// Wireframe rasterizer state
	D3D11_RASTERIZER_DESC wireframeRSdesc;
	ZeroMemory(&wireframeRSdesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeRSdesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeRSdesc.CullMode = D3D11_CULL_BACK;
	wireframeRSdesc.FrontCounterClockwise = false;
	wireframeRSdesc.DepthClipEnable = true;

	device->CreateRasterizerState(&wireframeRSdesc, &mWireframeRS);

	// No culling rasterizer state
	D3D11_RASTERIZER_DESC noCullRSdesc;
	ZeroMemory(&noCullRSdesc, sizeof(D3D11_RASTERIZER_DESC));
	noCullRSdesc.FillMode = D3D11_FILL_SOLID;
	noCullRSdesc.CullMode = D3D11_CULL_NONE;
	noCullRSdesc.FrontCounterClockwise = false;
	noCullRSdesc.DepthClipEnable = true;

	device->CreateRasterizerState(&noCullRSdesc, &mNoCullRS);

	//For rendering shadows close to the eye
	D3D11_RASTERIZER_DESC depthBiasCloseRSdesc;
	ZeroMemory(&depthBiasCloseRSdesc, sizeof(D3D11_RASTERIZER_DESC));
	depthBiasCloseRSdesc.DepthBias = 10000;
	depthBiasCloseRSdesc.DepthBiasClamp = 0.0f;
	depthBiasCloseRSdesc.SlopeScaledDepthBias = 4.0f;
	depthBiasCloseRSdesc.FillMode = D3D11_FILL_SOLID;
	depthBiasCloseRSdesc.CullMode = D3D11_CULL_BACK;

	device->CreateRasterizerState(&depthBiasCloseRSdesc, &mDepthBiasCloseToEyeRS);

	//For rendering shadows not as close to the eye
	D3D11_RASTERIZER_DESC depthBiasFarRSdesc;
	ZeroMemory(&depthBiasFarRSdesc, sizeof(D3D11_RASTERIZER_DESC));
	depthBiasFarRSdesc.DepthBias = 10000;
	depthBiasFarRSdesc.DepthBiasClamp = 0.0f;
	depthBiasFarRSdesc.SlopeScaledDepthBias = 5.0f;
	depthBiasFarRSdesc.FillMode = D3D11_FILL_SOLID;
	depthBiasFarRSdesc.CullMode = D3D11_CULL_BACK;

	device->CreateRasterizerState(&depthBiasFarRSdesc, &mDepthBiasFarFromEyeRS);

	//For rendering shadows far from the eye
	D3D11_RASTERIZER_DESC depthBiasSuperFarRSdesc;
	ZeroMemory(&depthBiasSuperFarRSdesc, sizeof(D3D11_RASTERIZER_DESC));
	depthBiasSuperFarRSdesc.DepthBias = 10000;
	depthBiasSuperFarRSdesc.DepthBiasClamp = 0.0f;
	depthBiasSuperFarRSdesc.SlopeScaledDepthBias = 4.0f;
	depthBiasSuperFarRSdesc.FillMode = D3D11_FILL_SOLID;
	depthBiasSuperFarRSdesc.CullMode = D3D11_CULL_BACK;

	device->CreateRasterizerState(&depthBiasSuperFarRSdesc, &mDepthBiasSuperFarFromEyeRS);

	//-----------------------------------------------------------
	// Sampler states
	//-----------------------------------------------------------
	// Linear sampler state
	D3D11_SAMPLER_DESC linearSSdesc;
	ZeroMemory(&linearSSdesc, sizeof(D3D11_SAMPLER_DESC));
	linearSSdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	linearSSdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	linearSSdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	linearSSdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	linearSSdesc.MipLODBias = 0.0f;
	linearSSdesc.MaxAnisotropy = 1;
	linearSSdesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	linearSSdesc.BorderColor[0] = 0;
	linearSSdesc.BorderColor[1] = 0;
	linearSSdesc.BorderColor[2] = 0;
	linearSSdesc.BorderColor[3] = 0;
	linearSSdesc.MinLOD = 0;
	linearSSdesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&linearSSdesc, &mLinearSS);

	// Linear clamped sampler state
	D3D11_SAMPLER_DESC linearClampedSSdesc;
	ZeroMemory(&linearClampedSSdesc, sizeof(D3D11_SAMPLER_DESC));
	linearClampedSSdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	linearClampedSSdesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	linearClampedSSdesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	linearClampedSSdesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	linearClampedSSdesc.MipLODBias = 0.0f;
	linearClampedSSdesc.MaxAnisotropy = 1;
	linearClampedSSdesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	linearClampedSSdesc.BorderColor[0] = 0;
	linearClampedSSdesc.BorderColor[1] = 0;
	linearClampedSSdesc.BorderColor[2] = 0;
	linearClampedSSdesc.BorderColor[3] = 0;
	linearClampedSSdesc.MinLOD = 0;
	linearClampedSSdesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&linearClampedSSdesc, &mLinearClampedSS);

	// Linear SSAO Depth sampler state
	D3D11_SAMPLER_DESC SSAODepthSSdesc;
	ZeroMemory(&SSAODepthSSdesc, sizeof(D3D11_SAMPLER_DESC));
	SSAODepthSSdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SSAODepthSSdesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	SSAODepthSSdesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	SSAODepthSSdesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	SSAODepthSSdesc.MipLODBias = 0.0f;
	SSAODepthSSdesc.MaxAnisotropy = 1;
	SSAODepthSSdesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	SSAODepthSSdesc.BorderColor[0] = 1.0f;
	SSAODepthSSdesc.BorderColor[1] = 1.0f;
	SSAODepthSSdesc.BorderColor[2] = 1.0f;
	SSAODepthSSdesc.BorderColor[3] = 1.0f;
	SSAODepthSSdesc.MinLOD = 0;
	SSAODepthSSdesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&SSAODepthSSdesc, &mSSAODepthSS);

	// Anisotropic sampler state
	D3D11_SAMPLER_DESC anisotropicSSdesc;
	ZeroMemory(&anisotropicSSdesc, sizeof(D3D11_SAMPLER_DESC));
	anisotropicSSdesc.Filter = D3D11_FILTER_ANISOTROPIC;
	anisotropicSSdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	anisotropicSSdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	anisotropicSSdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	anisotropicSSdesc.MipLODBias = 0.0f;
	anisotropicSSdesc.MaxAnisotropy = 4;
	anisotropicSSdesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	anisotropicSSdesc.BorderColor[0] = 0;
	anisotropicSSdesc.BorderColor[1] = 0;
	anisotropicSSdesc.BorderColor[2] = 0;
	anisotropicSSdesc.BorderColor[3] = 0;
	anisotropicSSdesc.MinLOD = 0;
	anisotropicSSdesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&anisotropicSSdesc, &mAnisotropicSS);

	//comparison filter sampler state
	D3D11_SAMPLER_DESC comparisonSSdesc;
	ZeroMemory(&comparisonSSdesc, sizeof(D3D11_SAMPLER_DESC));
	comparisonSSdesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	comparisonSSdesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSSdesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSSdesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	comparisonSSdesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	comparisonSSdesc.BorderColor[0] = 0.0f;
	comparisonSSdesc.BorderColor[1] = 0.0f;
	comparisonSSdesc.BorderColor[2] = 0.0f;
	comparisonSSdesc.BorderColor[3] = 0.0f;

	device->CreateSamplerState(&comparisonSSdesc, &mComparisonSS);

	// Point sampler state
	D3D11_SAMPLER_DESC pointSSdesc;
	ZeroMemory(&pointSSdesc, sizeof(D3D11_SAMPLER_DESC));
	pointSSdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	pointSSdesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	pointSSdesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	pointSSdesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	device->CreateSamplerState(&pointSSdesc, &mPointClampedSS);

	//-----------------------------------------------------------
	// Depth stencil states
	//-----------------------------------------------------------
	// Less equal depth stencil state
	D3D11_DEPTH_STENCIL_DESC lessEqualDSSdesc;
	ZeroMemory(&lessEqualDSSdesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	lessEqualDSSdesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	device->CreateDepthStencilState(&lessEqualDSSdesc, &mLessEqualDSS);

	// Less equal depth stencil state
	D3D11_DEPTH_STENCIL_DESC defaultDSSdesc;
	ZeroMemory(&defaultDSSdesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	defaultDSSdesc.DepthEnable = TRUE;
	defaultDSSdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	defaultDSSdesc.DepthFunc = D3D11_COMPARISON_LESS;
	defaultDSSdesc.StencilEnable = FALSE;
	defaultDSSdesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	defaultDSSdesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	defaultDSSdesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	defaultDSSdesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	defaultDSSdesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	defaultDSSdesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	defaultDSSdesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	defaultDSSdesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	defaultDSSdesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	defaultDSSdesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	device->CreateDepthStencilState(&defaultDSSdesc, &mDefaultDSS);

	// Disabled depth stencil state
	D3D11_DEPTH_STENCIL_DESC disabledDDSdesc;
	ZeroMemory(&disabledDDSdesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	disabledDDSdesc.DepthEnable = FALSE;
	disabledDDSdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	disabledDDSdesc.DepthFunc = D3D11_COMPARISON_LESS;
	disabledDDSdesc.StencilEnable = TRUE;
	disabledDDSdesc.StencilReadMask = 0xFF;
	disabledDDSdesc.StencilWriteMask = 0xFF;
	disabledDDSdesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	disabledDDSdesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	disabledDDSdesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	disabledDDSdesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	disabledDDSdesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	disabledDDSdesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	disabledDDSdesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	disabledDDSdesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	device->CreateDepthStencilState(&disabledDDSdesc, &mDisabledDSS);

	// Both depth and stencil testing enabled
	D3D11_DEPTH_STENCIL_DESC depthStencilEnabledDSSdesc;
	ZeroMemory(&depthStencilEnabledDSSdesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilEnabledDSSdesc.DepthEnable = TRUE;
	depthStencilEnabledDSSdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilEnabledDSSdesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilEnabledDSSdesc.StencilEnable = TRUE;
	depthStencilEnabledDSSdesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilEnabledDSSdesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	depthStencilEnabledDSSdesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilEnabledDSSdesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilEnabledDSSdesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	depthStencilEnabledDSSdesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	depthStencilEnabledDSSdesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilEnabledDSSdesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilEnabledDSSdesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	depthStencilEnabledDSSdesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	device->CreateDepthStencilState(&depthStencilEnabledDSSdesc, &mDepthStencilEnabledDSS);

	// Depth disabled, stencil enabled
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilEnabledDSSdesc;
	ZeroMemory(&depthDisabledStencilEnabledDSSdesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthDisabledStencilEnabledDSSdesc.DepthEnable = FALSE;
	depthDisabledStencilEnabledDSSdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilEnabledDSSdesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthDisabledStencilEnabledDSSdesc.StencilEnable = TRUE;
	depthDisabledStencilEnabledDSSdesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthDisabledStencilEnabledDSSdesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	depthDisabledStencilEnabledDSSdesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilEnabledDSSdesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilEnabledDSSdesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	depthDisabledStencilEnabledDSSdesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	depthDisabledStencilEnabledDSSdesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilEnabledDSSdesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilEnabledDSSdesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	depthDisabledStencilEnabledDSSdesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilEnabledDSSdesc.DepthEnable = FALSE;

	device->CreateDepthStencilState(&depthDisabledStencilEnabledDSSdesc, &mDepthDisabledStencilEnabledDSS);

	// Depth disabled, stencil replace
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilReplaceDSSdesc;
	ZeroMemory(&depthDisabledStencilReplaceDSSdesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthDisabledStencilReplaceDSSdesc.DepthEnable = FALSE;
	depthDisabledStencilReplaceDSSdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilReplaceDSSdesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthDisabledStencilReplaceDSSdesc.StencilEnable = TRUE;
	depthDisabledStencilReplaceDSSdesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthDisabledStencilReplaceDSSdesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	depthDisabledStencilReplaceDSSdesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilReplaceDSSdesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilReplaceDSSdesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	depthDisabledStencilReplaceDSSdesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	depthDisabledStencilReplaceDSSdesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilReplaceDSSdesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilReplaceDSSdesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	depthDisabledStencilReplaceDSSdesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	device->CreateDepthStencilState(&depthDisabledStencilReplaceDSSdesc, &mDepthDisabledStencilReplaceDSS);

	// Depth disabled, stencil use
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilUseDSSdesc;
	ZeroMemory(&depthDisabledStencilUseDSSdesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthDisabledStencilUseDSSdesc.DepthEnable = FALSE;
	depthDisabledStencilUseDSSdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilUseDSSdesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthDisabledStencilUseDSSdesc.StencilEnable = TRUE;
	depthDisabledStencilUseDSSdesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthDisabledStencilUseDSSdesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	depthDisabledStencilUseDSSdesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	depthDisabledStencilUseDSSdesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilUseDSSdesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilUseDSSdesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	depthDisabledStencilUseDSSdesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	depthDisabledStencilUseDSSdesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilUseDSSdesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilUseDSSdesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	device->CreateDepthStencilState(&depthDisabledStencilUseDSSdesc, &mDepthDisabledStencilUseDSS);

	// Depth disabled, stencil disabled
	D3D11_DEPTH_STENCIL_DESC depthStencilDisabledDSSdesc;
	ZeroMemory(&depthStencilDisabledDSSdesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilDisabledDSSdesc.DepthEnable = FALSE;
	depthStencilDisabledDSSdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDisabledDSSdesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDisabledDSSdesc.StencilEnable = FALSE;
	depthStencilDisabledDSSdesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDisabledDSSdesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	depthStencilDisabledDSSdesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDisabledDSSdesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDisabledDSSdesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDisabledDSSdesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	depthStencilDisabledDSSdesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDisabledDSSdesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDisabledDSSdesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDisabledDSSdesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	device->CreateDepthStencilState(&depthStencilDisabledDSSdesc, &mDepthStencilDisabledDSS);

	// Depth enabled, stencil use
	D3D11_DEPTH_STENCIL_DESC depthEnabledStencilUseDSSdesc;
	ZeroMemory(&depthEnabledStencilUseDSSdesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthEnabledStencilUseDSSdesc.DepthEnable = TRUE;
	depthEnabledStencilUseDSSdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthEnabledStencilUseDSSdesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthEnabledStencilUseDSSdesc.StencilEnable = TRUE;
	depthEnabledStencilUseDSSdesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthEnabledStencilUseDSSdesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	depthEnabledStencilUseDSSdesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	depthEnabledStencilUseDSSdesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthEnabledStencilUseDSSdesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthEnabledStencilUseDSSdesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	depthEnabledStencilUseDSSdesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	depthEnabledStencilUseDSSdesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	depthEnabledStencilUseDSSdesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthEnabledStencilUseDSSdesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;

	device->CreateDepthStencilState(&depthEnabledStencilUseDSSdesc, &mDepthEnabledStencilUseDSS);

	//-----------------------------------------------------------
	// Blend states
	//-----------------------------------------------------------
	// Default blend state
	D3D11_BLEND_DESC defaultBSdesc;
	ZeroMemory(&defaultBSdesc, sizeof(D3D11_BLEND_DESC));
	defaultBSdesc.AlphaToCoverageEnable = FALSE;
	defaultBSdesc.IndependentBlendEnable = FALSE;
	defaultBSdesc.RenderTarget[0].BlendEnable = FALSE;
	defaultBSdesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	defaultBSdesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	defaultBSdesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	defaultBSdesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	defaultBSdesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	defaultBSdesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	defaultBSdesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	device->CreateBlendState(&defaultBSdesc, &mDefaultBS);

	// Additive blend state
	D3D11_BLEND_DESC additiveBSdesc;
	ZeroMemory(&additiveBSdesc, sizeof(D3D11_BLEND_DESC));
	additiveBSdesc.AlphaToCoverageEnable = FALSE;
	additiveBSdesc.IndependentBlendEnable = FALSE;
	additiveBSdesc.RenderTarget[0].BlendEnable = TRUE;
	additiveBSdesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	additiveBSdesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	additiveBSdesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	additiveBSdesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	additiveBSdesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	additiveBSdesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	additiveBSdesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	device->CreateBlendState(&additiveBSdesc, &mAdditiveBS);

	// Blend enabled
	D3D11_BLEND_DESC blendBSdesc;
	ZeroMemory(&blendBSdesc, sizeof(D3D11_BLEND_DESC));
	blendBSdesc.AlphaToCoverageEnable = FALSE;
	blendBSdesc.IndependentBlendEnable = FALSE;
	blendBSdesc.RenderTarget[0].BlendEnable = TRUE;
	blendBSdesc.RenderTarget[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
	blendBSdesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
	blendBSdesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendBSdesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendBSdesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendBSdesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendBSdesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	device->CreateBlendState(&blendBSdesc, &mBlendBS);

	D3D11_BLEND_DESC particleBSdesc;
	ZeroMemory(&particleBSdesc, sizeof(D3D11_BLEND_DESC));
	particleBSdesc.AlphaToCoverageEnable = FALSE;
	particleBSdesc.IndependentBlendEnable = TRUE;
	particleBSdesc.RenderTarget[0].BlendEnable = FALSE;
	particleBSdesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	particleBSdesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	particleBSdesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	particleBSdesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	particleBSdesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	particleBSdesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	particleBSdesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	particleBSdesc.RenderTarget[1].BlendEnable = FALSE;
	particleBSdesc.RenderTarget[1].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
	particleBSdesc.RenderTarget[1].DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
	particleBSdesc.RenderTarget[1].BlendOp = D3D11_BLEND_OP_ADD;
	particleBSdesc.RenderTarget[1].SrcBlendAlpha = D3D11_BLEND_ZERO;
	particleBSdesc.RenderTarget[1].DestBlendAlpha = D3D11_BLEND_ZERO;
	particleBSdesc.RenderTarget[1].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	particleBSdesc.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	particleBSdesc.RenderTarget[2].BlendEnable = FALSE;
	particleBSdesc.RenderTarget[2].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
	particleBSdesc.RenderTarget[2].DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
	particleBSdesc.RenderTarget[2].BlendOp = D3D11_BLEND_OP_ADD;
	particleBSdesc.RenderTarget[2].SrcBlendAlpha = D3D11_BLEND_ZERO;
	particleBSdesc.RenderTarget[2].DestBlendAlpha = D3D11_BLEND_ZERO;
	particleBSdesc.RenderTarget[2].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	particleBSdesc.RenderTarget[2].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	particleBSdesc.RenderTarget[3].BlendEnable = TRUE;
	particleBSdesc.RenderTarget[3].SrcBlend = D3D11_BLEND_ZERO;
	particleBSdesc.RenderTarget[3].DestBlend = D3D11_BLEND_SRC_COLOR;
	particleBSdesc.RenderTarget[3].BlendOp = D3D11_BLEND_OP_ADD;
	particleBSdesc.RenderTarget[3].SrcBlendAlpha = D3D11_BLEND_ZERO;
	particleBSdesc.RenderTarget[3].DestBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	particleBSdesc.RenderTarget[3].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	particleBSdesc.RenderTarget[3].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	device->CreateBlendState(&particleBSdesc, &mParticleBlendBS);
}

void RenderStates::DestroyAll()
{
	ReleaseCOM(mDefaultRS);
	ReleaseCOM(mWireframeRS);
	ReleaseCOM(mNoCullRS);
	ReleaseCOM(mDepthBiasCloseToEyeRS);
	ReleaseCOM(mDepthBiasFarFromEyeRS);
	ReleaseCOM(mDepthBiasSuperFarFromEyeRS);

	ReleaseCOM(mLinearSS);
	ReleaseCOM(mLinearClampedSS);
	ReleaseCOM(mSSAODepthSS);
	ReleaseCOM(mAnisotropicSS);
	ReleaseCOM(mComparisonSS);
	ReleaseCOM(mPointClampedSS);

	ReleaseCOM(mLessEqualDSS);
	ReleaseCOM(mDefaultDSS);
	ReleaseCOM(mDisabledDSS);
	ReleaseCOM(mDepthStencilEnabledDSS);
	ReleaseCOM(mDepthDisabledStencilEnabledDSS);
	ReleaseCOM(mDepthDisabledStencilReplaceDSS);
	ReleaseCOM(mDepthDisabledStencilUseDSS);
	ReleaseCOM(mDepthStencilDisabledDSS);
	ReleaseCOM(mDepthEnabledStencilUseDSS);

	ReleaseCOM(mDefaultBS);
	ReleaseCOM(mAdditiveBS);
	ReleaseCOM(mBlendBS);
	ReleaseCOM(mParticleBlendBS);
}
