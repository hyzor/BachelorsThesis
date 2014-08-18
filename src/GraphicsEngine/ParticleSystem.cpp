#include "ParticleSystem.h"

#include <random>

Particle::Particle()
{
	mPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
// 	mAcceleration = XMFLOAT3(0.0f, 0.0f, 0.0f);
// 	mMass = 1.0f;
// 	mTime = 0.0f;
}

Particle::Particle(float x, float y, float z)
{
	mPosition = XMFLOAT3(x, y, z);
}

Particle::~Particle(){}

const DirectX::XMFLOAT3 Particle::GetPosition()
{
	return mPosition;
}

void Particle::SetPosition(float x, float y, float z)
{
	mPosition = XMFLOAT3(x, y, z);
}

void Particle::SetPosition(XMFLOAT3 position)
{
	mPosition = position;
}

void Particle::SetVelocity(float x, float y, float z)
{
	mVelocity = XMFLOAT3(x, y, z);
}

void Particle::SetVelocity(XMFLOAT3 velocity)
{
	mVelocity = velocity;
}

SphereParticle::SphereParticle()
	: Particle()
{
	/*
	mRadius = 1.0f;
	mGridHash = 0;
	*/
}

SphereParticle::SphereParticle(float x, float y, float z, float radius)
	: Particle(x, y, z)
{
	/*
	mRadius = radius;
	mGridHash = 0;
	*/
}

SphereParticle::~SphereParticle(){}

// const float SphereParticle::GetRadius()
// {
// 	return mRadius;
// }
// 
// void SphereParticle::SetRadius(float radius)
// {
// 	mRadius = radius;
// }

ParticleSystem::ParticleSystem()
{
	mComputeShader = nullptr;
	mParticleBuffer = nullptr;
	mParticleBufferView = nullptr;
	mParticleBufferUAV = nullptr;

	mConstantsBuffer = nullptr;
	mPerFrameBuffer = nullptr;

	mParticleSRV = nullptr;
}

ParticleSystem::~ParticleSystem()
{
	ReleaseCOM(mParticleBuffer);
	ReleaseCOM(mParticleBufferView);
	ReleaseCOM(mParticleBufferUAV);

	ReleaseCOM(mConstantsBuffer);
	ReleaseCOM(mPerFrameBuffer);
}

void ParticleSystem::Init(UINT maxParticles)
{
	mMaxParticles = maxParticles;
	mNumParticles = 0;
}

void ParticleSystem::Init(UINT maxParticles, ID3D11ComputeShader* computeShader)
{
	Init(maxParticles);
	mComputeShader = computeShader;
}

void ParticleSystem::Init(UINT maxParticles, ID3D11ComputeShader* computeShader,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleTexture)
{
	Init(maxParticles, computeShader);
	mParticleSRV = particleTexture;
}

bool ParticleSystem::CreateParticles(UINT numParticles)
{
	if (numParticles <= mMaxParticles)
	{
		Particle* particles = new Particle[numParticles];

		// Send particles to compute shader

		delete[] particles;

		return true;
	}
	else
	{
		MessageBox(nullptr, L"The amount of particles to be created exceeds the particle limit!", L"Error", 0);
		return false;
	}
}

void ParticleSystem::CalculateThreadGroups(UINT numParticles)
{
	int numGroups = 0;

	if (numParticles % BLOCKSIZE != 0)
		numGroups = (numParticles / BLOCKSIZE) + 1;
	else
		numGroups = numParticles / BLOCKSIZE;

	double thirdRoot = pow((double)numGroups, (double)(1.0 / 3.0));
	thirdRoot = ceil(thirdRoot);

	mNumThreadGroupsX = mNumThreadGroupsY = mNumThreadGroupsZ = static_cast<UINT> (thirdRoot);
}

void ParticleSystem::Update(ID3D11DeviceContext* dc, double time, float dt)
{
	// Run the particle compute shader
	dc->CSSetShader(mComputeShader, nullptr, 0);
	dc->CSSetConstantBuffers(0, 1, &mConstantsBuffer);
	dc->CSSetConstantBuffers(1, 1, &mPerFrameBuffer);
	dc->CSSetUnorderedAccessViews(0, 1, &mParticleBufferUAV, NULL);
	dc->Dispatch(mNumThreadGroupsX, mNumThreadGroupsY, mNumThreadGroupsZ);

	ID3D11UnorderedAccessView* nullUAV = nullptr;
	dc->CSSetUnorderedAccessViews(0, 1, &nullUAV, NULL);
}

void ParticleSystem::Render()
{

}

UINT ParticleSystem::GetNumParticles()
{
	return mNumParticles;
}

