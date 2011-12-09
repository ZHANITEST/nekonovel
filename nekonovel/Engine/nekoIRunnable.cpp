#include "nekoPrecompiledHeader.h"
#include "nekoIRunnable.h"

namespace neko
{
	/// Entry point
	static unsigned __stdcall _ThreadEntry(void *arg)
	{
		return ((nekoIRunnable *)arg)->Do();
	}

	/// Start
	result nekoIRunnable::Start()
	{
		if(mThread)
			return E_ALREADY;

		mValid = true;

		unsigned int id;
		mThread = (HANDLE)_beginthreadex(0, 0, _ThreadEntry, this, 0, &id);

		return S_OK;
	}
}