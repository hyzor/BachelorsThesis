#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdint>

#include "platform.h"

#ifdef PLATFORM_WINDOWS
#include <psapi.h>
#endif


#include "util.h"
#include <float.h>

// Must be included last!
#include "debug.h"

uint64_t Utils::GetMemoryUsage()
{
#ifdef PLATFORM_WINDOWS
	PROCESS_MEMORY_COUNTERS info;

	if (GetProcessMemoryInfo(GetCurrentProcess(), &info, sizeof(info)) == 0)
		return 0;

	return (uint64_t)info.WorkingSetSize;
#else
#error Unsupported OS
#endif
}

double Utils::GetTime()
{
#ifdef PLATFORM_WINDOWS
	static LARGE_INTEGER frequency;

	if (!frequency.QuadPart)
		QueryPerformanceFrequency(&frequency);

	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);

	return (double)counter.QuadPart / frequency.QuadPart;
#else
#error Unsupported OS
#endif
}

float MathHelper::DegreesToRadians(float degrees)
{
	return (float)(degrees * (M_PI / 180.0));
}

float MathHelper::GetInfinity()
{
	return FLT_MAX;
}

float MathHelper::GetPi()
{
	return 3.1415926535f;
}