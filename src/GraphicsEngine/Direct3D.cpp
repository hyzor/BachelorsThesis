#include "Direct3D.h"

// Must be included last!
#include "common/debug.h"

using namespace Microsoft::WRL;

Direct3D::Direct3D(void)
{
	mD3dDriverType = D3D_DRIVER_TYPE_HARDWARE;

	mD3dDevice = nullptr;
	mD3dImmediateContext = nullptr;
	mSwapChain = nullptr;
	mDepthStencilBuffer = nullptr;
	mRenderTargetView = nullptr;
	mDepthStencilView = nullptr;

	mEnable4xMsaa = false;
	m4xMSAAQuality = 0;

	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));
}


Direct3D::~Direct3D(void)
{
}

bool Direct3D::Init(HWND* mainWindow, UINT width, UINT height)
{
	HRESULT hr;

	// Create D3D device and D3D device context
	// These interfaces are used to interact with the hardware

	UINT createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;

	// This array defines the set of DirectX hardware feature levels this app will support.
	// Note the ordering should be preserved.
	// Don't forget to declare your application's minimum required feature level in its
	// description.  All applications are assumed to support 9.1 unless otherwise stated.
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> deviceContext;
	hr = D3D11CreateDevice(
		nullptr,						// Display adapter (Default)
		mD3dDriverType,			// Driver type (For 3D HW Acceleration)
		nullptr,						// Software driver (No software device)
		createDeviceFlags,		// Flags (i.e. Debug, Single thread)
		featureLevels,						// Feature level (Check what version of D3D is supported)
		ARRAYSIZE(featureLevels),						// Number of feature levels
		D3D11_SDK_VERSION,		// SDK version
		&device,			// Returns created device
		&featureLevel,			// Returns feature level
		&deviceContext); // Return created device context

	hr = device.As(&mD3dDevice);
	hr = deviceContext.As(&mD3dImmediateContext);

	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	if (featureLevel < D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature level 11 unsupported.", 0, 0);
		return false;
	}

	// Check if 4x MSAA quality is supported
	hr  = mD3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMSAAQuality);
	assert(m4xMSAAQuality > 0);

	// Instance used for describing the swap chain
	DXGI_SWAP_CHAIN_DESC sd;

	// Back buffer properties
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1; // No MSAA
	sd.SampleDesc.Quality = 0; // Lowest quality level
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Use back buffer as render target
	sd.BufferCount = 1;								  // Number of back buffers to use in swap chain
	sd.OutputWindow = *mainWindow;					  // Specify window we render into
	sd.Windowed = true;								  // Windowed mode or full-screen mode
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;	      // Let display driver select most efficient presentation method
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;		// Optional flags


	// We have to find and use the same IDXGIFactory that was used to create the device before
	ComPtr<IDXGIDevice1> dxgiDevice;
	hr = mD3dDevice.As(&dxgiDevice);

	ComPtr<IDXGIAdapter> dxgiAdapter;
	hr = dxgiDevice->GetAdapter(&dxgiAdapter);

	//IDXGIFactory* dxgiFactory = 0;
	ComPtr<IDXGIFactory2> dxgiFactory;
	hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), /*(void**)*/&dxgiFactory);

	/*
	// Otherwise, create a new one using the same adapter as the existing Direct3D device.
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
	swapChainDesc.Width = static_cast<UINT>(width); // Match the size of the window.
	swapChainDesc.Height = static_cast<UINT>(height);
	//swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // This is the most common swap chain format.
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.Stereo = false;
	swapChainDesc.SampleDesc.Count = 1; // Don't use multi-sampling.
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	*/

	// Now create the swap chain with the IDXGIFactory we found
	//hr = dxgiFactory->CreateSwapChainForHwnd(md3dDevice.Get(), *mainWindow, &swapChainDesc, NULL, NULL, &mSwapChain);
	hr = dxgiFactory->CreateSwapChain(mD3dDevice.Get(), &sd, mSwapChain.GetAddressOf());

	// Release temporary COM objects
	dxgiDevice = nullptr;
	dxgiAdapter = nullptr;
	dxgiFactory = nullptr;

	// Remaining steps are also used when we resize window, so we call that function
	// 1: Create render target view to the swap chain's back buffer
	// 2: Create depth/stencil buffer and view
	// 3: Bind render target view and depth/stencil view to pipeline
	// 4: Set viewport transform
	OnResize(width, height);

	return true;
}

