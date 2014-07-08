#ifndef SHARED_PLATFORM_H
#define SHARED_PLATFORM_H

// Platform detection
#ifdef _WIN32
	#define PLATFORM_WINDOWS
#endif

#ifdef PLATFORM_WINDOWS
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#include <Windows.h>
#endif

#ifdef PLATFORM_WINDOWS
	#ifdef COMPILING_DLL
		#define DLL_API __declspec(dllexport)
	#else
		#define DLL_API __declspec(dllimport)
	#endif
#else
	#define DLL_API
#endif

#endif