SphereParticleSystem::SphereParticleSystem()
	: ParticleSystem()
{
	ZeroMemory(&mSphere, sizeof(mSphere));
	ZeroMemory(&mConstantVariables, sizeof(mConstantVariables));
	ZeroMemory(&mPerFrameVariables, sizeof(mPerFrameVariables));

	mGridBuffer = nullptr;
	mGridSRV = nullptr;
	mGridUAV = nullptr;

	mGridPingPongBuffer = nullptr;
	mGridPingPongSRV = nullptr;
	mGridPingPongUAV = nullptr;

	mGridIndicesBuffer = nullptr;
	mGridIndicesSRV = nullptr;
	mGridIndicesUAV = nullptr;
	
	mSortedParticlesBuffer = nullptr;
	mSortedParticlesSRV = nullptr;
	mSortedParticlesUAV = nullptr;

	ZeroMemory(&mGridBuildConstants, sizeof(mGridBuildConstants));
	mGridBuildBuffer = nullptr;
	mGridBuildCS = nullptr;

	mSortShader = nullptr;
	mTransposeShader = nullptr;
	mGridIndicesShader = nullptr;
	mParticleRearrangeCS = nullptr;

	mParticleForces = nullptr;
	mParticleForcesSRV = nullptr;
	mParticleForcesUAV = nullptr;

	mParticleDensity = nullptr;
	mParticleDensitySRV = nullptr;
	mParticleDensityUAV = nullptr;

	ZeroMemory(&mParticleForcesConstants, sizeof(mParticleForcesConstants));
	mParticleForcesCS = nullptr;
	mParticleForcesBuffer = nullptr;

	ZeroMemory(&mParticleDensityConstants, sizeof(mParticleDensityConstants));
	mParticleDensityCS = nullptr;
	mParticleDensityBuffer = nullptr;

	ZeroMemory(&mFluidBehaviorProperties, sizeof(mFluidBehaviorProperties));
	mMaxAllowableTimeStep = 0.005f;

	mGridIndicesClearCS = nullptr;

	ZeroMemory(&mGridIndicesBuildConstants, sizeof(mGridIndicesBuildConstants));
	mGridIndicesBuildCS = nullptr;
	mGridIndicesBuildBuffer = nullptr;
}

SphereParticleSystem::~SphereParticleSystem()
{
	if (mSphere)
	{
		ReleaseCOM(mSphere->vertexBuffer);
		ReleaseCOM(mSphere->indexBuffer);
		delete mSphere;
	}

	ReleaseCOM(mGridBuffer);
	ReleaseCOM(mGridSRV);
	ReleaseCOM(mGridUAV);

	ReleaseCOM(mGridPingPongBuffer);
	ReleaseCOM(mGridPingPongSRV);
	ReleaseCOM(mGridPingPongUAV);

	ReleaseCOM(mGridIndicesBuffer);
	ReleaseCOM(mGridIndicesSRV);
	ReleaseCOM(mGridIndicesUAV);

	ReleaseCOM(mSortedParticlesBuffer);
	ReleaseCOM(mSortedParticlesSRV);
	ReleaseCOM(mSortedParticlesUAV);

	ReleaseCOM(mGridBuildBuffer);

	ReleaseCOM(mParticleForces);
	ReleaseCOM(mParticleForcesSRV);
	ReleaseCOM(mParticleForcesUAV);

	ReleaseCOM(mParticleDensity);
	ReleaseCOM(mParticleDensitySRV);
	ReleaseCOM(mParticleDensityUAV);

	ReleaseCOM(mParticleDensityBuffer);
	ReleaseCOM(mParticleForcesBuffer);

	ReleaseCOM(mGridParticleHash);
	ReleaseCOM(mGridParticleHashSRV);
	ReleaseCOM(mGridParticleHashUAV);

	ReleaseCOM(mGridParticleIndex);
	ReleaseCOM(mGridParticleIndexSRV);
	ReleaseCOM(mGridParticleIndexUAV);

	ReleaseCOM(mGridKeyValuePair);
	ReleaseCOM(mGridKeyValuePairSRV);
	ReleaseCOM(mGridKeyValuePairUAV);

	ReleaseCOM(mGridKeyValuePair_PingPong);
	ReleaseCOM(mGridKeyValuePairSRV_PingPong);
	ReleaseCOM(mGridKeyValuePairUAV_PingPong);

	ReleaseCOM(mGridIndicesBuildBuffer);
}

