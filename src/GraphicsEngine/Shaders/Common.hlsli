#define ALPHA_BLENDING 0
#define ADDITIVE_BLENDING 1

#define MAX_MOTIONBLURSAMPLES 32

//http://developer.download.nvidia.com/SDK/10.5/direct3d/Source/SoftParticles/doc/SoftParticles_hi.pdf
float Contrast(float Input, float ContrastPower)
{
#if 1
	//piecewise contrast function
	bool IsAboveHalf = Input > 0.5;
	float ToRaise = saturate(2 * (IsAboveHalf ? 1 - Input : Input));
	float Output = 0.5*pow(ToRaise, ContrastPower);
	Output = IsAboveHalf ? 1 - Output : Output;
	return Output;
#else
	// another solution to create a kind of contrast function
	return 1.0 - exp2(-2 * pow(2.0*saturate(Input), ContrastPower));
#endif
}

//http://frictionalgames.blogspot.se/2012/09/tech-feature-hdr-lightning.html
float3 Uncharted2Tonemap(float3 x)
{
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;

	return ((x*(A*x + C*B) + D*E) / (x*(A*x + B) + D*F)) - E / F;
};