// ===============================================================
//  nekoLockSection - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoLockSection_h__
#define __nekoLockSection_h__

namespace neko
{
	class nekoLockSection
	{
	protected:

		/// Critical Section
		CRITICAL_SECTION mCS;

	public:

		/// Constructor
		nekoLockSection()
		{
			InitializeCriticalSection(&mCS);
		}

		/// Destructor
		~nekoLockSection()
		{
			DeleteCriticalSection(&mCS);
		}

		/// Begin
		inline void Begin()
		{
			EnterCriticalSection(&mCS);
		}

		/// End
		inline void End()
		{
			LeaveCriticalSection(&mCS);
		}

	};
}


#endif // __nekoLockSection_h__