bool SphereParticleSystem::Init(ID3D11Device* device, float sphereRadius, int numSphereSlices, int numSphereStacks, UINT maxParticles)
{
	mNumParticles = 0;
	mParticleRadius = sphereRadius;

	mSphere = new Sphere();

	mSphere->radius = sphereRadius;
	mSphere->numSlices = numSphereSlices;
	mSphere->numStacks = numSphereStacks;
	mMaxParticles = maxParticles;

	GeometryGenerator geoGen;
	geoGen.CreateSphere(mSphere->radius,
		mSphere->numSlices, mSphere->numStacks, mSphere->mesh);

	std::vector<XMFLOAT3> vertices(mSphere->mesh.vertices.size());
	for (size_t i = 0; i < mSphere->mesh.vertices.size(); ++i)
	{
		vertices[i] = mSphere->mesh.vertices[i].position;
	}

	// Create sphere vertex buffer
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = (UINT)(sizeof(XMFLOAT3) * vertices.size());
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];

	if (FAILED(device->CreateBuffer(&vbd, &vinitData, &mSphere->vertexBuffer)))
	{
		MessageBox(nullptr, L"Could not create sphere particle vertex buffer!", L"Error", 0);
		return false;
	}

	mSphere->numIndices = (UINT)mSphere->mesh.indices.size();

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(USHORT) * mSphere->numIndices;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.StructureByteStride = 0;
	ibd.MiscFlags = 0;

	//std::vector<USHORT> indices16;
	//indices16.assign(mSphere.mesh.indices.begin(), mSphere.mesh.indices.end());

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &mSphere->mesh.indices[0];

	if (FAILED(device->CreateBuffer(&ibd, &iinitData, &mSphere->indexBuffer)))
	{
		MessageBox(nullptr, L"Could not create sphere particle index buffer!", L"Error", 0);
		return false;
	}

	return true;
}

void SphereParticleSystem::Init(ID3D11Device* device, ID3D11ComputeShader* computeShader, float sphereRadius, int numSphereSlices, int numSphereStacks, UINT maxParticles)
{
	Init(device, sphereRadius, numSphereSlices, numSphereStacks, maxParticles);
	mComputeShader = computeShader;
}

void SphereParticleSystem::Init(ID3D11Device* device, ID3D11ComputeShader* computeShader,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleSRV,
	float sphereRadius, int numSphereSlices, int numSphereStacks, UINT maxParticles)
{
	Init(device, computeShader, sphereRadius, numSphereSlices, numSphereStacks, maxParticles);
	mParticleSRV = particleSRV;
}

// void SphereParticleSystem::Init(ID3D11Device* device, ID3D11ComputeShader* computeShader, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particleSRV,
// 	float sphereRadius, int numSphereSlices, int numSphereStacks, UINT maxParticles)
// {
// 	Init(device, computeShader, particleSRV, sphereRadius, numSphereSlices, numSphereStacks, maxParticles);
// }