void Direct3D::OnResize(UINT width, UINT height)
{
	HRESULT hr;

	assert(mD3dImmediateContext);
	assert(mD3dDevice);
	assert(mSwapChain);

	// Release old views because they hold references to buffers we will be destroying
	mRenderTargetView = nullptr;
	mDepthStencilView = nullptr;

	// Release depth/stencil buffer
	mDepthStencilBuffer = nullptr;

	// Resize the swap chain
	hr = mSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

	// Recreate render target view
	ComPtr<ID3D11Texture2D> backBuffer;
	hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBuffer);
	mD3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, &mRenderTargetView);

	// Create the depth/stencil buffer and view
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = width;						// Texture width in texels
	depthStencilDesc.Height = height;					// Texture height in texels
	depthStencilDesc.MipLevels = 1;								// Number of mipmap levels
	depthStencilDesc.ArraySize = 1;								// Number of textures in texture array
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;	// Texel format

	// Set number of multisamples and quality level for the depth/stencil buffer
	// This has to match swap chain MSAA values
	if (mEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMSAAQuality - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;			// How the texture will be used
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;	// Where the resource will be bound to the pipeline
	depthStencilDesc.CPUAccessFlags = 0;					// Specify CPU access (Only GPU writes/reads to the depth/buffer)
	depthStencilDesc.MiscFlags = 0;							// Optional flags

	hr = mD3dDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	memset(&depthStencilViewDesc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	hr = mD3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &depthStencilViewDesc, &mDepthStencilView);

	D3D11_SHADER_RESOURCE_VIEW_DESC depthStencilSRViewDesc;
	memset(&depthStencilSRViewDesc, 0, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	depthStencilSRViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	depthStencilSRViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	depthStencilSRViewDesc.Texture2D.MipLevels = 1;
	
	hr = mD3dDevice->CreateShaderResourceView(mDepthStencilBuffer.Get(), &depthStencilSRViewDesc, &mDepthStencilSRView);

	// Bind render target and depth/stencil view to the pipeline
	mD3dImmediateContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());

	// Set the viewport transform
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(width);
	mScreenViewport.Height = static_cast<float>(height);
	mScreenViewport.MinDepth = 0.0f; // D3D uses a depth buffer range from 0
	mScreenViewport.MaxDepth = 1.0f; // ... to 1

	mD3dImmediateContext->RSSetViewports(1, &mScreenViewport);
}

void Direct3D::Shutdown()
{
	HRESULT hr;

	// Switch to windowed mode before releasing swap chain
	mSwapChain->SetFullscreenState(FALSE, NULL);

	mRenderTargetView = nullptr;
	mDepthStencilSRView = nullptr;
	mDepthStencilView = nullptr;
	mSwapChain = nullptr;
	mDepthStencilBuffer = nullptr;

	if (mD3dImmediateContext)
	{
		mD3dImmediateContext->ClearState();
		mD3dImmediateContext->Flush();
	}

	mD3dImmediateContext = nullptr;

#if defined(DEBUG) || defined(_DEBUG)
	//ID3D11Debug *d3dDebug;
	ComPtr<ID3D11Debug> d3dDebug;

	//hr = md3dDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
	hr = mD3dDevice.As(&d3dDebug);

	if (SUCCEEDED(hr))
	{
		d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		//d3dDebug->Release();
		d3dDebug = nullptr;
	}
#endif

	mD3dDevice = nullptr;
}

ID3D11Device1* Direct3D::GetDevice() const
{
	return mD3dDevice.Get();
}

ID3D11DeviceContext1* Direct3D::GetImmediateContext() const
{
	return mD3dImmediateContext.Get();
}

ID3D11RenderTargetView* Direct3D::GetRenderTargetView() const
{
	return mRenderTargetView.Get();
}

ID3D11DepthStencilView* Direct3D::GetDepthStencilView() const
{
	return mDepthStencilView.Get();
}

ID3D11ShaderResourceView* Direct3D::GetDepthStencilSRView() const
{
	return mDepthStencilSRView.Get();
}

D3D11_VIEWPORT Direct3D::GetScreenViewport() const
{
	return mScreenViewport;
}

IDXGISwapChain* Direct3D::GetSwapChain() const
{
	return mSwapChain.Get();
}

ID3D11Texture2D* Direct3D::GetDepthStencilBuffer() const
{
	return mDepthStencilBuffer.Get();
}
