#ifndef nekoPrecompiledHeader_h__
#define nekoPrecompiledHeader_h__


/********************************************************************
	filename: 	nekoPrecompiledHeader.h
	author:		kernys
	
	purpose:	미리 컴파일된 헤더입니다.
*********************************************************************/

#define _HAS_ITERATOR_DEBUGGING 0
#include <assert.h>
#include <string>
using namespace std;
#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#pragma warning(disable:4251)

#include "nekoDefined.h"
#include "nekoDataTypes.h"
#include "nekoAutoPtr.h"
#include "nekoPoint.h"
#include "nekoRectangle.h"


#endif