bool SphereParticleSystem::CreateParticles(ID3D11Device* device, ID3D11DeviceContext* dc, UINT numParticles)
{
	if (numParticles <= mMaxParticles)
	{
		mNumParticles = numParticles;

		SphereParticle* particles = new SphereParticle[numParticles];

		float rndX, rndY, rndZ;
		std::tr1::mt19937 eng;
		std::tr1::uniform_real_distribution<float> dist(-GRID_SIZE, GRID_SIZE);

		float velRange = GRID_SIZE * 0.5f;

		for (UINT i = 0; i < numParticles; ++i)
		{
			rndX = dist(eng);
			rndY = dist(eng);
			rndZ = dist(eng);

 			float velX = ((float(rand()) / float(RAND_MAX)) * (velRange - (-velRange))) + (-velRange);
 			float velY = ((float(rand()) / float(RAND_MAX)) * (velRange - (-velRange))) + (-velRange);
 			float velZ = ((float(rand()) / float(RAND_MAX)) * (velRange - (-velRange))) + (-velRange);

			particles[i].SetPosition(velX, velY, velZ);
			//particles[i].SetVelocity(velX, velY, velZ);
			particles[i].SetVelocity(velX, velY, velZ);
			//particles[i].SetVelocity(0.0f, 0.0f, 0.0f);
			//particles[i].SetRadius(mSphere->radius);
		}

		CalculateThreadGroups(numParticles);
		
		// Create sphere particle compute shader input buffer
		if (FAILED(CreateStructuredBuffer<SphereParticle>(device, numParticles, &mParticleBuffer, &mParticleBufferView, &mParticleBufferUAV, particles)))
			return false;

		// Create the sorted sphere particle compute shader input buffer
		if (FAILED(CreateStructuredBuffer<SphereParticle>(device, numParticles, &mSortedParticlesBuffer, &mSortedParticlesSRV, &mSortedParticlesUAV, particles)))
			return false;

		// Create particle forces buffer
		if (FAILED(CreateStructuredBuffer<ParticleForces>(device, numParticles, &mParticleForces, &mParticleForcesSRV, &mParticleForcesUAV)))
			return false;

		// Create particle density buffer
		if (FAILED(CreateStructuredBuffer<ParticleDensity>(device, numParticles, &mParticleDensity, &mParticleDensitySRV, &mParticleDensityUAV)))
			return false;

		// Create grid structured buffer
		if (FAILED(CreateStructuredBuffer<UINT>(device, numParticles, &mGridBuffer, &mGridSRV, &mGridUAV)))
			return false;

		// Create grid ping pong structured buffer
		if (FAILED(CreateStructuredBuffer<UINT>(device, numParticles, &mGridPingPongBuffer, &mGridPingPongSRV, &mGridPingPongUAV)))
			return false;

		// Create grid particle hash
		if (FAILED(CreateStructuredBuffer<UINT>(device, numParticles, &mGridParticleHash, &mGridParticleHashSRV, &mGridParticleHashUAV)))
			return false;

		// Create grid particle id
		if (FAILED(CreateStructuredBuffer<UINT>(device, numParticles, &mGridParticleIndex, &mGridParticleIndexSRV, &mGridParticleIndexUAV)))
			return false;

		// Create grid indices buffer
		if (FAILED(CreateStructuredBuffer<MathHelper::UINT2>(device, GRID_NUM_INDICES, &mGridIndicesBuffer, &mGridIndicesSRV, &mGridIndicesUAV)))
			return false;

		// Create grid key value pair
		if (FAILED(CreateStructuredBuffer<MathHelper::UINT2>(device, numParticles, &mGridKeyValuePair, &mGridKeyValuePairSRV, &mGridKeyValuePairUAV)))
			return false;

		if (FAILED(CreateStructuredBuffer<MathHelper::UINT2>(device, numParticles, &mGridKeyValuePair_PingPong, &mGridKeyValuePairSRV_PingPong, &mGridKeyValuePairUAV_PingPong)))
			return false;

		delete[] particles;
		
		if (FAILED(IShader::CreateBuffer(device, &mConstantsBuffer, &mConstantVariables)))
		{
			MessageBox(NULL, L"Could not create constant buffer for constant variables for the compute shader!", L"Error", 0);
			return false;
		}

		mConstantVariables.groupDimX = mNumThreadGroupsX;
		mConstantVariables.groupDimY = mNumThreadGroupsY;
		mConstantVariables.numParticles = numParticles;
// 		mConstantVariables.gridSizeX = mConstantVariables.gridSizeY = mConstantVariables.gridSizeZ = 11;
// 		mConstantVariables.numCells = mConstantVariables.gridSizeX * mConstantVariables.gridSizeY * mConstantVariables.gridSizeZ;
// 		mConstantVariables.cellSize = mSphere->radius * 2.0f;
// 		mConstantVariables.originPosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
		mConstantVariables.gravity = MathHelper::GetGravityConstant();
		mConstantVariables.boundaries.x = mConstantVariables.boundaries.y = mConstantVariables.boundaries.z = GRID_SIZE;
		IShader::UpdateBuffer(dc, mConstantsBuffer, &mConstantVariables);

		if (FAILED(IShader::CreateBuffer(device, &mPerFrameBuffer, &mPerFrameVariables)))
		{
			MessageBox(NULL, L"Could not create constant buffer for per frame variables for the compute shader!", L"Error", 0);
			return false;
		}

		// Set the constant for our grid build compute shader and create the buffer for it to use
		if (FAILED(IShader::CreateBuffer(device, &mGridBuildBuffer, &mGridBuildConstants)))
			return false;

		float cellSize = mParticleRadius * 2.0f;

		// Grid properties
		mGridBuildConstants.cellSize.x = mGridBuildConstants.cellSize.y = mGridBuildConstants.cellSize.z = cellSize;
		mGridBuildConstants.gridSize.x = mGridBuildConstants.gridSize.y = mGridBuildConstants.gridSize.z = GRID_SIZE;
		mGridBuildConstants.numCells = mGridBuildConstants.gridSize.x * mGridBuildConstants.gridSize.y * mGridBuildConstants.gridSize.z;
		mGridBuildConstants.originPosW = XMFLOAT3(0.0f, 0.0f, 0.0f);
		IShader::UpdateBuffer(dc, mGridBuildBuffer, &mGridBuildConstants);

		// Particle forces shader buffer
		if (FAILED(IShader::CreateBuffer(device, &mParticleForcesBuffer, &mParticleForcesConstants)))
			return false;

		// SPH method parameters
		mParticleForcesConstants.g_fGradPressureCoef = mFluidBehaviorProperties.ParticleMass * -45.0f / (XM_PI * pow(mFluidBehaviorProperties.Smoothlen, 6));
		mParticleForcesConstants.g_fLapViscosityCoef = mFluidBehaviorProperties.ParticleMass * mFluidBehaviorProperties.Viscosity * 45.0f / (XM_PI * pow(mFluidBehaviorProperties.Smoothlen, 6));
		mParticleForcesConstants.g_fPressureStiffness = mFluidBehaviorProperties.PressureStiffness;
		mParticleForcesConstants.g_fRestDensity = mFluidBehaviorProperties.RestDensity;
		mParticleForcesConstants.g_fSmoothlen = mFluidBehaviorProperties.Smoothlen;

		// Grid properties
		mParticleForcesConstants.cellSize = mGridBuildConstants.cellSize;
		mParticleForcesConstants.gridSize = mGridBuildConstants.gridSize;
		mParticleForcesConstants.originPosW = mGridBuildConstants.originPosW;

		mParticleForcesConstants.g_fSphereRadius = mParticleRadius;

		// DEM method parameters (Parameters varying depending on grid size and particle numbers)
		// Good for ~64k particles @ 64x64x64 grid
		mParticleForcesConstants.g_fParamsSpring = 25.5f;
		mParticleForcesConstants.g_fParamsDamping = 0.5f;
		mParticleForcesConstants.g_fParamsShear = 9.1f;
		mParticleForcesConstants.g_fParamsAttraction = -1.025f;

		// OK (Runnable) for ~512k particles @ 64x64x64 grid
		//mParticleForcesConstants.g_fParamsSpring = 5.5f;
		//mParticleForcesConstants.g_fParamsDamping = 0.5f;
		//mParticleForcesConstants.g_fParamsShear = 1.1f;
		//mParticleForcesConstants.g_fParamsAttraction = -1.025f;

		// OK (Runnable) for ~1024k particles @ 64x64x64 grid
		//mParticleForcesConstants.g_fParamsSpring = 1.5f;
		//mParticleForcesConstants.g_fParamsDamping = 0.5f;
		//mParticleForcesConstants.g_fParamsShear = 0.25f;
		//mParticleForcesConstants.g_fParamsAttraction = -0.45f;

		IShader::UpdateBuffer(dc, mParticleForcesBuffer, &mParticleForcesConstants);

		// Particle density shader buffer
		if (FAILED(IShader::CreateBuffer(device, &mParticleDensityBuffer, &mParticleDensityConstants)))
			return false;

		mParticleDensityConstants.g_fDensityCoef = mFluidBehaviorProperties.ParticleMass * 315.0f / (64.0f * XM_PI * pow(mFluidBehaviorProperties.Smoothlen, 9));
		mParticleDensityConstants.g_fSmoothlen = mFluidBehaviorProperties.Smoothlen;
		mParticleDensityConstants.cellSize = mGridBuildConstants.cellSize;
		mParticleDensityConstants.gridSize = mGridBuildConstants.gridSize;
		mParticleDensityConstants.originPosW = mGridBuildConstants.originPosW;
		IShader::UpdateBuffer(dc, mParticleDensityBuffer, &mParticleDensityConstants);

		if (FAILED(IShader::CreateBuffer(device, &mGridIndicesBuffer, &mGridIndicesBuildConstants)))
			return false;

		mGridIndicesBuildConstants.g_iNumElements = numParticles;
		IShader::UpdateBuffer(dc, mGridIndicesBuffer, &mGridIndicesBuildConstants);

		return true;
	}
	else
	{
		MessageBox(nullptr, L"The amount of particles to be created exceeds the particle limit!", L"Error", 0);
		return false;
	}
}

