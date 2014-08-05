struct Particle
{
	float3 position;
	float3 velocity;
	/*
	float3 acceleration;
	float mass;
	float time;
	*/
};

struct SphereParticle
{
	float3 position;
	float3 velocity;
	/*
	float3 acceleration;
	float mass;
	float time;
	float radius;
	uint gridHash;
	*/
};

struct ParticleForces
{
	float3 acceleration;
};

struct ParticleDensity
{
	float density;
};