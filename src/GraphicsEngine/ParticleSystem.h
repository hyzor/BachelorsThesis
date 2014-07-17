#ifndef PARTICLESYSTEM_H_
#define PARTICLESYSTEM_H_

#include "DirectXUtil.h"
#include "GeometryGenerator.h"
#include "common/util.h"
#include "ShaderHandler.h"
#include "Camera.h"

#define BLOCKSIZE 1024

class Particle
{
public:
	Particle();
	Particle(float x, float y, float z);
	~Particle();

	const XMFLOAT3 GetPosition();
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 position);

	void SetVelocity(float x, float y, float z);
	void SetVelocity(XMFLOAT3 velocity);

	void SetDirection(float x, float y, float z);

private:
	XMFLOAT3 mPosition;
	XMFLOAT3 mVelocity;
	XMFLOAT3 mAcceleration;
	XMFLOAT3 mDirection;
	float mMass;
	float mTime;
};

class SphereParticle : public Particle
{
public:
	SphereParticle();
	SphereParticle(float x, float y, float z, float radius);
	~SphereParticle();

	const float GetRadius();

	void SetRadius(float radius);

private:
	float mRadius;
};

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	void Init(UINT maxParticles);
	void Init(UINT maxParticles, ID3D11ComputeShader* computeShader);
	bool CreateParticles(UINT numParticles);
	void CalculateThreadGroups(UINT numParticles);

	UINT GetNumParticles();

	void Update(ID3D11DeviceContext* dc, double time, float dt);
	void Render();

protected:
	UINT mMaxParticles;
	UINT mNumParticles;
	UINT mNumThreadGroupsX, mNumThreadGroupsY, mNumThreadGroupsZ;

	ID3D11ComputeShader* mComputeShader;
	ID3D11Buffer* mParticleBuffer;
	ID3D11ShaderResourceView* mParticleBufferView;
	ID3D11UnorderedAccessView* mParticleBufferUAV;

	ID3D11Buffer* mConstantsBuffer;
	ID3D11Buffer* mPerFrameBuffer;
};

class SphereParticleSystem : public ParticleSystem
{
public:
	SphereParticleSystem();
	~SphereParticleSystem();

	bool Init(ID3D11Device* device, float sphereRadius, int numSphereSlices, int numSphereStacks, UINT maxParticles);
	void Init(ID3D11Device* device, ID3D11ComputeShader* computeShader, float sphereRadius,
		int numSphereSlices, int numSphereStacks, UINT maxParticles);
	bool CreateParticles(ID3D11Device* device, UINT numParticles);

	void Update(ID3D11DeviceContext* dc, double time, float dt);
	void Render(ParticleDrawShader* particleDrawShader, ID3D11DeviceContext* dc, const Camera& cam);

private:
	struct Sphere
	{
		Sphere()
		{
			vertexBuffer = nullptr;
			indexBuffer = nullptr;
		}

		float radius;
		int numSlices;
		int numStacks;
		GeometryGenerator::MeshData mesh;
		ID3D11Buffer* vertexBuffer;
		ID3D11Buffer* indexBuffer;
		UINT numIndices;
	};

	struct ConstantVariables
	{
		UINT groupDimX;
		UINT groupDimY;
		UINT numParticles;
		int padding;
	};

	struct PerFrameVariables
	{
		float dt;
		XMFLOAT3 boundaries;

		float gravity;
		XMFLOAT3 padding2;
	};

	ConstantVariables mConstantVariables;
	PerFrameVariables mPerFrameVariables;

	Sphere* mSphere;
};

#endif