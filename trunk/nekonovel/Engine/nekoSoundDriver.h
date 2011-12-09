// ===============================================================
//  nekoSoundDriver - 2009.7.6
// ===============================================================
// - Purpose : 
// ---------------------------------------------------------------

#ifndef __nekoSoundDriver_h__
#define __nekoSoundDriver_h__

#include <audiere.h>
using namespace audiere;
#include "nekoLockSection.h"

namespace neko
{
	class nekoSoundDriver : public nekoIUnknown
	{
	protected:

		AudioDevicePtr mDevice;

		nekoLockSection mLockSection;

	public:
		
		/// Constructor
		nekoSoundDriver();
		
		/// Destructor
		virtual ~nekoSoundDriver();

		/// Initialize
		result Initialize();

		/// Get Handle
		inline AudioDevicePtr &GetHandle() { return mDevice; }

		/// Update
		void Update(float32 frameTime);

		/// Lock
		inline void Lock()
		{
			mLockSection.Begin();
		}

		/// Unlock
		inline void Unlock()
		{
			mLockSection.End();
		}
		
	};
}


#endif // __nekoSoundDriver_h__