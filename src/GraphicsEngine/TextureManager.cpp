//------------------------------------------------------------------------------------------
// File: TextureManager.cpp
//
// This class is used to avoid loading duplicate textures
//------------------------------------------------------------------------------------------

#include "TextureManager.h"

// Must be included last!
#include "common/debug.h"

TextureManager::TextureManager(void)
	: md3dDevice(0)
{
}


TextureManager::~TextureManager(void)
{
	for (auto it = mStaticTextureSRV.begin(); it != mStaticTextureSRV.end(); ++it)
	{
		ReleaseCOM(it->second);
	}

	mStaticTextureSRV.clear();
}

void TextureManager::Init(ID3D11Device* device, ID3D11DeviceContext* dc)
{
	md3dDevice = device;
	mDC = dc;
}

ID3D11ShaderResourceView* TextureManager::CreateStaticTexture(std::string fileName)
{
	ID3D11ShaderResourceView* srv = 0;

	// Find if texture already exists
	if (mStaticTextureSRV.find(fileName) != mStaticTextureSRV.end())
	{
		srv = mStaticTextureSRV[fileName]; // Just point to existing texture
	}

	// Otherwise create the new texture
	else
	{
		std::wstring path(fileName.begin(), fileName.end());

		HRESULT hr;

		// Try loading the texture as dds
		hr = DirectX::CreateDDSTextureFromFile(md3dDevice.Get(), path.c_str(), nullptr, &srv);

		// Failed loading texture (not .dds) - assume it's another format
		if (srv == NULL)
			hr = DirectX::CreateWICTextureFromFile(md3dDevice.Get(), mDC.Get(), path.c_str(), nullptr, &srv);

		// Texture loading still failed, format either unsupported or file doesn't exist
		if (srv == NULL)
		{
			std::wostringstream ErrorStream;
			ErrorStream << "Failed to load texture " << path;
			MessageBox(0, ErrorStream.str().c_str(), 0, 0);
		}

		mStaticTextureSRV[fileName] = srv;
	}

	return srv;
}

void TextureManager::DeleteTexture(ID3D11ShaderResourceView* srv)
{

}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> TextureManager::CreateTexture(std::string fileName)
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv = nullptr;

	// Find if texture already exists
	if (mTextureSRV.find(fileName) != mTextureSRV.end())
	{
		//srv = mLevelTextureSRV[fileName]; // Just point to existing texture
		srv = mTextureSRV[fileName].Get(); // Just point to existing texture
	}

	// Otherwise create the new texture
	else
	{
		std::wstring path(fileName.begin(), fileName.end());

		HRESULT hr;

		// Try loading the texture as dds
		hr = DirectX::CreateDDSTextureFromFile(md3dDevice.Get(), path.c_str(), nullptr, &srv);

		// Failed loading texture (not .dds) - assume it's another format
		if (srv == NULL)
			hr = DirectX::CreateWICTextureFromFile(md3dDevice.Get(), mDC.Get(), path.c_str(), nullptr, &srv);

		// Texture loading still failed, format either unsupported or file doesn't exist
		if (srv == NULL)
		{
			std::wostringstream ErrorStream;
			ErrorStream << "Failed to load texture " << path;
			MessageBox(0, ErrorStream.str().c_str(), 0, 0);
		}

		mTextureSRV[fileName] = srv.Get();
	}

	//return srv;
	return srv;
}