void SphereParticleSystem::Update(ID3D11DeviceContext* dc, double time, float dt)
{
	// Update variables and buffers
	mPerFrameVariables.dt = dt;
	IShader::UpdateBuffer(dc, mPerFrameBuffer, &mPerFrameVariables);

	// Call the generic particle system update function to run compute shader
	//ParticleSystem::Update(dc, time, dt);

	// Run the particle compute shader
// 	dc->CSSetShader(mComputeShader, nullptr, 0);
// 	dc->CSSetConstantBuffers(0, 1, &mConstantsBuffer);
// 	dc->CSSetConstantBuffers(1, 1, &mPerFrameBuffer);
// 	dc->CSSetUnorderedAccessViews(0, 1, &mParticleBufferUAV, NULL);
// 	dc->CSSetUnorderedAccessViews(1, 1, &mGridUAV, NULL);
// 	dc->Dispatch(mNumThreadGroupsX, mNumThreadGroupsY, mNumThreadGroupsZ);
// 
// 	ID3D11UnorderedAccessView* nullUAV = nullptr;
// 	dc->CSSetUnorderedAccessViews(0, 1, &nullUAV, NULL);

	UINT UAVInitialCounts = 0;
	ID3D11UnorderedAccessView* nullUAV = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;

	//------------------------------------------------------------
	// Clear grid indices
	//------------------------------------------------------------
	// First clear the grid indices
	dc->CSSetShader(mGridIndicesClearCS, nullptr, 0);
	dc->CSSetUnorderedAccessViews(0, 1, &mGridIndicesUAV, &UAVInitialCounts);
	dc->Dispatch(GRID_NUM_INDICES / BLOCKSIZE, 1, 1);
	dc->CSSetUnorderedAccessViews(0, 1, &nullUAV, &UAVInitialCounts);

	//------------------------------------------------------------
	// Build the grid
	//------------------------------------------------------------
	dc->CSSetShader(mGridBuildCS, nullptr, 0);
	dc->CSSetConstantBuffers(0, 1, &mGridBuildBuffer);
	dc->CSSetUnorderedAccessViews(0, 1, &mGridKeyValuePairUAV, &UAVInitialCounts);
	dc->CSSetShaderResources(0, 1, &mParticleBufferView);

	dc->Dispatch(mNumParticles / BLOCKSIZE, 1, 1);

	dc->CSSetUnorderedAccessViews(0, 1, &nullUAV, &UAVInitialCounts);
	dc->CSSetShaderResources(0, 1, &nullSRV);

	//------------------------------------------------------------
	// Sort the grid
	//------------------------------------------------------------
	// We sort the grid key-value uint pair by their keys so that the particles with the same cell hash are adjacent to each other
	GPUSort(dc, mGridKeyValuePairUAV, mGridKeyValuePairSRV, mGridKeyValuePairUAV_PingPong, mGridKeyValuePairSRV_PingPong);

	//------------------------------------------------------------
	// Build grid indices
	//------------------------------------------------------------
	dc->CSSetShader(mGridIndicesBuildCS, nullptr, 0);
	dc->CSSetUnorderedAccessViews(0, 1, &mGridIndicesUAV, &UAVInitialCounts);
	dc->CSSetConstantBuffers(0, 1, &mGridIndicesBuffer);
	dc->CSSetShaderResources(0, 1, &mGridKeyValuePairSRV);

	dc->Dispatch(mNumParticles / BLOCKSIZE, 1, 1);

	dc->CSSetUnorderedAccessViews(0, 1, &nullUAV, &UAVInitialCounts);
	dc->CSSetShaderResources(0, 1, &nullSRV);

	//------------------------------------------------------------
	// Rearrange particles
	//------------------------------------------------------------
	dc->CSSetShader(mParticleRearrangeCS, nullptr, 0);
	dc->CSSetUnorderedAccessViews(0, 1, &mSortedParticlesUAV, &UAVInitialCounts); // New sorted particle array
	dc->CSSetShaderResources(0, 1, &mParticleBufferView); // Old unsorted particle array
	dc->CSSetShaderResources(1, 1, &mGridKeyValuePairSRV);

	dc->Dispatch(mNumParticles / BLOCKSIZE, 1, 1);

	dc->CSSetUnorderedAccessViews(0, 1, &nullUAV, &UAVInitialCounts);
	dc->CSSetShaderResources(0, 1, &nullSRV); // Old unsorted particle array
	dc->CSSetShaderResources(1, 1, &nullSRV);

	//------------------------------------------------------------
	// Calculate densities
	//------------------------------------------------------------
// 	dc->CSSetShader(mParticleDensityCS, nullptr, 0);
// 	dc->CSSetShaderResources(0, 1, &mSortedParticlesSRV);
// 	dc->CSSetUnorderedAccessViews(0, 1, &mParticleDensityUAV, &UAVInitialCounts);
// 	dc->CSSetShaderResources(1, 1, &mGridIndicesSRV);
// 	dc->CSSetConstantBuffers(0, 1, &mParticleDensityBuffer);
// 
// 	dc->Dispatch(mNumParticles / BLOCKSIZE, 1, 1);
// 
// 	dc->CSSetShaderResources(0, 1, &nullSRV);
// 	dc->CSSetUnorderedAccessViews(0, 1, &nullUAV, &UAVInitialCounts);
// 	dc->CSSetShaderResources(1, 1, &nullSRV);

	//------------------------------------------------------------
	// Calculate forces
	//------------------------------------------------------------
	dc->CSSetShader(mParticleForcesCS, nullptr, 0);
	dc->CSSetShaderResources(0, 1, &mSortedParticlesSRV);
	dc->CSSetShaderResources(1, 1, &mParticleDensitySRV);
	dc->CSSetUnorderedAccessViews(0, 1, &mParticleForcesUAV, &UAVInitialCounts);
	dc->CSSetShaderResources(2, 1, &mGridIndicesSRV);
	dc->CSSetConstantBuffers(0, 1, &mParticleForcesBuffer);

	dc->Dispatch(mNumParticles / BLOCKSIZE, 1, 1);

	dc->CSSetShaderResources(0, 1, &nullSRV);
	dc->CSSetShaderResources(1, 1, &nullSRV);
	dc->CSSetUnorderedAccessViews(0, 1, &nullUAV, &UAVInitialCounts);
	dc->CSSetShaderResources(2, 1, &nullSRV);

	//------------------------------------------------------------
	// Integrate
	//------------------------------------------------------------
	// Now we must perform collision detection on every particle. Every particle has now been assigned to a cell,
	// therefore we have quick access to the neighboring particles (particles that are in the neighboring cells)

	dc->CSSetShader(mComputeShader, nullptr, 0);
	dc->CSSetConstantBuffers(0, 1, &mConstantsBuffer);
	dc->CSSetConstantBuffers(1, 1, &mPerFrameBuffer);
	dc->CSSetUnorderedAccessViews(0, 1, &mParticleBufferUAV, &UAVInitialCounts);
	dc->CSSetShaderResources(0, 1, &mSortedParticlesSRV);
	//dc->CSSetShaderResources(0, 1, &mParticleBufferView);
	dc->CSSetShaderResources(1, 1, &mParticleForcesSRV);
	//dc->CSSetUnorderedAccessViews(1, 1, &mGridUAV, &UAVInitialCounts);

	//dc->Dispatch(mNumThreadGroupsX, mNumThreadGroupsY, mNumThreadGroupsZ);
	dc->Dispatch(mNumParticles / BLOCKSIZE, 1, 1);

	dc->CSSetUnorderedAccessViews(0, 1, &nullUAV, &UAVInitialCounts);
	dc->CSSetShaderResources(0, 1, &nullSRV);
	dc->CSSetShaderResources(1, 1, &nullSRV);
	//dc->CSSetUnorderedAccessViews(1, 1, &nullUAV, &UAVInitialCounts);

	// Clean-up
// 	dc->CSSetShader(nullptr, nullptr, 0);
// 	for (int i = 0; i < 8; ++i)
// 		dc->CSSetUnorderedAccessViews(i, 1, &nullUAV, nullptr);

	// Now we can proceed to draw the particles using the particle buffer SRV as source in the vertex shader
}

