#ifndef PARTICLESYSTEM_H_
#define PARTICLESYSTEM_H_

#include "DirectXUtil.h"
#include "GeometryGenerator.h"
#include "common/util.h"
#include "ShaderHandler.h"
#include "Camera.h"

#define BLOCKSIZE 1024
#define BITONIC_BLOCK_SIZE 512
#define TRANSPOSE_BLOCK_SIZE 16
#define GRID_NUM_INDICES 16777216 // 256 * 256 * 256

struct ParticleDensity
{
	FLOAT Density;
};

struct ParticleForces
{
	XMFLOAT3 Acceleration;
};

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

	//void SetDirection(float x, float y, float z);

private:
	XMFLOAT3 mPosition;
	XMFLOAT3 mVelocity;
	/*
	XMFLOAT3 mAcceleration;
	float mMass;
	float mTime;
	*/
};

class SphereParticle : public Particle
{
public:
	SphereParticle();
	SphereParticle(float x, float y, float z, float radius);
	~SphereParticle();

// 	const float GetRadius();
// 
// 	void SetRadius(float radius);

private:
	/*
	float mRadius;
	UINT mGridHash;
	*/
};

class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();

	void Init(UINT maxParticles);
	void Init(UINT maxParticles, ID3D11ComputeShader* computeShader);
	void Init(UINT maxParticles, ID3D11ComputeShader* computeShader, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleSRV);
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

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mParticleSRV;
};

class SphereParticleSystem : public ParticleSystem
{
public:
	SphereParticleSystem();
	~SphereParticleSystem();

	bool Init(ID3D11Device* device, float sphereRadius, int numSphereSlices, int numSphereStacks, UINT maxParticles);
	void Init(ID3D11Device* device, ID3D11ComputeShader* computeShader, float sphereRadius,
		int numSphereSlices, int numSphereStacks, UINT maxParticles);
	void Init(ID3D11Device* device, ID3D11ComputeShader* computeShader, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleSRV, float sphereRadius,
		int numSphereSlices, int numSphereStacks, UINT maxParticles);

	void Init(ID3D11Device* device, ID3D11ComputeShader* computeShader,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleSRV, float sphereRadius,
		int numSphereSlices, int numSphereStacks, UINT maxParticles,
		BitonicSortShader* sortShader, MatrixTransposeShader* transposeShader, GridIndicesShader* gridIndicesShader,
		ID3D11ComputeShader* particleRearrangeShader, ID3D11ComputeShader* gridBuildShader,
		ID3D11ComputeShader* particleForcesShader, ID3D11ComputeShader* particleDensityShader);

	void SetFluidBehaviorProperties(FLOAT smoothlen, FLOAT pressureStiffnes, FLOAT restDensity, FLOAT particleMass, FLOAT viscosity);

	bool CreateParticles(ID3D11Device* device, ID3D11DeviceContext* dc, UINT numParticles);

	void Update(ID3D11DeviceContext* dc, double time, float dt);
	void Render(ParticleDrawShader* particleDrawShader, ID3D11DeviceContext* dc, const Camera& cam);

	void GPUSort(ID3D11DeviceContext* pd3dImmediateContext,
		ID3D11UnorderedAccessView* inUAV, ID3D11ShaderResourceView* inSRV,
		ID3D11UnorderedAccessView* tempUAV, ID3D11ShaderResourceView* tempSRV);

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

	// Fluid-particle behavior properties
	struct FluidBehaviorProperties
	{
		FLOAT Smoothlen;
		FLOAT PressureStiffness;
		FLOAT RestDensity;
		FLOAT ParticleMass;
		FLOAT Viscosity;
	};

	FluidBehaviorProperties mFluidBehaviorProperties;

	float mParticleRadius;

	FLOAT mMaxAllowableTimeStep;

	struct ConstantVariables
	{
		UINT groupDimX;
		UINT groupDimY;
		UINT numParticles;
		int padding;

// 		UINT gridSizeX;
// 		UINT gridSizeY;
// 		UINT gridSizeZ;
// 		UINT numCells;

// 		float cellSize;
// 		XMFLOAT3 originPosW;

		float gravity;
		XMFLOAT3 boundaries;

/*		UINT maxParticlesPerCell;*/
	};

	struct PerFrameVariables
	{
		float dt;
// 		XMFLOAT3 boundaries;
// 
// 		float gravity;
		XMFLOAT3 padding2;
	};

	BitonicSortShader* mSortShader;
	MatrixTransposeShader* mTransposeShader;
	GridIndicesShader* mGridIndicesShader;

	ConstantVariables mConstantVariables;
	PerFrameVariables mPerFrameVariables;

	Sphere* mSphere;

	// Grid
	ID3D11Buffer*                       mGridBuffer;
	ID3D11ShaderResourceView*           mGridSRV;
	ID3D11UnorderedAccessView*          mGridUAV;

	ID3D11Buffer*                       mGridPingPongBuffer;
	ID3D11ShaderResourceView*           mGridPingPongSRV;
	ID3D11UnorderedAccessView*          mGridPingPongUAV;

	ID3D11Buffer*                       mGridIndicesBuffer;
	ID3D11ShaderResourceView*           mGridIndicesSRV;
	ID3D11UnorderedAccessView*          mGridIndicesUAV;

	// Sorted particle array
	ID3D11Buffer*                       mSortedParticlesBuffer;
	ID3D11ShaderResourceView*           mSortedParticlesSRV;
	ID3D11UnorderedAccessView*          mSortedParticlesUAV;

	// Particle forces
	ID3D11Buffer*                       mParticleForces;
	ID3D11ShaderResourceView*           mParticleForcesSRV;
	ID3D11UnorderedAccessView*          mParticleForcesUAV;

	// Particle density
	ID3D11Buffer*                       mParticleDensity;
	ID3D11ShaderResourceView*           mParticleDensitySRV;
	ID3D11UnorderedAccessView*          mParticleDensityUAV;

	ID3D11ComputeShader* mParticleRearrangeCS;

	//---------------------------------
	// Grid build shader
	//---------------------------------
	ID3D11ComputeShader* mGridBuildCS;
	struct GridBuildConstants
	{
		MathHelper::UINT3 gridSize;
		UINT numCells;

		XMFLOAT3 cellSize;
		XMFLOAT3 originPosW;
	};
	GridBuildConstants mGridBuildConstants;
	ID3D11Buffer* mGridBuildBuffer;

	//---------------------------------
	// Particle forces shader
	//---------------------------------
	ID3D11ComputeShader* mParticleForcesCS;
	struct ParticleForcesConstants
	{
		float g_fPressureStiffness;
		float g_fRestDensity;
		float g_fSmoothlen;
		float g_fGradPressureCoef;
		float g_fLapViscosityCoef;
		//XMFLOAT3 padding;

		XMFLOAT3 originPosW;
		MathHelper::UINT3 gridSize;
		XMFLOAT3 cellSize;
		XMFLOAT2 padding;
	};
	ParticleForcesConstants mParticleForcesConstants;
	ID3D11Buffer* mParticleForcesBuffer;

	//---------------------------------
	// Particle density shader
	//---------------------------------
	ID3D11ComputeShader* mParticleDensityCS;
	struct ParticleDensityConstants
	{
		float g_fSmoothlen;
		float g_fDensityCoef;
		//XMFLOAT2 padding;

		XMFLOAT3 originPosW;
		MathHelper::UINT3 gridSize;
		XMFLOAT3 cellSize;
		float padding;
	};
	ParticleDensityConstants mParticleDensityConstants;
	ID3D11Buffer* mParticleDensityBuffer;
};

#endif