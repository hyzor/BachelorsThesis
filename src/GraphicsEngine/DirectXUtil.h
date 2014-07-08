#ifndef DIRECTXUTIL_H_
#define DIRECTXUTIL_H_

#include <DirectXMath.h>

using namespace DirectX;

class DirectXUtil
{
public:
	static XMMATRIX InverseTranspose(CXMMATRIX M);
};

#endif