void SphereParticleSystem::Render(ParticleDrawShader* particleDrawShader, ID3D11DeviceContext* dc, const Camera& cam)
{
	// Set vertex and pixel shader
	particleDrawShader->SetActive(dc);

	// Set shader buffers
	particleDrawShader->SetViewProj(cam.GetViewProjMatrix(), cam.GetViewMatrix(), cam.GetProjMatrix());
	particleDrawShader->SetEyePos(cam.GetPosition());
	particleDrawShader->SetParticleRadius(mParticleRadius);

	if (mParticleSRV)
		particleDrawShader->SetDiffuseTexture(dc, mParticleSRV.Get());

	particleDrawShader->UpdatePerFrame(dc);

	// Set vertex shader particle SRV
	dc->VSSetShaderResources(0, 1, &mParticleBufferView);

	dc->IASetInputLayout(nullptr);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	dc->Draw(mNumParticles, 0);
}

//--------------------------------------------------------------------------------------
// GPU Bitonic Sort
// For more information, please see the ComputeShaderSort11 sample
//
// The code is taken from the FluidCS11 sample created by Microsoft Corporation
// Author: Microsoft Corporation (modified by me)
//--------------------------------------------------------------------------------------
void SphereParticleSystem::GPUSort(ID3D11DeviceContext* pd3dImmediateContext,
	ID3D11UnorderedAccessView* inUAV, ID3D11ShaderResourceView* inSRV,
	ID3D11UnorderedAccessView* tempUAV, ID3D11ShaderResourceView* tempSRV)
{
	//pd3dImmediateContext->CSSetConstantBuffers(0, 1, &g_pSortCB);

	const UINT NUM_ELEMENTS = mNumParticles;
	const UINT MATRIX_WIDTH = BITONIC_BLOCK_SIZE;
	const UINT MATRIX_HEIGHT = NUM_ELEMENTS / BITONIC_BLOCK_SIZE;

	ID3D11UnorderedAccessView* nullUAV = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;

	// Sort the data
	// First sort the rows for the levels <= to the block size
	for (UINT level = 2; level <= BITONIC_BLOCK_SIZE; level <<= 1)
	{
		mSortShader->SetActive(pd3dImmediateContext);
		//SortCB constants = { level, level, MATRIX_HEIGHT, MATRIX_WIDTH };
		mSortShader->SetLevelProperties(level, level);
		//pd3dImmediateContext->UpdateSubresource(g_pSortCB, 0, nullptr, &constants, 0, 0);
		mSortShader->UpdatePerFrame(pd3dImmediateContext);

		// Sort the row data
		UINT UAVInitialCounts = 0;
		pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &inUAV, &UAVInitialCounts);
		//pd3dImmediateContext->CSSetShader(g_pSortBitonic, nullptr, 0);
		pd3dImmediateContext->Dispatch(NUM_ELEMENTS / BITONIC_BLOCK_SIZE, 1, 1);
		pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, &UAVInitialCounts);
	}

	UINT UAVInitialCounts = 0;
	pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, &UAVInitialCounts);

	// Then sort the rows and columns for the levels > than the block size
	// Transpose. Sort the Columns. Transpose. Sort the Rows.
	for (UINT level = (BITONIC_BLOCK_SIZE << 1); level <= NUM_ELEMENTS; level <<= 1)
	{
		mTransposeShader->SetActive(pd3dImmediateContext);
		//SortCB constants1 = { (level / BITONIC_BLOCK_SIZE), (level & ~NUM_ELEMENTS) / BITONIC_BLOCK_SIZE, MATRIX_WIDTH, MATRIX_HEIGHT };
		mTransposeShader->SetMatrixProperties(MATRIX_WIDTH, MATRIX_HEIGHT);
		//pd3dImmediateContext->UpdateSubresource(g_pSortCB, 0, nullptr, &constants1, 0, 0);
		mTransposeShader->UpdatePerFrame(pd3dImmediateContext);

		// Transpose the data from buffer 1 into buffer 2
		UINT UAVInitialCounts = 0;
		pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &tempUAV, &UAVInitialCounts);
		pd3dImmediateContext->CSSetShaderResources(0, 1, &inSRV);
		//pd3dImmediateContext->CSSetShader(g_pSortTranspose, nullptr, 0);
		pd3dImmediateContext->Dispatch(MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE, MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE, 1);
		pd3dImmediateContext->CSSetShaderResources(0, 1, &nullSRV);
		pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, &UAVInitialCounts);

		// Sort the transposed column data
		//pd3dImmediateContext->CSSetShader(g_pSortBitonic, nullptr, 0);
		mSortShader->SetActive(pd3dImmediateContext);
		//mSortShader->SetBuffers(pd3dImmediateContext);
		mSortShader->SetLevelProperties(level / BITONIC_BLOCK_SIZE, (level & ~NUM_ELEMENTS) / BITONIC_BLOCK_SIZE);
		mSortShader->UpdatePerFrame(pd3dImmediateContext);
		pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &tempUAV, &UAVInitialCounts);
		pd3dImmediateContext->Dispatch(NUM_ELEMENTS / BITONIC_BLOCK_SIZE, 1, 1);
		pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, &UAVInitialCounts);

		//SortCB constants2 = { BITONIC_BLOCK_SIZE, level, MATRIX_HEIGHT, MATRIX_WIDTH };
		//pd3dImmediateContext->UpdateSubresource(g_pSortCB, 0, nullptr, &constants2, 0, 0);
		//mSortShader->SetLevelProperties(BITONIC_BLOCK_SIZE, level);

		// Transpose the data from buffer 2 back into buffer 1
		mTransposeShader->SetActive(pd3dImmediateContext);
		//mTransposeShader->SetBuffers(pd3dImmediateContext);
		mTransposeShader->SetMatrixProperties(MATRIX_HEIGHT, MATRIX_WIDTH);
		mTransposeShader->UpdatePerFrame(pd3dImmediateContext);
		pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &inUAV, &UAVInitialCounts);
		pd3dImmediateContext->CSSetShaderResources(0, 1, &tempSRV);
		//pd3dImmediateContext->CSSetShader(g_pSortTranspose, nullptr, 0);
		pd3dImmediateContext->Dispatch(MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE, MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE, 1);
		pd3dImmediateContext->CSSetShaderResources(0, 1, &nullSRV);
		pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, &UAVInitialCounts);

		// Sort the row data
		//pd3dImmediateContext->CSSetShader(g_pSortBitonic, nullptr, 0);
		mSortShader->SetActive(pd3dImmediateContext);
		mSortShader->SetLevelProperties(BITONIC_BLOCK_SIZE, level);
		mSortShader->UpdatePerFrame(pd3dImmediateContext);
		pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &inUAV, &UAVInitialCounts);
		pd3dImmediateContext->Dispatch(NUM_ELEMENTS / BITONIC_BLOCK_SIZE, 1, 1);
		pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, &UAVInitialCounts);
	}

	pd3dImmediateContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, &UAVInitialCounts);
	pd3dImmediateContext->CSSetShaderResources(0, 1, &nullSRV);
}

