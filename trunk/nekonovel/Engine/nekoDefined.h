#ifndef nekoDefined_h__
#define nekoDefined_h__

#include <assert.h>

/********************************************************************
	filename: 	nekoDefined.h
	author:		kernys
	
	purpose:	
*********************************************************************/

namespace neko
{
#ifdef NEKONOVEL_BUILD
#	define NEKONOVEL_API __declspec(dllexport)
#else
#	define NEKONOVEL_API __declspec(dllimport)
#endif

#define NEKO_CLASSNAME		"NekoEngine"
#define NEKO_WINDOWNAME		"NekoEngine"
#define NEKO_DEFFONT_NAME	"굴림체"
#define NEKO_DEFFONT_HEIGHT	16

#define nekoAssert(x, msg) assert((x) && msg)


	enum
	{
		E_NOT_SUPPORTED = -1024,
		E_NOT_EXIST,
		E_ALREADY,
		E_FAIL_,
		E_NEED_WAIT, // 기다림이 필요해!
		E_TOO_LONG, // 너무 길다!

		S_OK_ = 0,
	};

/// Debugheap Allocation 
#if (_DEBUG)
#define nekoNew new(_CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define nekoNew new
#endif
/// Log output Macro function.
#define LogPrint /*nekoLogger::SetInformation(__FILE__, __LINE__) && */nekoLogger::Log

/// Is Float zero ?
#define IsFloatZero(x) (abs(x) <= 0.0001f)

/// Is Float same ?
#define IsFloatSame(x, y) (abs(x - y) <= 0.0001f)

#define REAL_Z_DIFF 0.00005f

/// Safe Release Macro function.
#define SafeRelease(x) if(x) { x->Release(); x = 0; }
#define SafeDelete(x) if(x) { delete x; x = 0; }
#define SafeDeleteArray(x) if(x) { delete []x; x = 0; }

#define MATH_PI (float64)3.141592654
inline DWORD FtoDW(float f) { return *((DWORD*)&f); }

#define CONVRGB(c) ((uint32)(((((c) >> 8) & 0xff) << 8)|(((c) >> 16) & 0xff)|(((c) & 0xff) << 16)))
#define MixColorsAlpha(c, alpha) ( ((((int32)(((c) >> 24) & 0xff)) * (alpha) / 255) << 24) | ((c) & 0xffffff) )

#pragma warning(disable:4251)
#pragma warning(disable:4275)
}


#endif