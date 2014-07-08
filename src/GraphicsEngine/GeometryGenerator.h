//------------------------------------------------------------------------------------
// File: GeometryGenerator.h
//
// This class generates the geometry of common mathematical objects
//------------------------------------------------------------------------------------

#ifndef GEOMETRYGENERATOR_H_
#define GEOMETRYGENERATOR_H_

#include <vector>

#include "Direct3D.h"
#include "DirectXUtil.h"

class GeometryGenerator
{
public:
	struct Vertex
	{
		Vertex(){}

		Vertex(const XMFLOAT3& _position, const XMFLOAT3& _normal, const XMFLOAT3& _tangentU,
			const XMFLOAT2& _texCoord) 
		: position(_position), normal(_normal), tangentU(_tangentU), texCoord(_texCoord){}

		Vertex(	float posX, float posY, float posZ,
				float normX, float normY, float normZ,
				float tanX, float tanY, float tanZ,
				float texU, float texV) : 
		position(posX, posY, posZ), normal(normX, normY, normZ), tangentU(tanX, tanY, tanZ),
		texCoord(texU, texV){}

		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT3 tangentU;
		XMFLOAT2 texCoord;
	};

	struct MeshData
	{
		std::vector<Vertex> vertices;
		std::vector<UINT> indices;
	};

	// Creates a sphere centered at the origin, slices and stacks determine the degree
	// of tessellation
	void createSphere(float radius, UINT sliceCount, UINT stackCount, MeshData& meshData);
};

#endif