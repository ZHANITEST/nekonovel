#pragma once

using namespace System;
using namespace System::IO;

namespace ParticleTool
{
	static char *lastAllocated = NULL;
	inline const char8 *ConvertToChar(System::String^ str)
	{
		if(lastAllocated)
			System::Runtime::InteropServices::Marshal::FreeHGlobal((IntPtr)lastAllocated);
		return (lastAllocated = (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(str));
	}

	inline void ClearHGlobal()
	{
		if(lastAllocated)
			System::Runtime::InteropServices::Marshal::FreeHGlobal((IntPtr)lastAllocated);
	}

	inline String ^ConvertToString(const char *str)
	{
		return gcnew String(str);
	}
}