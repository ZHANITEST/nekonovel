// ===============================================================
//  nekoIRunnable - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoIRunnable_h__
#define __nekoIRunnable_h__

#include <process.h>

namespace neko
{
	class nekoIRunnable : public nekoIUnknown
	{
	public:

		/// Constructor
		nekoIRunnable()
			: mThread(0)
			, mValid(false)
		{

		}

		/// Destructor
		virtual ~nekoIRunnable()
		{
			Stop();
		}

		/// Do something
		virtual uint32 Do() { return 0; };

		/// Start
		virtual result Start();

		/// Stop the thread
		virtual void Stop(bool wait = true)
		{
			if(mThread == 0)
				return;

			mValid = false;
			if(wait)
				WaitForSingleObject(mThread, INFINITE);

			mThread = 0;
		}


		/// Terminate the thread
		virtual void Terminiate()
		{
			if(mThread == 0)
				return;

			mValid = false;
			CloseHandle(mThread);

			mThread = 0;
		}

	protected:

		/// Thread handle
		HANDLE mThread;

		/// Valid ?
		bool mValid;
	};
}


#endif // __nekoIRunnable_h__