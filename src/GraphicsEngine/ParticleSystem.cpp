#include "ParticleSystem.h"

#include <random>

Particle::Particle()
{
	mPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mAcceleration = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mDirection = XMFLOAT3(0.0f, -1.0f, 0.0f);
	mMass = 1.0f;
	mTime = 0.0f;
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

void Particle::SetDirection(float x, float y, float z)
{
	mDirection = XMFLOAT3(x, y, z);
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
}

SphereParticleSystem::~SphereParticleSystem()
{
	if (mSphere)
	{
		ReleaseCOM(mSphere->vertexBuffer);
		ReleaseCOM(mSphere->indexBuffer);
		delete mSphere;
	}
}

bool SphereParticleSystem::Init(ID3D11Device* device, float sphereRadius, int numSphereSlices, int numSphereStacks, UINT maxParticles)
{
	mNumParticles = 0;

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

bool SphereParticleSystem::CreateParticles(ID3D11Device* device, UINT numParticles)
{
	if (numParticles <= mMaxParticles)
	{
		mNumParticles = numParticles;

		SphereParticle* particles = new SphereParticle[numParticles];

		float rndX, rndY, rndZ;
		std::tr1::mt19937 eng;
		std::tr1::uniform_real_distribution<float> dist(-50.0f, 50.0f);

		float velRange = 100.0f;

		for (UINT i = 0; i < numParticles; ++i)
		{
			rndX = dist(eng);
			rndY = dist(eng);
			rndZ = dist(eng);

// 			float velX = ((float(rand()) / float(RAND_MAX)) * (velRange - (-velRange))) + (-velRange);
// 			float velY = ((float(rand()) / float(RAND_MAX)) * (velRange - (-velRange))) + (-velRange);
// 			float velZ = ((float(rand()) / float(RAND_MAX)) * (velRange - (-velRange))) + (-velRange);

			particles[i].SetPosition(rndX, rndY, rndZ);
			//particles[i].SetVelocity(velX, velY, velZ);
			particles[i].SetVelocity(rndX, rndY, rndZ);
			particles[i].SetRadius(mSphere->radius);
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
	// Update variables and buffers
	mPerFrameVariables.dt = dt;
	mPerFrameVariables.gravity = MathHelper::GetGravityConstant();
	mPerFrameVariables.boundaries.x = mPerFrameVariables.boundaries.y = mPerFrameVariables.boundaries.z = 200.0f;
	IShader::UpdateBuffer(dc, mPerFrameBuffer, &mPerFrameVariables);

	mConstantVariables.groupDimX = mNumThreadGroupsX;
	mConstantVariables.groupDimY = mNumThreadGroupsY;
	mConstantVariables.numParticles = mNumParticles;
	IShader::UpdateBuffer(dc, mConstantsBuffer, &mConstantVariables);

	// Call the generic particle system update function to run compute shader
	ParticleSystem::Update(dc, time, dt);

	// Now we can proceed to draw the particles using the particle buffer SRV as source in the vertex shader
}

void SphereParticleSystem::Render(ParticleDrawShader* particleDrawShader, ID3D11DeviceContext* dc, const Camera& cam)
{
	// Set vertex and pixel shader
	particleDrawShader->SetActive(dc);

	// Set shader buffers
	particleDrawShader->SetViewProj(cam.GetViewProjMatrix());
	particleDrawShader->UpdatePerFrame(dc);

	// Set vertex shader particle SRV
	dc->VSSetShaderResources(0, 1, &mParticleBufferView);

	dc->IASetInputLayout(nullptr);
	dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	dc->Draw(mNumParticles, 0);
}
