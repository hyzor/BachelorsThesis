//-------------------------------------------------------------------------------------------
// File: LightDef.h
//
// This class is only used to define lights
//-------------------------------------------------------------------------------------------

#ifndef LIGHTDEF_H
#define LIGHTDEF_H

#include <windows.h>
#include <DirectXMath.h>

using namespace DirectX;

struct DirLight
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	// Forms into a 4D vector
	XMFLOAT3 Direction;
	float Padding;
};

struct PointLight
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	// Forms into a 4D vector
	XMFLOAT3 Position;
	float Range;

	// Forms into a 4D vector
	XMFLOAT3 Attenuation;
	float Padding;
};

struct SpotLight
{
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	// Forms into a 4D vector
	XMFLOAT3 Position;
	float Range;

	// Forms into a 4D vector
	XMFLOAT3 Direction;
	float Spot;

	// Forms into a 4D vector
	XMFLOAT3 Attenuation;
	float Padding;
};

struct Material
{
	Material() { ZeroMemory(this, sizeof(Material)); }

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular; // w = SpecPower
	XMFLOAT4 Reflect;

	inline bool operator==(Material mat)
	{
		if (mat.Ambient.x == Ambient.x && mat.Ambient.y == Ambient.y && mat.Ambient.z == Ambient.z && mat.Ambient.w == Ambient.w &&
			mat.Diffuse.x == Diffuse.x && mat.Diffuse.y == Diffuse.y && mat.Diffuse.z == Diffuse.z && mat.Diffuse.w == Diffuse.w &&
			mat.Specular.x == Specular.x && mat.Specular.y == Specular.y && mat.Specular.z == Specular.z && mat.Specular.w == Specular.w &&
			mat.Reflect.x == Reflect.x && mat.Reflect.y == Reflect.y && mat.Reflect.z == Reflect.z && mat.Reflect.w == Reflect.w)
		{
			return true;
		}
		else
			return false;
	}
};

#endif