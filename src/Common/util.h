#ifndef SHARED_UTIL_H
#define SHARED_UTIL_H

#include <cstdint>
#include <DirectXMath.h>

#include "platform.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0])) // Macro for finding the size of an array
#define ReleaseCOM(x) { if (x) { x->Release(); x = 0; } } // Macro for releasing COM objects
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x]))))) // Macro for getting the number of elements from an array

class DLL_API Utils
{
public:
	// Returns the application's memory usage in bytes.
	static uint64_t GetMemoryUsage();

	// Returns the current time in seconds.
	static double GetTime();
};

class DLL_API MathHelper
{
public:
	static float DegreesToRadians(float degrees);

	static float GetInfinity();
	static float GetPi();
	static float GetGravityConstant();

	template <typename T>
	static T Lerp(T a, T b, float amount);
};

template <typename T>
T MathHelper::Lerp(T a, T b, float amount)
{
	return a + (b - a) * amount;
}

#endif
