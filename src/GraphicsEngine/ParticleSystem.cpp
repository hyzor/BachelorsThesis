#include "ParticleSystem.h"

Particle::Particle()
{
	mPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
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
	mRadius = 1.0f;
}

SphereParticle::SphereParticle(float x, float y, float z, float radius)
	: Particle(x, y, z)
{
	mRadius = radius;
}

SphereParticle::~SphereParticle(){}

const float SphereParticle::GetRadius()
{
	return mRadius;
}

void SphereParticle::SetRadius(float radius)
{
	mRadius = radius;
}

ParticleSystem::ParticleSystem()
{
	mComputeShader = nullptr;
	mParticleBuffer = nullptr;
	mParticleBufferView = nullptr;
	mParticleBufferUAV = nullptr;

	mConstantsBuffer = nullptr;
	mPerFrameBuffer = nullptr;
}

ParticleSystem::~ParticleSystem()
{
	ReleaseCOM(mComputeShader);
	ReleaseCOM(mParticleBuffer);
	ReleaseCOM(mParticleBufferView);
	ReleaseCOM(mParticleBufferUAV);

	ReleaseCOM(mConstantsBuffer);
	ReleaseCOM(mPerFrameBuffer);
}

void ParticleSystem::Init(int maxParticles)
{
	mMaxParticles = maxParticles;
}

void ParticleSystem::Init(int maxParticles, ID3D11ComputeShader* computeShader)
{
	mMaxParticles = maxParticles;
	mComputeShader = computeShader;
}

bool ParticleSystem::CreateParticles(int numParticles)
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

void ParticleSystem::CalculateThreadGroups(int numParticles)
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

SphereParticleSystem::SphereParticleSystem(){}

SphereParticleSystem::~SphereParticleSystem(){}

void SphereParticleSystem::Init(float sphereRadius, int numSphereSlices, int numSphereStacks, int maxParticles)
{
	mSphereProperties.sphereRadius = sphereRadius;
	mSphereProperties.numSlices = numSphereSlices;
	mSphereProperties.numStacks = numSphereStacks;
	mMaxParticles = maxParticles;

	GeometryGenerator geoGen;
	geoGen.CreateSphere(mSphereProperties.sphereRadius,
		mSphereProperties.numSlices, mSphereProperties.numStacks, mSphere);
}

void SphereParticleSystem::Init(ID3D11ComputeShader* computeShader, float sphereRadius, int numSphereSlices, int numSphereStacks, int maxParticles)
{
	Init(sphereRadius, numSphereSlices, numSphereStacks, maxParticles);
	mComputeShader = computeShader;

	ZeroMemory(&mConstantVariables, sizeof(mConstantVariables));
	ZeroMemory(&mPerFrameVariables, sizeof(mPerFrameVariables));
}

bool SphereParticleSystem::CreateParticles(ID3D11Device* device, int numParticles)
{
	if (numParticles <= mMaxParticles)
	{
		SphereParticle* particles = new SphereParticle[numParticles];

		for (int i = 0; i < numParticles; ++i)
		{
			particles[i].SetPosition(0.0f, 0.0f, 0.0f);
			particles[i].SetRadius(mSphereProperties.sphereRadius);
		}

		CalculateThreadGroups(numParticles);

		// Create compute shader input buffer
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		bufferDesc.ByteWidth = sizeof(SphereParticle) * numParticles;
		bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		bufferDesc.StructureByteStride = sizeof(SphereParticle);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;

		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = particles;

		if (FAILED(device->CreateBuffer(&bufferDesc, &data, &mParticleBuffer)))
		{
			MessageBox(nullptr, L"Could not create source particle buffer for compute shaders!", L"Error", 0);
			return false;
		}

		ZeroMemory(&bufferDesc, sizeof(bufferDesc));
		mParticleBuffer->GetDesc(&bufferDesc);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.Buffer.NumElements = numParticles;

		if (FAILED(device->CreateShaderResourceView(mParticleBuffer, &srvDesc, &mParticleBufferView)))
		{
			MessageBox(nullptr, L"Could not create source particle buffer view for compute shaders!", L"Error", 0);
			return false;
		}

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory(&uavDesc, sizeof(uavDesc));
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.Flags = 0;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.Buffer.NumElements = numParticles;

		if (FAILED(device->CreateUnorderedAccessView(mParticleBuffer, &uavDesc, &mParticleBufferUAV)))
		{
			MessageBox(nullptr, L"Could not create source particle buffer UAV for compute shaders!", L"Error", 0);
			return false;
		}

		delete[] particles;

		mConstantVariables.groupDimX = mNumThreadGroupsX;
		mConstantVariables.groupDimY = mNumThreadGroupsY;
		mConstantVariables.numParticles = numParticles;		
		if (FAILED(IShader::CreateBuffer(device, &mConstantsBuffer, &mConstantVariables)))
		{
			MessageBox(NULL, L"Could not create constant buffer for constant variables for the compute shader!", L"Error", 0);
			return false;
		}

		if (FAILED(IShader::CreateBuffer(device, &mPerFrameBuffer, &mPerFrameVariables)))
		{
			MessageBox(NULL, L"Could not create constant buffer for per frame variables for the compute shader!", L"Error", 0);
			return false;
		}

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
	// Update variables
	mPerFrameVariables.frameTime = dt;
	IShader::UpdateBuffer(dc, mPerFrameBuffer, &mPerFrameVariables);

	// Run the particle compute shader
	dc->CSSetShader(mComputeShader, nullptr, 0);
	dc->CSSetConstantBuffers(0, 1, &mConstantsBuffer);
	dc->CSSetConstantBuffers(1, 1, &mPerFrameBuffer);
	dc->CSSetUnorderedAccessViews(0, 1, &mParticleBufferUAV, NULL);
	dc->Dispatch(mNumThreadGroupsX, mNumThreadGroupsY, mNumThreadGroupsZ);

	ID3D11UnorderedAccessView* nullUAV = nullptr;
	dc->CSSetUnorderedAccessViews(0, 1, &nullUAV, NULL);

	// Now we can proceed to draw the particles using the particle buffer SRV as source in the vertex shader
}

void SphereParticleSystem::Render()
{
	// Set vertex and pixel shader

	// Set shader buffers

	// Set vertex shader particle SRV
	//dc->VSSetShaderResources(0, 1, &mParticleBufferView);
}
