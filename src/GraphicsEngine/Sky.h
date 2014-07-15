//------------------------------------------------------------------------------------
// File: Sky.h
//
// This class renders a sky using a cube map
//------------------------------------------------------------------------------------

#ifndef SKY_H_
#define SKY_H_

#include "Direct3D.h"
#include "Camera.h"
#include <DirectXTK/DDSTextureLoader.h>
#include "ShaderHandler.h"
#include "RenderStates.h"
#include "TextureManager.h"

class Sky
{
public:
	Sky(ID3D11Device* device, TextureManager *textureManager, const std::string& cubeMapFileName, float skySphereRadius);
	Sky(ID3D11Device* device, TextureManager *textureManager, float skySphereRadius);
	~Sky(void);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetCubeMapSRV();

	void Draw(ID3D11DeviceContext* dc, const Camera& cam, SkyDeferredShader* skyShader);

	void SetTexture(const std::string&, TextureManager* textureManager);

private:
	// Buffers
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;

	// Texture
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mCubeMapSRV;

	UINT mIndexCount;
};

#endif