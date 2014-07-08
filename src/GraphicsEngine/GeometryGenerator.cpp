//------------------------------------------------------------------------------------
// File: GeometryGenerator.cpp
//
// This class generates the geometry of common mathematical objects
//------------------------------------------------------------------------------------

#include "GeometryGenerator.h"

// Must be included last!
#include "common/debug.h"

void GeometryGenerator::createSphere(float radius, UINT sliceCount, UINT stackCount, MeshData& meshData)
{
	meshData.vertices.clear();
	meshData.indices.clear();

	// Create poles
	Vertex topVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	Vertex bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);

	meshData.vertices.push_back(topVertex);

	float phiStep = XM_PI/stackCount;
	float thetaStep = 2.0f*XM_PI/sliceCount;

	// Compute vertices for each stack ring (do not count the poles as rings)
	for(UINT i = 1; i <= stackCount-1; ++i)
	{
		float phi = i*phiStep;

		// Vertices of ring.
		for(UINT j = 0; j <= sliceCount; ++j)
		{
			float theta = j*thetaStep;

			Vertex v;

			// spherical to cartesian
			v.position.x = radius*sinf(phi)*cosf(theta);
			v.position.y = radius*cosf(phi);
			v.position.z = radius*sinf(phi)*sinf(theta);

			// Partial derivative of P with respect to theta
			v.tangentU.x = -radius*sinf(phi)*sinf(theta);
			v.tangentU.y = 0.0f;
			v.tangentU.z = +radius*sinf(phi)*cosf(theta);

			XMVECTOR T = XMLoadFloat3(&v.tangentU);
			XMStoreFloat3(&v.tangentU, XMVector3Normalize(T));

			XMVECTOR p = XMLoadFloat3(&v.position);
			XMStoreFloat3(&v.normal, XMVector3Normalize(p));

			v.texCoord.x = theta / XM_2PI;
			v.texCoord.y = phi / XM_PI;

			meshData.vertices.push_back( v );
		}
	}

	meshData.vertices.push_back( bottomVertex );

	// Compute indices for top stack.  The top stack was written first to the vertex buffer
	// and connects the top pole to the first ring

	for(UINT i = 1; i <= sliceCount; ++i)
	{
		meshData.indices.push_back(0);
		meshData.indices.push_back(i+1);
		meshData.indices.push_back(i);
	}

	// Compute indices for inner stacks (not connected to poles)

	// Offset the indices to the index of the first vertex in the first ring
	// This is just skipping the top pole vertex
	UINT baseIndex = 1;
	UINT ringVertexCount = sliceCount+1;
	for(UINT i = 0; i < stackCount-2; ++i)
	{
		for(UINT j = 0; j < sliceCount; ++j)
		{
			meshData.indices.push_back(baseIndex + i*ringVertexCount + j);
			meshData.indices.push_back(baseIndex + i*ringVertexCount + j+1);
			meshData.indices.push_back(baseIndex + (i+1)*ringVertexCount + j);

			meshData.indices.push_back(baseIndex + (i+1)*ringVertexCount + j);
			meshData.indices.push_back(baseIndex + i*ringVertexCount + j+1);
			meshData.indices.push_back(baseIndex + (i+1)*ringVertexCount + j+1);
		}
	}

	// Compute indices for bottom stack.  The bottom stack was written last to the vertex buffer
	// and connects the bottom pole to the bottom ring

	// South pole vertex was added last.
	UINT southPoleIndex = (UINT)meshData.vertices.size()-1;

	// Offset the indices to the index of the first vertex in the last ring
	baseIndex = southPoleIndex - ringVertexCount;

	for(UINT i = 0; i < sliceCount; ++i)
	{
		meshData.indices.push_back(southPoleIndex);
		meshData.indices.push_back(baseIndex+i);
		meshData.indices.push_back(baseIndex+i+1);
	}
}
