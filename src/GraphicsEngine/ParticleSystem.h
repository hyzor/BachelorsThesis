#ifndef PARTICLESYSTEM_H_
#define PARTICLESYSTEM_H_

#include "DirectXUtil.h"
#include "GeometryGenerator.h"
#include "common/util.h"
#include "ShaderHandler.h"

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

private:
	XMFLOAT3 mPosition;
	XMFLOAT3 mVelocity;
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

	void Init(int maxParticles);
	void Init(int maxParticles, ID3D11ComputeShader* computeShader);
	bool CreateParticles(int numParticles);
	void CalculateThreadGroups(int numParticles);

	void Update(double time);
	void Render();

protected:
	int mMaxParticles;
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

	void Init(float sphereRadius, int numSphereSlices, int numSphereStacks, int maxParticles);
	void Init(ID3D11ComputeShader* computeShader, float sphereRadius,
		int numSphereSlices, int numSphereStacks, int maxParticles);
	bool CreateParticles(ID3D11Device* device, int numParticles);

	void Update(ID3D11DeviceContext* dc, double time, float dt);
	void Render();

private:
	struct SphereProperties
	{
		float sphereRadius;
		int numSlices;
		int numStacks;
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
		float frameTime;
		XMFLOAT3 padding2;
	};

	ConstantVariables mConstantVariables;
	PerFrameVariables mPerFrameVariables;

	SphereProperties mSphereProperties;
	GeometryGenerator::MeshData mSphere;
};

#endif