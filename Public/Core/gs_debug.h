// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

//#include "GradientspacePlatform.h"
//// gross...why do we need this again?
//#ifdef WITH_ENGINE
//#include "Windows/AllowWindowsPlatformTypes.h"
//#endif
////#include "windows.h"
//// only on WIN32 platform...
//#include <debugapi.h>
//
//#ifdef WITH_ENGINE
//#include "Windows/HideWindowsPlatformTypes.h"
//#endif

#if defined(__linux__) || defined(__APPLE__)
#define PLATFORM_BREAK() (__builtin_trap())
#else
// seems like this is all we need on windows, and does not bring in windows.h garbage
#include <intrin.h>
#define PLATFORM_BREAK() (__nop(), __debugbreak())
#endif

#define gs_debug_assert( expr )   { if (!(expr)) PLATFORM_BREAK(); }
#define gs_runtime_assert( expr ) { if (!(expr)) PLATFORM_BREAK(); }

#if defined(__linux__) || defined(__APPLE__)
	#define GS_DISABLE_OPTIMIZATION
	#define GS_ENABLE_OPTIMIZATION
#else
	#define GS_DISABLE_OPTIMIZATION __pragma(optimize("",off))
	#define GS_ENABLE_OPTIMIZATION __pragma(optimize("",on))
#endif


#ifdef GSUE_LINKING_INSIDE_UE
#define GS_LOG(Message, ...) UE_LOG(LogTemp, Warning, TEXT(Message), ##__VA_ARGS__)
#else
#define GS_LOG(Message, ...) printf(Message, ##__VA_ARGS__)
#endif