void SphereParticleSystem::SetFluidBehaviorProperties(FLOAT smoothlen, FLOAT pressureStiffnes, FLOAT restDensity, FLOAT particleMass, FLOAT viscosity)
{
	mFluidBehaviorProperties.Smoothlen = smoothlen;
	mFluidBehaviorProperties.PressureStiffness = pressureStiffnes;
	mFluidBehaviorProperties.RestDensity = restDensity;
	mFluidBehaviorProperties.ParticleMass = particleMass;
	mFluidBehaviorProperties.Viscosity = viscosity;
}

void SphereParticleSystem::SetShaders(BitonicSortShader* sortShader, MatrixTransposeShader* transposeShader, ID3D11ComputeShader* particleRearrangeShader,
	ID3D11ComputeShader* gridBuildShader, ID3D11ComputeShader* particleForcesShader,
	ID3D11ComputeShader* particleDensityShader, ID3D11ComputeShader* gridIndicesClearShader,
	ID3D11ComputeShader* gridIndicesBuildShader)
{
	mSortShader = sortShader;
	mTransposeShader = transposeShader;
	mParticleRearrangeCS = particleRearrangeShader;
	mGridBuildCS = gridBuildShader;
	mParticleForcesCS = particleForcesShader;
	mParticleDensityCS = particleDensityShader;
	mGridIndicesClearCS = gridIndicesClearShader;
	mGridIndicesBuildCS = gridIndicesBuildShader;
}