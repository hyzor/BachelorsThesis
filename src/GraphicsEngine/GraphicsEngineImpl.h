#ifndef GRAPHICS_GRAPHICSENGINEIMPL_H_
#define GRAPHICS_GRAPHICSENGINEIMPL_H_

#include <map>
#include <string>
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>

#include "Direct3D.h"
#include "Camera.h"
#include "DeferredBuffers.h"
#include "OrthoWindow.h"
#include "TextureManager.h"
#include "ShaderHandler.h"
#include "LightDef.h"
#include "GraphicsEngine.h"
#include "Sky.h"
#include "RenderStates.h"
#include "common/util.h"
#include "ParticleSystem.h"

static const enum BlendingMethods
{
	ALPHA_BLENDING = 0,
	ADDITIVE_BLENDING
};

class GraphicsEngineImpl : public GraphicsEngine
{
public:
	GraphicsEngineImpl();
	~GraphicsEngineImpl();

	bool Init(HWND hWindow, UINT width, UINT height, const std::string &resourceDir);
	void OnResize(UINT width, UINT height);
	void Run(float dt);
	void DrawScene();
	void UpdateScene(float dt, float gameTime);
	void RenderSceneToTexture();
	void Present();
	void UpdateSceneData();
	void Clear();
	void SetFullscreen(bool fullscreen);
	bool IsFullscreen();
	void GetWindowResolution(UINT& width, UINT& height);
	void ResetRenderTargetAndViewport();
	void SetSkyTexture(const std::string& fileName);

	// Text
	void LoadFont(std::string fontPath, std::string fontName);
	void PrintText(std::string text, int x, int y, XMFLOAT3 RGB, float scale, float alpha);
	void SetFont(std::string fontName);

private:
	Direct3D* mD3D;
	std::string mResourceDir;
	TextureManager* mTextureMgr;
	Camera* mCamera;
	InputLayouts* mInputLayouts;

	int mPointLightsCount;
	int mDirLightsCount;
	int mSpotLightsCount;

	Sky* mSky;

	std::vector<Material*> mMaterials;
	std::vector<PointLight*> mPointLights;
	std::vector<DirLight*> mDirLights;
	std::vector<SpotLight*> mSpotLights;

	ShaderHandler* mShaderHandler;

	SpriteBatch* mSpriteBatch;

	std::map<std::string, SpriteFont*> mSpriteFonts;
	SpriteFont* mCurFont;

	DirectX::BoundingSphere mSceneBounds;
	DirectX::BoundingBox mSceneBB;

	DeferredBuffers* mDeferredBuffers;
	OrthoWindow* mOrthoWindow;

	float mGameTime;

	float mCurFPS;
	float mTargetFPS;

	UINT mScreenWidth;
	UINT mScreenHeight;

	SphereParticleSystem* mSphereParticleSystem;
};

#endif
