#ifndef GRAPHICS_GRAPHICSENGINE_H_
#define GRAPHICS_GRAPHICSENGINE_H_

#include <DirectXMath.h>

#include <string>
#include "common/platform.h"

class DLL_API GraphicsEngine
{
public:
	virtual ~GraphicsEngine() {}

	virtual bool Init(HWND hWindow, UINT width, UINT height, const std::string &resourceDir) = 0;
	virtual void OnResize(UINT width, UINT height) = 0;

	virtual void Run(float dt) = 0;

	virtual void DrawScene() = 0;
	virtual void UpdateScene(float dt, float gameTime) = 0;
	virtual void RenderSceneToTexture() = 0;
	virtual void Present() = 0;

	virtual void UpdateSceneData() = 0;

	virtual void Clear() = 0;

	virtual void SetFullscreen(bool fullscreen) = 0;
	virtual bool IsFullscreen() = 0;
	virtual void GetWindowResolution(UINT& width, UINT& height) = 0;

	virtual void ResetRenderTargetAndViewport() = 0;

	virtual void SetSkyTexture(const std::string& fileName) = 0;

	// Text
	virtual void LoadFont(std::string fontPath, std::string fontName) = 0;
	virtual void PrintText(std::string text, int x, int y, DirectX::XMFLOAT3 RGB, float scale, float alpha) = 0;
	virtual void SetFont(std::string fontName) = 0;
};

DLL_API GraphicsEngine* CreateGraphicsEngine();
DLL_API void DestroyGraphicsEngine(GraphicsEngine* engine);

#endif
