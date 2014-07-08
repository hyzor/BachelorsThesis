#ifndef RENDERSTATES_H_
#define RENDERSTATES_H_

#include "Direct3D.h"
#include "common/util.h"

class RenderStates
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static ID3D11RasterizerState* mDefaultRS;
	static ID3D11RasterizerState* mWireframeRS;
	static ID3D11RasterizerState* mNoCullRS;
	static ID3D11RasterizerState* mDepthBiasCloseToEyeRS;
	static ID3D11RasterizerState* mDepthBiasFarFromEyeRS;
	static ID3D11RasterizerState* mDepthBiasSuperFarFromEyeRS;

	static ID3D11SamplerState* mLinearSS;
	static ID3D11SamplerState* mLinearClampedSS;
	static ID3D11SamplerState* mSSAODepthSS;
	static ID3D11SamplerState* mAnisotropicSS;
	static ID3D11SamplerState* mComparisonSS;
	static ID3D11SamplerState* mPointClampedSS;

	static ID3D11DepthStencilState* mLessEqualDSS;
	static ID3D11DepthStencilState* mDefaultDSS;
	static ID3D11DepthStencilState* mDisabledDSS;
	static ID3D11DepthStencilState* mDepthStencilEnabledDSS;
	static ID3D11DepthStencilState* mDepthDisabledStencilEnabledDSS;
	static ID3D11DepthStencilState* mDepthDisabledStencilReplaceDSS;
	static ID3D11DepthStencilState* mDepthDisabledStencilUseDSS;
	static ID3D11DepthStencilState* mDepthStencilDisabledDSS;
	static ID3D11DepthStencilState* mDepthEnabledStencilUseDSS;

	static ID3D11BlendState* mDefaultBS;
	static ID3D11BlendState* mAdditiveBS;
	static ID3D11BlendState* mBlendBS;
	static ID3D11BlendState* mParticleBlendBS;
};

#endif