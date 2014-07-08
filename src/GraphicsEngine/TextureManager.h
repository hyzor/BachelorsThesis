//------------------------------------------------------------------------------------------
// File: TextureManager.h
//
// This class is used to avoid loading duplicate textures
//------------------------------------------------------------------------------------------

#ifndef TEXTUREMANAGER_H_
#define TEXTUREMANAGER_H_

#include <sstream>
#include <DirectXTK/DDSTextureLoader.h>
#include <DirectXTK/WICTextureLoader.h>

#include "Direct3D.h"
#include <common/util.h>
#include <map>

class TextureManager
{
public:
	TextureManager(void);
	~TextureManager(void);

	void Init(ID3D11Device* device, ID3D11DeviceContext* dc);

	ID3D11ShaderResourceView* CreateStaticTexture(std::string fileName);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateTexture(std::string fileName);
	void DeleteTexture(ID3D11ShaderResourceView* srv);

private:
	Microsoft::WRL::ComPtr<ID3D11Device> md3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> mDC;

	std::map<std::string, ID3D11ShaderResourceView*> mStaticTextureSRV;
	std::map<std::string, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> mTextureSRV;
};

#endif
