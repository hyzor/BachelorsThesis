#ifndef DIRECT3D_H_
#define DIRECT3D_H_

#include <d3d11_1.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <wrl/client.h>

class Direct3D
{
public:
	Direct3D(void);
	~Direct3D(void);

	//bool Init(HWND* mainWindow, int& _clientWidth, int& _clientHeight);
	bool Init(HWND* mainWindow, UINT width, UINT height);
	//void OnResize();
	void OnResize(UINT width, UINT height);
	void Shutdown();

	ID3D11Device1* GetDevice() const;
	ID3D11DeviceContext1* GetImmediateContext() const;
	ID3D11RenderTargetView* GetRenderTargetView() const;
	ID3D11DepthStencilView* GetDepthStencilView() const;
	ID3D11ShaderResourceView* GetDepthStencilSRView() const;
	ID3D11Texture2D* GetDepthStencilBuffer() const;
	D3D11_VIEWPORT GetScreenViewport() const;
	IDXGISwapChain* GetSwapChain() const;

private:
	D3D11_VIEWPORT mScreenViewport;

	Microsoft::WRL::ComPtr<ID3D11Device1> mD3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext1> mD3dImmediateContext;
	//Microsoft::WRL::ComPtr<IDXGISwapChain1> mSwapChain;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mDepthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mDepthStencilSRView;

	D3D_DRIVER_TYPE mD3dDriverType;

	bool mEnable4xMsaa;
	UINT m4xMSAAQuality;
};

#endif